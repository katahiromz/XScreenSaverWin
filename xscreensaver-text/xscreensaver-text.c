#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

char *progname = "xscreensaver-text";
char *version = "0.00";
char *saver_name = "";

int verbose = 0;
char *text_mode = "date";
char *text_literal = "";
char *text_file = "";
char *text_program = "";
int wrap_columns = 72;
int load_p = 1;
int head = -1;

char *xsst_key = "Software\\Katayama Hirofumi MZ\\xscreensaver-text";
char *saver_key = "Software\\Katayama Hirofumi MZ\\%s";

void usage(void)
{
    fprintf(stderr, "usage: %s [ -- options ...]\n\n", progname);
    fprintf(stderr,
        "    Prints out some text for use by various screensavers,\n"
        "    according to the registry information of the keys:\n"
        "    HKEY_CURRENT_USER\\Software\\Katayama Hirofumi MZ\\xscreensaver-text\n"
        "    and HKEY_CURRENT_USER\\Software\\Katayama Hirofumi MZ\\(saver_name) .\n"
        "    This may dump the contents of a file or run a program.\n"
        "\n"
        "  Options\n"
        "      --date              Print the host name and current time.\n"
        "\n"
        "      --text STRING       Print out the given text. It may contains %\n"
        "                          escape sequences as per sterftime(2).\n"
        "\n"
        "      --file PATH         Print the contents of given file.\n"
        "                          If --cols is specified re-wrap the lines;\n"
        "                          otherwise, print them as-is.\n"
        "\n"
        "      --program CMD       Run the given program and print its output.\n"
        "                          If --cols is specified, re-wrap the output.\n"
        "\n"
        "      --cols N            Wrap lines at this column.  Default 72.\n"
        "\n"
        "      --head N            Print first N lines only.\n"
        "\n");
}

int parse_options(int argc, char **argv)
{
    int i;

    for (i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-' && argv[i][1] == 'v')
        {
            verbose += strlen(argv[i]) - 1;
        }
        else if (lstrcmpiA(argv[i], "--verbose") == 0)
        {
            verbose++;
        }
        else if (lstrcmpiA(argv[i], "--date") == 0)
        {
            text_mode = "date";
            load_p = 0;
        }
        else if (lstrcmpiA(argv[i], "--text") == 0)
        {
            text_mode = "literal";
            text_literal = argv[++i];
            load_p = 0;
        }
        else if (lstrcmpiA(argv[i], "--file") == 0)
        {
            text_mode = "file";
            text_file = argv[++i];
            load_p = 0;
        }
        else if (lstrcmpiA(argv[i], "--head") == 0 ||
                 lstrcmpiA(argv[i], "--heads") == 0)
        {
            head = atoi(argv[++i]);
        }
        else if (lstrcmpiA(argv[i], "--program") == 0)
        {
            text_mode = "program";
            text_program = argv[++i];
            load_p = 0;
        }
        else if (lstrcmpiA(argv[i], "--url") == 0)
        {
            fprintf(stderr, "ERROR: --url is not supported yet\n");
            return 1;
        }
        else if (lstrcmpiA(argv[i], "--col") == 0 ||
                 lstrcmpiA(argv[i], "--cols") == 0 ||
                 lstrcmpiA(argv[i], "--column") == 0 ||
                 lstrcmpiA(argv[i], "--columns") == 0)
        {
            wrap_columns = atoi(argv[++i]);
            if (verbose)
            {
                fprintf(stderr, "%s: cols == %d\n", wrap_columns);
            }
        }
        else if (lstrcmpiA(argv[i], "--cocoa") == 0)
        {
            fprintf(stderr, "ERROR: --cocoa is not supported yet\n");
            return 1;
        }
        else if (lstrcmpiA(argv[i], "--nyarlathotep") == 0)
        {
            fprintf(stderr, "ERROR: --nyarlathotep is not supported yet\n");
            return 1;
        }
        else
        {
            fprintf(stderr, "ERROR: invalid option detected '%s'\n", argv[i]);
            usage();
            return 1;
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

output(void)
{
    if (lstrcmpiA(text_mode, "literal") == 0 && strlen(trim(text_literal)) == 0 ||
        lstrcmpiA(text_mode, "file") == 0 && strlen(trim(text_file)) == 0 ||
        lstrcmpiA(text_mode, "program") == 0 && strlen(trim(text_program)) == 0)
    {
        if (verbose)
            fprintf(stderr, "%s: falling back to 'date'\n", progname);

        text_mode = "date";
    }

    if (lstrcmpiA(text_mode, "literal") == 0)
    {
        int c, cols, lines;
        char *p;
        size_t len;
        char buf[1024];
        time_t t = time(NULL);
        strftime(buf, 1024, text_literal, localtime(&t));
        text_literal = _strdup(buf);

        cols = lines = 0;
        p = text_literal;
        while ((c = *p++) != '\0')
        {
            if (*p == '\r')
                continue;

            if (c == '\n')
            {
                cols = 0;
                lines++;
                if (head >= 1 && lines >= head)
                {
                    fputc(c, stdout);
                    break;
                }
            }
            else
                cols++;

            fputc(c, stdout);
            if (wrap_columns && cols % wrap_columns == 0)
                fputc('\n', stdout);
        }

        puts(text_literal);
        len = lstrlenA(text_literal);
        if (text_literal[len - 1] != '\n')
            puts("");
    }
    else if (lstrcmpiA(text_mode, "file") == 0)
    {
        int c, cols, lines;
        FILE *fp = fopen(text_file, "r");
        if (fp)
        {
            if (verbose)
                fprintf(stderr, "%s: reading %s\n", progname, text_file);

            cols = lines = 0;
            while ((c = fgetc(fp)) != EOF)
            {
                if (c == '\r')
                    continue;

                if (c == '\n')
                {
                    cols = 0;
                    lines++;
                    if (head >= 1 && lines >= head)
                    {
                        fputc(c, stdout);
                        break;
                    }
                }
                else
                    cols++;

                fputc(c, stdout);
                if (wrap_columns && cols % wrap_columns == 0)
                    fputc('\n', stdout);
            }
            fclose(fp);
        }
        else
        {
            if (verbose)
                fprintf(stderr, "%s: cannot open '%s'\n", progname, text_file);
        }
    }
    else if (lstrcmpiA(text_mode, "program") == 0)
    {
        int c, cols, lines;
        FILE *pipe = _popen(text_program, "w");
        if (pipe)
        {
            if (verbose)
                fprintf(stderr, "%s: running %s\n", progname, text_program);

            cols = lines = 0;
            while ((c = fgetc(pipe)) != EOF)
            {
                if (c == '\r')
                    continue;

                if (c == '\n')
                {
                    cols = 0;
                    lines++;
                    if (head >= 1 && lines >= head)
                    {
                        fputc(c, stdout);
                        break;
                    }
                }
                else
                    cols++;

                fputc(c, stdout);
                if (wrap_columns && cols % wrap_columns == 0)
                    fputc('\n', stdout);
            }
            _pclose(pipe);
        }
        else
        {
            if (verbose)
                fprintf(stderr, "%s: cannot open '%s'\n", progname, text_program);
        }
    }
    else    // "date"
    {
        OSVERSIONINFOA verinfo;
        CHAR buf[64];
        time_t t = time(NULL);

        verinfo.dwOSVersionInfoSize = sizeof(verinfo);
        GetVersionExA(&verinfo);
        printf("Microsoft Windows [Version %u.%u.%u]\n",
            verinfo.dwMajorVersion, verinfo.dwMinorVersion, verinfo.dwBuildNumber);

        // Thu Nov 28 14:52:58     2013
        strftime(buf, 64, "%a %b %d %H:%M:%S     %Y", localtime(&t));
        printf("%s\n", buf);
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

    result = RegOpenKeyExA(HKEY_CURRENT_USER, xsst_key, 0, KEY_READ, &hKey);
    if (result == ERROR_SUCCESS)
    {
        got_any_p = load_values(hKey);
        RegCloseKey(hKey);
    }

    saver_name = get_saver_name();

    if (saver_name && *saver_name)
    {
        if (verbose)
            fprintf(stderr, "%s: saver:   %s\n", saver_name);

        sprintf(keypath, saver_key, saver_name);
        result = RegOpenKeyExA(HKEY_CURRENT_USER, keypath, 0, KEY_READ, &hKey);
        if (result == ERROR_SUCCESS)
        {
            got_any_p |= load_values(hKey);
            RegCloseKey(hKey);
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
        return 1;

    if (load_p)
        get_prefs();

    output();

    return 0;
}
