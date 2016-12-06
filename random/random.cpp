#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS
#include <windows.h>
#include <tchar.h>

#include <vector>
#include <string>
#include <cstdlib>
#include <process.h>

#ifdef UNICODE
    typedef std::wstring tstring;
#else
    typedef std::string tstring;
#endif

std::vector<tstring> g_screensavers;

void get_screensavers(std::vector<tstring>& savers)
{
    TCHAR szPath[MAX_PATH], szPath2[MAX_PATH], szDir[MAX_PATH];
    WIN32_FIND_DATA find;
    HANDLE hFind;

    savers.clear();
    
    GetModuleFileName(NULL, szPath, MAX_PATH);
    LPTSTR pch = _tcsrchr(szPath, TEXT('\\'));
    *pch = TEXT('\0');
    lstrcpy(szDir, szPath);
    lstrcpy(pch, TEXT("\\*.scr"));

    // Now, szDir is the directory that has random.scr.

    // try #1
    hFind = FindFirstFile(szPath, &find);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (lstrcmpi(find.cFileName, TEXT("random.scr")) == 0)
                continue;

            lstrcpy(szPath2, szDir);
            lstrcat(szPath2, TEXT("\\"));
            lstrcat(szPath2, find.cFileName);

            savers.push_back(szPath);
        } while (FindNextFile(hFind, &find));
        FindClose(hFind);
    }
    if (savers.size() > 0)
        return;

    // try #2
    lstrcpy(pch, TEXT("\\..\\*_scr"));
    pch = _tcsrchr(szDir, TEXT('\\'));
    *pch = TEXT('\0');

    hFind = FindFirstFile(szPath, &find);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (lstrcmpi(find.cFileName, TEXT("random_scr")) == 0)
                continue;

            lstrcpy(szPath2, szDir);
            lstrcat(szPath2, TEXT("\\"));
            lstrcat(szPath2, find.cFileName);
            lstrcat(szPath2, TEXT("\\*.scr"));

            WIN32_FIND_DATA find2;
            HANDLE hFind2 = FindFirstFile(szPath2, &find2);
            if (hFind2 != INVALID_HANDLE_VALUE)
            {
                lstrcpy(szPath, szDir);
                lstrcat(szPath, TEXT("\\"));
                lstrcat(szPath, find.cFileName);
                lstrcat(szPath, TEXT("\\"));
                lstrcat(szPath, find2.cFileName);
                savers.push_back(szPath);

                FindClose(hFind2);
            }
        } while (FindNextFile(hFind, &find));
        FindClose(hFind);
    }
}

BOOL Execute(const TCHAR *cmdline)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.wShowWindow = SW_SHOWDEFAULT;
    si.dwFlags = STARTF_USESHOWWINDOW;

    LPTSTR pszCmdLine = _tcsdup(cmdline);
    if (CreateProcess(NULL, pszCmdLine, NULL, NULL, FALSE, 0,
                      NULL, NULL, &si, &pi))
    {
        free(pszCmdLine);
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return TRUE;
    }
    free(pszCmdLine);
    return FALSE;
}

void do_it(tstring& saver)
{
    TCHAR cmdline[MAX_PATH * 3];
    if (__argc == 3)
    {
        _stprintf(cmdline, TEXT("\"%s\" %s %s"), saver.c_str(), __targv[1], __targv[2]);
    }
    else if (__argc == 2)
    {
        _stprintf(cmdline, TEXT("\"%s\" %s"), saver.c_str(), __targv[1]);
    }
    else
    {
        _stprintf(cmdline, TEXT("\"%s\""), saver.c_str());
    }

    Execute(cmdline);
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    srand(GetTickCount());
    get_screensavers(g_screensavers);

    int i = std::rand() % (UINT)g_screensavers.size();
    do_it(g_screensavers[i]);

    return 0;
}
