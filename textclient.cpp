/* xscreensaver, Copyright(c) 2012 Jamie Zawinski <jwz@jwz.org>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  No representations are made about the suitability of this
 * software for any purpose.  It is provided "as is" without express or 
 * implied warranty.
 *
 * Running programs under a pipe or pty and returning bytes from them.
 * Uses these X resources:
 * 
 * program:     What to run.  Usually "xscreensaver-text".
 * relaunchDelay: secs  How long after the command dies before restarting.
 * usePty: bool     Whether to run the command interactively.
 * metaSendsESC: bool   Whether to send Alt-x as ESC x in pty-mode.
 * swapBSDEL: bool  Swap Backspace and Delete in pty-mode.
 */

#include "xws2win.h"
#include <vector>
#include "mzc2mini.h"

#include <cstdio>
using namespace std;

/*#define DEBUG*/

EXTERN_C const char *progname;
EXTERN_C char *program;
EXTERN_C int relaunchDelay;

int tc_fprintf(FILE *fp, char *fmt, ...)
{
    CHAR sz[512];
    va_list va;
    int n;
    va_start(va, fmt);
    n = wvsprintfA(sz, fmt, va);
    va_end(va);
    OutputDebugStringA(sz);
    return n;
}
#define fprintf tc_fprintf

struct text_data
{
    Display *dpy;
    char *program;
    int pix_w, pix_h, char_w, char_h;

    MFile *pipeInput;
    MFile *pipeOutput;
    MProcessMaker *pmaker;
    Bool input_available_p;
    DWORD subproc_relaunch_delay;
    DWORD dwTick;

    const char *out_buffer;
    int out_column;
};

static void launch_text_generator(text_data *d)
{
    const char *oprogram = d->program;
    char program[MAX_PATH * 3], program2[MAX_PATH * 3];
    char comspec[MAX_PATH];

    if (lstrcmpiA(oprogram, "xscreensaver-text") == 0 ||
        lstrcmpiA(oprogram, "xscreensaver-text.exe") == 0)
    {
        sprintf(program, "xscreensaver-text --cols %d", d->char_w);
    }
    else
    {
        strcpy(program, oprogram);
    }
    fprintf(stderr, "%s\n", program);

    d->pmaker->SetShowWindow(SW_HIDE);
    d->pmaker->SetCreationFlags(CREATE_NEW_CONSOLE);
    if (d->pmaker->PrepareForRedirect(
        &d->pipeInput->m_hHandle, &d->pipeOutput->m_hHandle, &d->pipeOutput->m_hHandle))
    {
        BOOL bOK;
        CHAR *p, szDir[MAX_PATH];
# ifdef DEBUG
        fprintf(stderr, "%s: textclient: launch pipe: %s\n", progname, program);
# endif
        GetModuleFileNameA(NULL, szDir, MAX_PATH);
        p = strrchr(szDir, '\\');
        *p = '\0';
        d->pmaker->SetCurrentDirectory(szDir);
        bOK = d->pmaker->CreateProcess(NULL, program);
        if (!bOK)
        {
            // retry
            strcpy(p, "\\..");
            d->pmaker->SetCurrentDirectory(szDir);
            bOK = d->pmaker->CreateProcess(NULL, program);
        }
        if (!bOK)
        {
            // and retry again
            GetEnvironmentVariableA("COMSPEC", comspec, MAX_PATH);
            wsprintfA(program2, "\"%s\" /C %s", comspec, program);
            bOK = d->pmaker->CreateProcess(NULL, program2);
        }
        if (bOK)
        {
# ifdef DEBUG
            fprintf(stderr, "%s: textclient: CreateProcess\n", progname);
# endif
            d->input_available_p = True;
        }
    }

    d->dwTick = 0;
}

static void relaunch_generator_timer(void *closure)
{
    text_data *d = (text_data *)closure;
#ifdef DEBUG
    fprintf(stderr, "%s: textclient: launch timer fired\n", progname);
#endif
    launch_text_generator(d);
}


EXTERN_C void textclient_reshape(text_data *d,
    int pix_w, int pix_h, int char_w, int char_h)
{
    /* If we're running xscreensaver-text, then kill and restart it any
       time the window is resized so that it gets an updated --cols arg
       right away.  But if we're running something else, leave it alone.
       */
    if (!strcmp(d->program, "xscreensaver-text"))
    {
        d->pipeInput->CloseHandle();
        d->pipeOutput->CloseHandle();
        d->pmaker->Close();
        d->input_available_p = False;
        relaunch_generator_timer(d);
    }
}


EXTERN_C text_data *textclient_open(Display *dpy)
{
    text_data *d = (text_data *) calloc(1, sizeof(*d));

#ifdef DEBUG
    fprintf(stderr, "%s: textclient: init\n", progname);
#endif

    d->dpy = dpy;

    d->subproc_relaunch_delay = (1000 * relaunchDelay);

    //d->program = get_string_resource(dpy, "program", "Program");
    d->program = _strdup(program);

    d->pipeInput = new MFile;
    d->pipeOutput = new MFile;
    d->pmaker = new MProcessMaker;

    launch_text_generator(d);

    return d;
}


EXTERN_C void textclient_close(text_data *d)
{
# ifdef DEBUG
    fprintf(stderr, "%s: textclient: free\n", progname);
# endif
    if (d->pmaker->IsRunning())
        d->pmaker->TerminateProcess(-1);

    if (d->program)
        free(d->program);
    delete d->pipeInput;
    delete d->pipeOutput;
    delete d->pmaker;
    memset(d, 0, sizeof(*d));
    free(d);
}

EXTERN_C int textclient_getc(text_data *d)
{
    int ret = -1;

    if (d->dwTick != 0)
    {
        if (GetTickCount() - d->dwTick > d->subproc_relaunch_delay)
        {
            relaunch_generator_timer(d);
            d->dwTick = 0;
        }
    }

    if (d->out_buffer && *d->out_buffer)
    {
        ret = *d->out_buffer;
        d->out_buffer++;
    }
    else if (d->input_available_p)
    {
        unsigned char s[2];
        DWORD n;

        //fprintf(stderr, "getc: waiting\n");
        d->pipeOutput->PeekNamedPipe(s, 1, &n);
        //fprintf(stderr, "getc: done\n");

        if (n == 1)
        {
            d->pipeOutput->ReadFile(s, 1, &n);
            ret = s[0];
            fprintf(stderr, "getc: '%c'\n", s[0]);
        }
        else if (!d->pmaker->IsRunning())
        {
            d->pipeInput->CloseHandle();
            d->pipeOutput->CloseHandle();

# ifdef DEBUG
            fprintf(stderr, "%s: textclient: pclose\n", progname);
# endif
            d->pmaker->TerminateProcess(-1);
            d->pmaker->Close();

            if (d->out_column > 0)
            {
# ifdef DEBUG
                fprintf(stderr, "%s: textclient: adding blank line at EOF\n",
                        progname);
# endif
                d->out_buffer = "\r\n\r\n";
            }

# ifdef DEBUG
            fprintf(stderr, "%s: textclient: relaunching in %d\n", progname,
                   (int) d->subproc_relaunch_delay);
# endif
            d->dwTick = GetTickCount();
            d->input_available_p = False;
        }
    }

    if (ret == '\r' || ret == '\n')
        d->out_column = 0;
    else if (ret > 0)
        d->out_column++;

# ifdef DEBUG
    if (ret <= 0)
        fprintf(stderr, "%s: textclient: getc: %d\n", progname, ret);
    else if (ret < ' ')
        fprintf(stderr, "%s: textclient: getc: %03o\n", progname, ret);
    else
        fprintf(stderr, "%s: textclient: getc: '%c'\n", progname, (char) ret);
# endif

    return ret;
}

EXTERN_C Bool textclient_putc(text_data *d, int c)
{
    DWORD cb;

# ifdef DEBUG
    fprintf(stderr, "%s: textclient: putc '%c'\n", progname, (char) c);
# endif

    return d->pipeInput->WriteFile(&c, 1, &cb);
}
