#include "stdafx.h"
#include "xscreensaver.h"

#define MAX_LOADSTRING 100

HINSTANCE hInst;
TCHAR szTitle[MAX_LOADSTRING];
TCHAR szWindowClass[MAX_LOADSTRING];

LPTSTR GetScreenSaverPath(HWND hwnd)
{
    HWND hCombo = GetDlgItem(hwnd, ID_COMBO);

    static TCHAR szPath[MAX_PATH * 2];
    GetModuleFileName(NULL, szPath, MAX_PATH);
    LPTSTR pch = _tcsrchr(szPath, _T('\\'));
    *pch = _T('\0');

    TCHAR szName1[MAX_PATH], szName2[MAX_PATH];
    INT nIndex = (INT)(INT_PTR)SendMessage(hCombo, CB_GETCURSEL, 0, 0);
    SendMessage(hCombo, CB_GETLBTEXT, nIndex, (LPARAM)szName1);
    lstrcpy(szName2, szName1);

    pch = _tcsrchr(szName2, _T('.'));
    if (pch != NULL)
        *pch = _T('_');

    lstrcat(szPath, TEXT("\\"));
    lstrcat(szPath, szName2);
    lstrcat(szPath, TEXT("\\"));
    lstrcat(szPath, szName1);

    if (GetFileAttributes(szPath) == 0xFFFFFFFF)
        return NULL;

    return szPath;
}

VOID CenterDialog(HWND hwnd)
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

BOOL Execute(HWND hwnd, LPCTSTR program, LPCTSTR params)
{
    TCHAR szCmdLine[MAX_PATH];
    lstrcpy(szCmdLine, TEXT("\""));
    lstrcat(szCmdLine, program);
    lstrcat(szCmdLine, TEXT("\" "));
    lstrcat(szCmdLine, params);

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    BOOL b = CreateProcess(NULL, szCmdLine, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
    if (b)
    {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return TRUE;
    }
    return FALSE;
}

VOID OnConfigure(HWND hDlg)
{
    Execute(hDlg, GetScreenSaverPath(hDlg), NULL);
}

VOID OnTest(HWND hDlg)
{
    Execute(hDlg, GetScreenSaverPath(hDlg), TEXT("/S"));
}

VOID OnTestOnWindow(HWND hDlg)
{
    HWND hPreview = GetDlgItem(hDlg, ID_PREVIEW);
    HWND hChild = GetWindow(hPreview, GW_CHILD);
    if (hChild != NULL)
        SendMessage(hChild, WM_DESTROY, 0, 0);

    TCHAR szParams[MAX_PATH];
    wsprintf(szParams, TEXT("/p %u"), (UINT)(UINT_PTR)hPreview);
    Execute(hDlg, GetScreenSaverPath(hDlg), szParams);
}

VOID OnInstall(HWND hDlg)
{
    LPTSTR pszPath = GetScreenSaverPath(hDlg);

    TCHAR szParams[MAX_PATH];
    wsprintf(szParams, TEXT("desk.cpl,InstallScreenSaver %s"), pszPath);
    Execute(hDlg, TEXT("rundll32.exe"), szParams);
}

#ifdef UNICODE
    __declspec(dllimport)
    DWORD WINAPI GetLongPathNameW(LPCTSTR, LPTSTR, DWORD);
    #define GetLongPathName GetLongPathNameW
#else
    __declspec(dllimport)
    DWORD WINAPI GetLongPathNameA(LPCTSTR, LPTSTR, DWORD);
    #define GetLongPathName GetLongPathNameA
#endif  // def UNICODE

LPTSTR get_registered_screen_saver(void)
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

VOID OnInitDialog(HWND hDlg)
{
    HICON hIcon;
    hIcon = LoadIcon(hInst, MAKEINTRESOURCE(1));
    SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    hIcon = (HICON)LoadImage(hInst, MAKEINTRESOURCE(1), IMAGE_ICON,
        GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
    SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

    HWND hCombo = GetDlgItem(hDlg, ID_COMBO);

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
                COMBOBOXEXITEM item;
                item.mask = CBEIF_TEXT;
                item.iItem = -1;
                item.pszText = find2.cFileName;
                SendMessage(hCombo, CBEM_INSERTITEM, 0, (LPARAM)&item);
                FindClose(hFind2);
            }
        } while (FindNextFile(hFind, &find));
        FindClose(hFind);
    }

    INT nCount = (INT)SendMessage(hCombo, CB_GETCOUNT, 0, 0);
    if (nCount == 0)
    {
        EnableWindow(hCombo, FALSE);
        EnableWindow(GetDlgItem(hDlg, ID_INSTALL), FALSE);
        EnableWindow(GetDlgItem(hDlg, ID_CONFIGURE), FALSE);
        EnableWindow(GetDlgItem(hDlg, ID_TEST), FALSE);
    }
    else
    {
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
        SendMessage(hCombo, CB_SETCURSEL, i, 0);
        OnTestOnWindow(hDlg);
    }

    CenterDialog(hDlg);
}

INT_PTR CALLBACK DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        OnInitDialog(hDlg);
        return TRUE;

        
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
            EndDialog(hDlg, IDOK);
            return TRUE;

        case IDCANCEL:
            EndDialog(hDlg, IDCANCEL);
            return TRUE;

        case ID_INSTALL:
            OnInstall(hDlg);
            break;

        case ID_CONFIGURE:
            OnConfigure(hDlg);
            break;

        case ID_TEST:
            OnTest(hDlg);
            break;

        case ID_COMBO:
            if (HIWORD(wParam) == CBN_SELCHANGE)
            {
                OnTestOnWindow(hDlg);
            }
            break;
        }
        break;
    }
    return FALSE;
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    return TRUE;
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_XSCREENSAVER, szWindowClass, MAX_LOADSTRING);

    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    return DialogBox(hInstance, MAKEINTRESOURCE(1), NULL, DialogProc);
}
