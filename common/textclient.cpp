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
#include <strsafe.h>
using namespace std;

EXTERN_C const char *progname;
EXTERN_C char *program;
EXTERN_C int relaunchDelay;

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
    Bool  awaiting_relaunch_p;
    DWORD subproc_died_tick;

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
        StringCchPrintfA(program, _countof(program), "xscreensaver-text --cols %d", d->char_w);
    }
    else
    {
        StringCchCopyA(program, _countof(program), oprogram);
    }
    fprintf(stderr, "%s\n", program);

    d->pmaker->Close();
    d->pmaker->SetShowWindow(SW_HIDE);
    d->pmaker->SetCreationFlags(CREATE_NEW_CONSOLE);
    if (d->pmaker->PrepareForRedirect(
        &d->pipeInput->m_hHandle, &d->pipeOutput->m_hHandle, &d->pipeOutput->m_hHandle))
    {
        BOOL bOK;
        CHAR *p, szDir[MAX_PATH];
        fprintf(stderr, "%s: textclient: launch pipe: %s\n", progname, program);
        GetModuleFileNameA(NULL, szDir, MAX_PATH);
        p = strrchr(szDir, '\\');
        if (p)
            *p = 0;
        d->pmaker->SetCurrentDirectory(szDir);
        bOK = d->pmaker->CreateProcess(NULL, program);
        if (!bOK)
        {
            // retry
            p = strrchr(szDir, '\\');
            if (p)
                *p = 0;
            d->pmaker->SetCurrentDirectory(szDir);
            bOK = d->pmaker->CreateProcess(NULL, program);
        }
        if (!bOK)
        {
            // and retry again
            if (p)
                *p = 0;
            GetEnvironmentVariableA("COMSPEC", comspec, MAX_PATH);
            StringCchPrintfA(program2, _countof(program2), "\"%s\" /C %s", comspec, program);
            bOK = d->pmaker->CreateProcess(NULL, program2);
        }
        if (bOK)
        {
            fprintf(stderr, "%s: textclient: CreateProcess\n", progname);
            d->input_available_p = True;
            d->awaiting_relaunch_p = False;
        }
        else
        {
            d->awaiting_relaunch_p = True;
            d->subproc_died_tick = GetTickCount();
        }
    }
    else
    {
        d->subproc_died_tick = GetTickCount();
    }
}

static void relaunch_generator_timer(void *closure)
{
    text_data *d = (text_data *)closure;
    fprintf(stderr, "%s: textclient: launch timer fired\n", progname);
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

    fprintf(stderr, "%s: textclient: init\n", progname);

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
    fprintf(stderr, "%s: textclient: free: %lu\n", progname, GetTickCount());

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

    if (d->awaiting_relaunch_p)
    {
        DWORD elapsed = GetTickCount() - d->subproc_died_tick;
        fprintf(stderr, "%s: textclient: waiting relaunch (%lu / %lu ms): %lu\n",
                progname, elapsed, d->subproc_relaunch_delay, GetTickCount());
        if (elapsed > d->subproc_relaunch_delay)
        {
            relaunch_generator_timer(d);
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
        BOOL peek_ok;

        peek_ok = d->pipeOutput->PeekNamedPipe(s, 1, &n);

        if (peek_ok && n == 1)
        {
            d->pipeOutput->ReadFile(s, 1, &n);
            ret = s[0];
        }

        if (ret < 0 && (!peek_ok || !d->pmaker->IsRunning()))
        {
            d->pmaker->TerminateProcess(-1);
            if (!!*d->pipeInput)  d->pipeInput->CloseHandle();
            if (!!*d->pipeOutput) d->pipeOutput->CloseHandle();
            d->pmaker->Close();

            d->awaiting_relaunch_p = True;
            d->subproc_died_tick = GetTickCount();
            d->input_available_p = False;
            fprintf(stderr, "%s: textclient: EOF detected, subproc_died_tick=%lu\n", progname,
                    d->subproc_died_tick);
        }
    }

    if (ret == '\r' || ret == '\n')
        d->out_column = 0;
    else if (ret > 0)
        d->out_column++;

    if (ret <= 0)
        fprintf(stderr, "%s: textclient: getc: %d\n", progname, ret);
    else if (ret < ' ')
        fprintf(stderr, "%s: textclient: getc: 0x%02x\n", progname, ret);
    else
        fprintf(stderr, "%s: textclient: getc: '%c'\n", progname, (char) ret);

    return ret;
}

EXTERN_C Bool textclient_putc(text_data *d, int c)
{
    DWORD cb;
    fprintf(stderr, "%s: textclient: putc '%c'\n", progname, (char) c);
    return d->pipeInput->WriteFile(&c, 1, &cb);
}
