// Copyright (C) 2013 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>
//
// Permission to use, copy, modify, distribute, and sell this software and its
// documentation for any purpose is hereby granted without fee, provided that
// the above copyright notice appear in all copies and that both that
// copyright notice and this permission notice appear in supporting
// documentation.  No representations are made about the suitability of this
// software for any purpose.  It is provided "as is" without express or 
// implied warranty.

#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <tchar.h>

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

char *progname = "xscreensaver-text";
char *version = "0.00";
char *saver_name = "";

int verbose = 0;
char *text_mode = "";
char *text_literal = "";
char *text_file = "";
char *text_program = "";
int wrap_columns = 72;
int load_p = 1;
int head = -1;

int nyarlathotep_p = 0;

const char *saver_key = "Software\\Katayama Hirofumi MZ\\%s";

void show_version(void)
{
    fprintf(stderr, "XScreenSaver for Windows v0.79 text client utility\n");
    fprintf(stderr, "Written by Katayama Hirofumi MZ\n");
}

void usage(void)
{
    fprintf(stderr, "usage: %s [ --options ...]\n\n", progname);
    fprintf(stderr,
        "    Prints out some text for use by various screensavers,\n"
        "    according to the registry information of the key:\n"
        "    HKEY_CURRENT_USER\\Software\\Katayama Hirofumi MZ\\(saver_name).\n"
        "    This may dump the contents of a file or run a program.\n"
        "\n"
        "  Options\n"
        "      --date              Print the host info and current time.\n"
        "\n"
        "      --text STRING       Print out the given text. It may contains %\n"
        "                          escape sequences as per sterftime(2).\n"
        "                          If --cols is specified re-wrap the lines;\n"
        "                          otherwise, print them as-is.\n"
        "\n"
        "      --file \"PATH\"       Print the contents of given file.\n"
        "                          If --cols is specified re-wrap the lines;\n"
        "                          otherwise, print them as-is.\n"
        "\n"
        "      --program \"CMD\"     Run the given program and print its output.\n"
        "                          If --cols is specified, re-wrap the output.\n"
        "\n"
        "      --cols N            Wrap N lines at this column.  Default 72.\n"
        "\n"
        "      --head N            Print first N lines only.\n"
        "\n");
}

char do_nyarlathotep_char(char c)
{
	// y/A-Za-z/N-ZA-Mn-za-m/
    if ('A' <= c && c <= 'M')
    {
        c = 'N' + c - 'A';
    }
    else if ('N' <= c && c <= 'Z')
    {
        c = 'A' + c - 'N';
    }
    else if ('a' <= c && c <= 'm')
    {
        c = 'n' + c - 'a';
    }
    else if ('n' <= c && c <= 'z')
    {
        c = 'a' + c - 'n';
    }
    return c;
}

char *do_nyarlathotep_buffer(char *s)
{
    char *p = s;
    while (*p)
    {
        *p = do_nyarlathotep_char(*p);
        p++;
    }
    return s;
}

int parse_options(int argc, char **argv)
{
    int i, j, n;
    char *p, *q;

    for (i = 1; i < argc; i++)
    {
        if (lstrcmpiA(argv[i], "--version") == 0 || lstrcmpA(argv[i], "-V") == 0)
        {
            show_version();
            return 1;
        }
        else if (argv[i][0] == '-' && argv[i][1] == 'v')
        {
            for (j = 1; argv[i][j] == 'v'; j++)
                verbose++;
        }
        else if (lstrcmpiA(argv[i], "--verbose") == 0)
        {
            verbose++;
        }
        else if (lstrcmpiA(argv[i], "--date") == 0 || lstrcmpiA(argv[i], "-d") == 0)
        {
            text_mode = "date";
            load_p = 0;
        }
        else if (lstrcmpiA(argv[i], "--text") == 0 || lstrcmpiA(argv[i], "-t") == 0)
        {
            text_mode = "literal";
            text_literal = argv[++i];
            load_p = 0;
        }
        else if (lstrcmpiA(argv[i], "--file") == 0 || lstrcmpiA(argv[i], "-f") == 0)
        {
            text_mode = "file";
            text_file = argv[++i];
            load_p = 0;
        }
        else if (lstrcmpiA(argv[i], "--head") == 0 || lstrcmpiA(argv[i], "--heads") == 0 ||
                 lstrcmpiA(argv[i], "-h") == 0)
        {
            p = argv[++i];
            n = strtol(p, &q, 10);
            if (*q != '\0')
            {
                fprintf(stderr, "%s: ERROR: '%s' is not integer\n", progname, p);
                exit(EXIT_FAILURE);
            }
            head = n;
            if (verbose)
            {
                fprintf(stderr, "%s: head == %d\n", progname, head);
            }
        }
        else if (lstrcmpiA(argv[i], "--program") == 0 || lstrcmpiA(argv[i], "-p") == 0)
        {
            text_mode = "program";
            text_program = argv[++i];
            load_p = 0;
        }
        else if (lstrcmpiA(argv[i], "--url") == 0 || lstrcmpiA(argv[i], "-u") == 0)
        {
            fprintf(stderr, "%s: ERROR: --url is not supported yet\n", progname);
            return 1;
        }
        else if (lstrcmpiA(argv[i], "-c") == 0 ||
                 lstrcmpiA(argv[i], "--col") == 0 ||
                 lstrcmpiA(argv[i], "--cols") == 0 ||
                 lstrcmpiA(argv[i], "--column") == 0 ||
                 lstrcmpiA(argv[i], "--columns") == 0)
        {
            p = argv[++i];
            n = strtol(p, &q, 10);
            if (*q != '\0')
            {
                fprintf(stderr, "%s: ERROR: '%s' is not integer\n", progname, p);
                exit(EXIT_FAILURE);
            }
            wrap_columns = n;
            if (verbose)
            {
                fprintf(stderr, "%s: cols == %d\n", progname, wrap_columns);
            }
        }
        else if (lstrcmpiA(argv[i], "--cocoa") == 0)
        {
            fprintf(stderr, "%s: ERROR: --cocoa is not supported yet\n", progname);
            return 1;
        }
        else if (lstrcmpiA(argv[i], "--nyarlathotep") == 0 || lstrcmpiA(argv[i], "-n") == 0)
        {
            nyarlathotep_p = 1;
        }
        else if (lstrcmpiA(argv[i], "--help") == 0 || lstrcmpiA(argv[i], "-?") == 0 ||
                 lstrcmpiA(argv[i], "/?") == 0)
        {
            usage();
            return 1;
        }
        else
        {
            fprintf(stderr, "%s: ERROR: invalid option detected '%s'\n", progname, argv[i]);
            usage();
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}

void escape_newlines(char *s)
{
    char *d = s;
    while (*s)
    {
        if (*s == '\\' && s[1] == 'n')
        {
            *d++ = '\n';
            s++;
        }
        else
        {
            *d++ = *s;
        }
        s++;
    }
    *d = '\0';
}

char *trim(char *s)
{
    char *p = s;
    char *d = s;

    while (*s == ' ' || *s == '\t' || *s == '\n' || *s == '\r' || *s == '\f' || *s == '\v')
    {
        s++;
    }

    while (*s)
    {
        *d++ = *s++;
    }

    s--;
    while (*s == ' ' || *s == '\t' || *s == '\n' || *s == '\r' || *s == '\f' || *s == '\v')
    {
        s--;
        d--;
    }

    *d = '\0';
    return p;
}

void do_text_literal(void)
{
    int c, d, cols, lines;
    char *p;
    char buf[1024];
    time_t t = time(NULL);

    strftime(buf, 1024, text_literal, localtime(&t));
    if (nyarlathotep_p)
        do_nyarlathotep_buffer(buf);

    cols = lines = 0;
    p = buf;
    d = EOF;
    while ((c = *p++) != '\0')
    {
        if (c == '\r')
            continue;

        if (c == '\n')
        {
            cols = 0;
            lines++;
            if (head >= 1 && lines >= head)
            {
                putchar(c);
                break;
            }
        }
        else
            cols++;

        putchar(c);
        if (wrap_columns && cols % wrap_columns == 0 && cols)
            putchar('\n');
        d = c;
    }
    if (d != '\n')
        putchar('\n');
}

void do_text_file(void)
{
    int c, d, cols, lines;
    FILE *fp = fopen(text_file, "r");
    if (fp)
    {
        if (verbose)
            fprintf(stderr, "%s: reading %s\n", progname, text_file);

        cols = lines = 0;
        d = EOF;
        while ((c = fgetc(fp)) != EOF)
        {
            if (c == '\r')
                continue;

            if (nyarlathotep_p)
                c = do_nyarlathotep_char(c);

            if (c == '\n')
            {
                cols = 0;
                lines++;
                if (head >= 1 && lines >= head)
                {
                    putchar(c);
                    break;
                }
            }
            else
                cols++;

            putchar(c);
            if (wrap_columns && cols % wrap_columns == 0 && cols)
                putchar('\n');

            d = c;
        }
        fclose(fp);
        if (d != '\n')
            putchar('\n');
    }
    else
    {
        if (verbose)
            fprintf(stderr, "%s: ERROR: cannot open '%s'\n", progname, text_file);
    }
}

void do_text_program(void)
{
    int c, d, cols, lines;
    FILE *pipe = _popen(text_program, "w");
    if (pipe)
    {
        if (verbose)
            fprintf(stderr, "%s: running %s\n", progname, text_program);

        cols = lines = 0;
        d = EOF;
        while ((c = fgetc(pipe)) != EOF)
        {
            if (c == '\r')
                continue;

            if (nyarlathotep_p)
                c = do_nyarlathotep_char(c);

            if (c == '\n')
            {
                cols = 0;
                lines++;
                if (head >= 1 && lines >= head)
                {
                    putchar(c);
                    break;
                }
            }
            else
                cols++;

            putchar(c);
            if (wrap_columns && cols % wrap_columns == 0 && cols)
                putchar('\n');

            d = c;
        }
        _pclose(pipe);
        if (d != '\n')
            putchar('\n');
    }
    else
    {
        if (verbose)
            fprintf(stderr, "%s: ERROR: cannot open '%s'\n", progname, text_program);
    }
}

void do_date(void)
{
    OSVERSIONINFOA verinfo;
    CHAR buf[128];
    time_t t = time(NULL);
    DWORD dwSize;
    CHAR szComp[MAX_PATH], szUser[MAX_PATH];

    // OS info
    verinfo.dwOSVersionInfoSize = sizeof(verinfo);
    GetVersionExA(&verinfo);
    sprintf(buf, "Microsoft Windows [Version %u.%u.%u]",
        verinfo.dwMajorVersion, verinfo.dwMinorVersion, verinfo.dwBuildNumber);
    if (nyarlathotep_p)
        do_nyarlathotep_buffer(buf);
    puts(buf);

    // computer_name - user_name
    dwSize = MAX_PATH;
    GetComputerNameA(szComp, &dwSize);
    dwSize = MAX_PATH;
    GetUserNameA(szUser, &dwSize);
    sprintf(buf, "%s - %s", szComp, szUser);
    if (nyarlathotep_p)
        do_nyarlathotep_buffer(buf);
    puts(buf);

    // Thu Nov 28 14:52:58     2013
    strftime(buf, 64, "%a %b %d %H:%M:%S     %Y", localtime(&t));
    if (nyarlathotep_p)
        do_nyarlathotep_buffer(buf);
    puts(buf);
}

void output(void)
{
    if (lstrcmpiA(text_mode, "literal") == 0 && strlen(trim(text_literal)) == 0 ||
        lstrcmpiA(text_mode, "file") == 0 && strlen(trim(text_file)) == 0 ||
        lstrcmpiA(text_mode, "program") == 0 && strlen(trim(text_program)) == 0)
    {
        if (verbose)
            fprintf(stderr, "%s: WARNING: falling back to 'date'\n", progname);

        text_mode = "date";
    }

    if (lstrcmpiA(text_mode, "literal") == 0)
    {
        do_text_literal();
    }
    else if (lstrcmpiA(text_mode, "file") == 0)
    {
        do_text_file();
    }
    else if (lstrcmpiA(text_mode, "program") == 0)
    {
        do_text_program();
    }
    else    // "date"
    {
        do_date();
    }
}

int load_values(HKEY hKey)
{
    LONG result;
    CHAR value[MAX_PATH * 2];
    DWORD cb;
    int got_any_p = 0;

    cb = sizeof(value);
    result = RegQueryValueExA(hKey, "textMode", NULL, NULL, (LPBYTE)value, &cb);
    if (result == ERROR_SUCCESS)
    {
        text_mode = _strdup(value);
        got_any_p = 1;
    }
    cb = sizeof(value);
    result = RegQueryValueExA(hKey, "textLiteral", NULL, NULL, (LPBYTE)value, &cb);
    if (result == ERROR_SUCCESS)
    {
        text_literal = _strdup(value);
    }
    cb = sizeof(value);
    result = RegQueryValueExA(hKey, "textFile", NULL, NULL, (LPBYTE)value, &cb);
    if (result == ERROR_SUCCESS)
    {
        text_file = _strdup(value);
    }
    cb = sizeof(value);
    result = RegQueryValueExA(hKey, "textProgram", NULL, NULL, (LPBYTE)value, &cb);
    if (result == ERROR_SUCCESS)
    {
        text_program = _strdup(value);
    }
    return got_any_p;
}

char *get_saver_name(void)
{
    static char s_saver[MAX_PATH];
    HANDLE hMapping;
    LPVOID p = NULL;

    s_saver[0] = '\0';
    hMapping = OpenFileMappingA(FILE_MAP_READ, TRUE, "Katayama Hirofumi MZ XScreenSaverWin");
    if (hMapping != NULL)
    {
        p = MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 256);
        if (p)
        {
            lstrcpyA(s_saver, (char *)p);
            UnmapViewOfFile(p);
        }
        CloseHandle(hMapping);
    }

    return s_saver;
}

int get_prefs(void)
{
    HKEY hKey;
    LONG result;
    CHAR keypath[MAX_PATH];
    int got_any_p = 0;

    if (text_mode == NULL || *text_mode == '\0')
    {
        saver_name = get_saver_name();
        if (saver_name && *saver_name)
        {
            if (verbose)
                fprintf(stderr, "%s: saver:   %s\n", saver_name);

            sprintf(keypath, saver_key, saver_name);
            result = RegOpenKeyExA(HKEY_CURRENT_USER, keypath, 0, KEY_READ, &hKey);
            if (result == ERROR_SUCCESS)
            {
                got_any_p = load_values(hKey);
                RegCloseKey(hKey);
            }
        }
    }

    if (verbose > 1)
    {
        fprintf(stderr, "%s: mode:    %s\n", text_mode);
        fprintf(stderr, "%s: literal: %s\n", text_literal);
        fprintf(stderr, "%s: file:    %s\n", text_file);
        fprintf(stderr, "%s: program: %s\n", text_program);
    }

    _strlwr(text_mode);
    text_literal = _strdup(text_literal);
    escape_newlines(text_literal);

    return got_any_p;
}

int main(int argc, char **argv)
{
    if (parse_options(argc, argv))
        return EXIT_SUCCESS;

    if (verbose > 1)
    {
        fprintf(stderr, "head:         %d\n", head);
        fprintf(stderr, "wrap_columns: %d\n", wrap_columns);
        if (nyarlathotep_p)
            fprintf(stderr, "nyarlathotep is enabled\n");
    }

    if (load_p)
        get_prefs();

    output();

    return EXIT_SUCCESS;
}
