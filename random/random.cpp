// random.cpp --- random screensaver
// Author: katahiromz
// License: GPL v3
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
    if (pch) *pch = TEXT('\0');
    lstrcpy(szDir, szPath);
    lstrcpy(pch, TEXT("\\*.scr"));

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

            savers.push_back(szPath2);
        } while (FindNextFile(hFind, &find));
        FindClose(hFind);
    }
    if (savers.size() > 0)
        return;

    // try #2
    lstrcpy(pch, TEXT("\\..\\*_scr"));
    pch = _tcsrchr(szDir, TEXT('\\'));
    if (pch) *pch = TEXT('\0');

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

struct ENUM_PARAM
{
    DWORD dwProcessId;
};

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    ENUM_PARAM* pParam = (ENUM_PARAM*)lParam;
    DWORD dwPID = 0;
    GetWindowThreadProcessId(hwnd, &dwPID);
    if (dwPID == pParam->dwProcessId && IsWindowVisible(hwnd))
    {
        PostMessage(hwnd, WM_CLOSE, 0, 0);
    }
    return TRUE;
}

BOOL Execute(const TCHAR *cmdline, DWORD timeout_ms, BOOL* terminated_by_timeout)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    if (terminated_by_timeout)
        *terminated_by_timeout = FALSE;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.wShowWindow = SW_SHOWDEFAULT;
    si.dwFlags = STARTF_USESHOWWINDOW;

    LPTSTR pszCmdLine = _tcsdup(cmdline);
    if (CreateProcess(NULL, pszCmdLine, NULL, NULL, FALSE, 0,
                      NULL, NULL, &si, &pi))
    {
        WaitForInputIdle(pi.hProcess, 5000);
        free(pszCmdLine);

        DWORD waitResult = WaitForSingleObject(pi.hProcess, timeout_ms);

        if (waitResult == WAIT_TIMEOUT)
        {
            if (terminated_by_timeout)
                *terminated_by_timeout = TRUE;

            ENUM_PARAM param = { pi.dwProcessId };
            EnumWindows(EnumWindowsProc, (LPARAM)&param);
            PostThreadMessage(pi.dwThreadId, WM_QUIT, 0, 0);

            if (WaitForSingleObject(pi.hProcess, 1000) == WAIT_TIMEOUT)
            {
                TerminateProcess(pi.hProcess, 0);
                WaitForSingleObject(pi.hProcess, 1000);
            }
        }

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return TRUE;
    }
    free(pszCmdLine);
    return FALSE;
}

BOOL do_it(tstring& saver, DWORD timeout_ms, BOOL* terminated_by_timeout)
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

    return Execute(cmdline, timeout_ms, terminated_by_timeout);
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    srand(GetTickCount());
    get_screensavers(g_screensavers);

    if (g_screensavers.empty())
        return 0;

    const DWORD TIMEOUT_2_MIN = 2 * 60 * 1000; // 180,000ms

    BOOL isScreenSaverMode = TRUE;
    if (__argc >= 2)
    {
        if (_tcsnicmp(__targv[1], TEXT("/p"), 2) == 0 || 
            _tcsnicmp(__targv[1], TEXT("-p"), 2) == 0 ||
            _tcsnicmp(__targv[1], TEXT("/c"), 2) == 0 ||
            _tcsnicmp(__targv[1], TEXT("-c"), 2) == 0)
        {
            isScreenSaverMode = FALSE;
        }
    }

    while (TRUE)
    {
        int i = std::rand() % (UINT)g_screensavers.size();
        BOOL terminated_by_timeout = FALSE;

        if (!do_it(g_screensavers[i], isScreenSaverMode ? TIMEOUT_2_MIN : INFINITE, &terminated_by_timeout))
        {
            break;
        }

        if (!terminated_by_timeout)
        {
            break;
        }
    }

    return 0;
}
