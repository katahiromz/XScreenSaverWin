// xscreensaver.cpp --- XScreenSaverWin screensaver front-end
// Author: katahiromz
// License: GPL v3
// Copyright (C) 2013-2026 Katayama Hirofumi MZ
#include "stdafx.h"

#define MAX_LOADSTRING 256
#define RESTART_TIMER_ID 999
#define COMPANY_KEY TEXT("Software\\Katayama Hirofumi MZ\\XScreenSaverWin")

static HINSTANCE g_hInst = NULL;
static WNDPROC g_fnOldPreviewWndProc = NULL;

static LPTSTR GetScreenSaverPath(HWND hwnd)
{
    HWND hCombo = GetDlgItem(hwnd, ID_COMBO);

    static TCHAR szPath[MAX_PATH * 2];
    GetModuleFileName(NULL, szPath, MAX_PATH);
    LPTSTR pch = _tcsrchr(szPath, _T('\\'));
    *pch = 0;

    TCHAR szName[MAX_PATH], szName2[MAX_PATH];
    INT nIndex = (INT)(INT_PTR)SendMessage(hCombo, CB_GETCURSEL, 0, 0);
    SendMessage(hCombo, CB_GETLBTEXT, nIndex, (LPARAM)szName);

    lstrcpy(pch, TEXT("\\"));
    lstrcat(pch, szName);
    if (GetFileAttributes(szPath) != 0xFFFFFFFF)
    {
        return szPath;
    }

    lstrcpy(szName2, szName);
    LPTSTR pchDot = _tcsrchr(szName2, _T('.'));
    if (pchDot != NULL)
        *pchDot = _T('_');

    lstrcpy(pch, TEXT("\\"));
    lstrcat(pch, szName2);
    lstrcat(szPath, TEXT("\\"));
    lstrcat(szPath, szName);

    if (GetFileAttributes(szPath) != 0xFFFFFFFF)
    {
        return szPath;
    }

    return NULL;
}

static VOID CenterDialog(HWND hwnd)
{
    POINT pt;
    HWND hwndOwner;
    RECT rc, rcOwner;
    BOOL bChild = !!(GetWindowLong(hwnd, GWL_STYLE) & WS_CHILD);

    if (bChild)
        hwndOwner = GetParent(hwnd);
    else
        hwndOwner = GetWindow(hwnd, GW_OWNER);

    if (hwndOwner != NULL)
        GetWindowRect(hwndOwner, &rcOwner);
    else
        SystemParametersInfo(SPI_GETWORKAREA, 0, &rcOwner, 0);

    GetWindowRect(hwnd, &rc);

    pt.x = rcOwner.left +
        ((rcOwner.right - rcOwner.left) - (rc.right - rc.left)) / 2;
    pt.y = rcOwner.top +
        ((rcOwner.bottom - rcOwner.top) - (rc.bottom - rc.top)) / 2;

    if (bChild && hwndOwner != NULL)
        ScreenToClient(hwndOwner, &pt);

    SetWindowPos(hwnd, NULL, pt.x, pt.y, 0, 0,
        SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

    SendMessage(hwnd, DM_REPOSITION, 0, 0);
}

static BOOL Execute(HWND hwnd, LPCTSTR program, LPCTSTR params)
{
    HWND hPreview = GetDlgItem(hwnd, ID_PREVIEW);
    HWND hChild = GetWindow(hPreview, GW_CHILD);
    if (hChild != NULL)
    {
        SendMessage(hChild, WM_DESTROY, 0, 0);
    }

    TCHAR szCmdLine[MAX_PATH];
    lstrcpy(szCmdLine, TEXT("\""));
    lstrcat(szCmdLine, program);
    lstrcat(szCmdLine, TEXT("\" "));
    lstrcat(szCmdLine, params);

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    BOOL ret = CreateProcess(NULL, szCmdLine, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
    if (ret)
    {
        WaitForSingleObject(pi.hProcess, 250);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    return ret;
}

static __inline VOID OnConfigure(HWND hwnd)
{
    Execute(hwnd, GetScreenSaverPath(hwnd), NULL);
}

static __inline VOID OnTest(HWND hwnd)
{
    Execute(hwnd, GetScreenSaverPath(hwnd), TEXT("/S"));
}

static VOID OnTestOnWindow(HWND hwnd)
{
    HWND hPreview = GetDlgItem(hwnd, ID_PREVIEW);
    TCHAR szParams[MAX_PATH];
    wsprintf(szParams, TEXT("/p %u"), (UINT)(UINT_PTR)hPreview);
    Execute(hwnd, GetScreenSaverPath(hwnd), szParams);
}

static VOID OnInstall(HWND hwnd)
{
    LPTSTR pszPath = GetScreenSaverPath(hwnd);

    TCHAR szParams[MAX_PATH];
    wsprintf(szParams, TEXT("desk.cpl,InstallScreenSaver %s"), pszPath);
    Execute(hwnd, TEXT("rundll32.exe"), szParams);
}

static LPTSTR get_registered_screen_saver(void)
{
    HKEY hKey;
    LONG result;
    DWORD dwSize;
    TCHAR szPath[MAX_PATH], szLongPath[MAX_PATH];
    static TCHAR s_buf[MAX_PATH];

    s_buf[0] = _T('\0');
    result = RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("Control Panel\\Desktop"), 0,
        KEY_READ, &hKey);
    if (result == ERROR_SUCCESS)
    {
        dwSize = sizeof(szPath);
        result = RegQueryValueEx(hKey, TEXT("SCRNSAVE.EXE"), NULL, NULL,
            (LPBYTE)szPath, &dwSize);
        if (result == ERROR_SUCCESS)
        {
            LPTSTR pch;
            GetLongPathName(szPath, szLongPath, MAX_PATH);
            pch = _tcsrchr(szLongPath, _T('\\'));
            if (pch)
                lstrcpy(s_buf, pch + 1);
            else
                lstrcpy(s_buf, szLongPath);
            OutputDebugString(s_buf);
            OutputDebugString(TEXT("\n"));
        }
        RegCloseKey(hKey);
    }
    return s_buf[0] ? s_buf : NULL;
}

static LRESULT CALLBACK
PreviewWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_ERASEBKGND)
    {
        RECT rc;
        GetClientRect(hwnd, &rc);
        HDC hDC = (HDC)wParam;
        FillRect(hDC, &rc, GetStockBrush(BLACK_BRUSH));
        return TRUE;
    }
    if (uMsg == WM_PAINT)
    {
        RECT rc;
        GetClientRect(hwnd, &rc);
        PAINTSTRUCT ps;
        HDC hDC = BeginPaint(hwnd, &ps);
        FillRect(hDC, &rc, GetStockBrush(BLACK_BRUSH));
        EndPaint(hwnd, &ps);
        return 0;
    }
    return CallWindowProc(g_fnOldPreviewWndProc, hwnd, uMsg, wParam, lParam);
}

static BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    HWND hwndPreview = GetDlgItem(hwnd, ID_PREVIEW);
    g_fnOldPreviewWndProc = SubclassWindow(hwndPreview, PreviewWndProc);

    HICON hIcon;
    hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(1));
    SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    hIcon = (HICON)LoadImage(g_hInst, MAKEINTRESOURCE(1), IMAGE_ICON,
        GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
    SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

    HWND hCombo = GetDlgItem(hwnd, ID_COMBO);

    TCHAR szPath[MAX_PATH], szPath2[MAX_PATH], szDir[MAX_PATH];
    GetModuleFileName(NULL, szPath, MAX_PATH);
    LPTSTR pch = _tcsrchr(szPath, _T('\\'));
    *pch = _T('\0');
    lstrcpy(szDir, szPath);
    *pch = _T('\\');
    lstrcpy(pch, TEXT("\\*_scr"));

    WIN32_FIND_DATA find;
    HANDLE hFind = FindFirstFile(szPath, &find);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            lstrcpy(szPath2, szDir);
            lstrcat(szPath2, TEXT("\\"));
            lstrcat(szPath2, find.cFileName);
            lstrcat(szPath2, TEXT("\\*.scr"));

            WIN32_FIND_DATA find2;
            HANDLE hFind2 = FindFirstFile(szPath2, &find2);
            if (hFind2 != INVALID_HANDLE_VALUE)
            {
                SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)find2.cFileName);
                FindClose(hFind2);
            }
        } while (FindNextFile(hFind, &find));
        FindClose(hFind);
    }

    INT nCount = (INT)SendMessage(hCombo, CB_GETCOUNT, 0, 0);
    if (nCount == 0)
    {
        lstrcpy(pch, TEXT("\\*.scr"));
        hFind = FindFirstFile(szPath, &find);
        if (hFind != INVALID_HANDLE_VALUE)
        {
            do
            {
                SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)find.cFileName);
            } while (FindNextFile(hFind, &find));
            FindClose(hFind);
        }
    }
    nCount = (INT)SendMessage(hCombo, CB_GETCOUNT, 0, 0);
    if (nCount == 0)
    {
        EnableWindow(hCombo, FALSE);
        EnableWindow(GetDlgItem(hwnd, ID_INSTALL), FALSE);
        EnableWindow(GetDlgItem(hwnd, ID_CONFIGURE), FALSE);
        EnableWindow(GetDlgItem(hwnd, ID_TEST), FALSE);
    }
    else
    {
        TCHAR szBuff[MAX_PATH] = TEXT("");
        HKEY hKey = NULL;
        RegOpenKeyEx(HKEY_CURRENT_USER, COMPANY_KEY, 0, KEY_READ, &hKey);
        if (hKey)
        {
            DWORD cbBuff = sizeof(szBuff);
            RegQueryValueEx(hKey, TEXT("CurSel"), NULL, NULL, (PBYTE)szBuff, &cbBuff);
            szBuff[sizeof(szBuff) / sizeof(WCHAR) - 1] = 0;
            RegCloseKey(hKey);
        }

        INT i = 0;
        LPTSTR name = get_registered_screen_saver();
        if (name)
        {
            TCHAR szName[MAX_PATH];
            for (i = 0; i < nCount; i++)
            {
                SendMessage(hCombo, CB_GETLBTEXT, i, (LPARAM)szName);
                if (lstrcmpi(szName, name) == 0)
                    break;
            }
            if (i == nCount)
                i = 0;
        }
        else if (szBuff[0])
        {
            TCHAR szName[MAX_PATH];
            for (i = 0; i < nCount; i++)
            {
                SendMessage(hCombo, CB_GETLBTEXT, i, (LPARAM)szName);
                if (lstrcmpi(szName, szBuff) == 0)
                    break;
            }
            if (i == nCount)
                i = 0;
        }
        SendMessage(hCombo, CB_SETCURSEL, i, 0);
        OnTestOnWindow(hwnd);
    }

    CenterDialog(hwnd);
	return TRUE;
}

static void OnDestroy(HWND hwnd)
{
    HWND hCombo = GetDlgItem(hwnd, ID_COMBO);

    TCHAR szName[MAX_PATH];
    INT nIndex = (INT)(INT_PTR)SendMessage(hCombo, CB_GETCURSEL, 0, 0);
    SendMessage(hCombo, CB_GETLBTEXT, nIndex, (LPARAM)szName);

    HKEY hKey = NULL;
    RegCreateKeyEx(HKEY_CURRENT_USER, COMPANY_KEY, 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL);
    if (hKey)
    {
        RegSetValueEx(hKey, TEXT("CurSel"), 0, REG_SZ, (PBYTE)szName, (lstrlen(szName) + 1) * sizeof(TCHAR));
        RegCloseKey(hKey);
    }
}

static void RestartIfNotRunning(HWND hwnd)
{
    HWND hPreview = GetDlgItem(hwnd, ID_PREVIEW);
    HWND hChild = GetWindow(hPreview, GW_CHILD);
    if (hChild == NULL)
    {
        KillTimer(hwnd, RESTART_TIMER_ID);
        OnTestOnWindow(hwnd);
    }
}

static void OnTimer(HWND hwnd, UINT id)
{
    if (id == RESTART_TIMER_ID)
    {
        RestartIfNotRunning(hwnd);
    }
}

static void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch (id)
    {
    case IDOK:
        EndDialog(hwnd, IDOK);
        break;

    case IDCANCEL:
        EndDialog(hwnd, IDCANCEL);
        break;

    case ID_INSTALL:
        OnInstall(hwnd);
        break;

    case ID_CONFIGURE:
        OnConfigure(hwnd);
        break;

    case ID_TEST:
        OnTest(hwnd);
        SetTimer(hwnd, RESTART_TIMER_ID, 500, NULL);
        break;

    case ID_COMBO:
        if (codeNotify == CBN_SELCHANGE)
            OnTestOnWindow(hwnd);
        break;
    }
}

static void OnActivate(HWND hwnd, UINT state, HWND hwndActDeact, BOOL fMinimized)
{
    if (state == WA_ACTIVE || state == WA_CLICKACTIVE)
    {
        RestartIfNotRunning(hwnd);
    }
}

static INT_PTR CALLBACK
DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        HANDLE_MSG(hwnd, WM_INITDIALOG, OnInitDialog);
        HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
        HANDLE_MSG(hwnd, WM_TIMER, OnTimer);
        HANDLE_MSG(hwnd, WM_ACTIVATE, OnActivate);
        HANDLE_MSG(hwnd, WM_DESTROY, OnDestroy);
    }
    return 0;
}

static BOOL InitInstance(HINSTANCE hInstance, INT nCmdShow)
{
    UNREFERENCED_PARAMETER(nCmdShow);
    InitCommonControls();
    g_hInst = hInstance;
    return TRUE;
}

INT WINAPI
WinMain(HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR     lpCmdLine,
        INT       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    if (!InitInstance(hInstance, nCmdShow))
        return FALSE;

    return DialogBox(hInstance, MAKEINTRESOURCE(1), NULL, DialogProc);
}
