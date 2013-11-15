#include "stdafx.h"

#define MAX_LOADSTRING 100


HINSTANCE hInst;
TCHAR szTitle[MAX_LOADSTRING];
TCHAR szWindowClass[MAX_LOADSTRING];


LPTSTR GetParametersToDebug(HWND hWnd)
{
    static TCHAR sz[256];
    wsprintf(sz, TEXT("/p %d"), (UINT)(UINT_PTR)hWnd);
    return sz;
}

VOID OnCopy(HWND hWnd)
{
    LPTSTR psz = GetParametersToDebug(hWnd);
    DWORD cb = (lstrlen(psz) + 1) * sizeof(TCHAR);
    HGLOBAL hGlobal = GlobalAlloc(GHND | GMEM_DDESHARE, cb);
    LPTSTR pszGlobal = (LPTSTR)GlobalLock(hGlobal);
    if (pszGlobal != NULL)
    {
        lstrcpy(pszGlobal, psz);
        GlobalUnlock(hGlobal);
        if (OpenClipboard(hWnd))
        {
            EmptyClipboard();
#ifdef UNICODE
            SetClipboardData(CF_UNICODETEXT, hGlobal);
#else
            SetClipboardData(CF_TEXT, hGlobal);
#endif
            CloseClipboard();
        }
        else
            GlobalFree(hGlobal);
    }
    else
        GlobalFree(hGlobal);
}


VOID OnPaint(HWND hWnd, HDC hdc)
{
    RECT rc;
    HWND hChild = GetWindow(hWnd, GW_CHILD);
    if (hChild != NULL)
        return;

    LPTSTR psz = GetParametersToDebug(hWnd);
    GetClientRect(hWnd, &rc);
    DrawText(hdc, psz, lstrlen(psz), &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
    LPCTSTR pszMsg = TEXT("(Press Ctrl+C to copy)");
    DrawText(hdc, pszMsg, lstrlen(pszMsg), &rc, DT_SINGLELINE | DT_CENTER | DT_BOTTOM);
}


INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch (message)
    {
    case WM_INITDIALOG:
        return TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
        }
        break;
    }
    return FALSE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
    case WM_COMMAND:
        wmId    = LOWORD(wParam);
        wmEvent = HIWORD(wParam);

        switch (wmId)
        {
        case IDM_COPY:
            OnCopy(hWnd);
            break;

        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;

        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        }
        break;
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        OnPaint(hWnd, hdc);
        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DEBUGHELPER));
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCE(IDC_DEBUGHELPER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;

    hInst = hInstance;

    INT cx = 200, cy = 150;
    RECT rc = {0, 0, cx, cy};
    AdjustWindowRectEx(&rc, WS_OVERLAPPEDWINDOW, TRUE, 0);

    cx = rc.right - rc.left;
    cy = rc.bottom - rc.top;
    hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, cx, cy, NULL, NULL, hInstance, NULL);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

extern "C"
int APIENTRY _tWinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPTSTR    lpCmdLine,
    int       nCmdShow)
{
    MSG msg;
    HACCEL hAccelTable;

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_DEBUGHELPER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DEBUGHELPER));

    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}
