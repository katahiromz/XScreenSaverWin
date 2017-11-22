#include "xlockmore.h"
#include "resource.h"

#ifdef _UNICODE
    #pragma comment(lib, "scrnsavw.lib")
#else
    #pragma comment(lib, "scrnsave.lib")
#endif

//////////////////////////////////////////////////////////////////////////////

SCREENSAVER ss;
Bool mono_p = False;
#ifdef NDEBUG
    Bool debug_p = False;
#else
    Bool debug_p = True;
#endif

static LPCSTR pszCompany = "Software\\Katayama Hirofumi MZ";

//////////////////////////////////////////////////////////////////////////////

HBITMAP GetScreenShotBM(VOID)
{
    return ss.hbmScreenShot;
}

//////////////////////////////////////////////////////////////////////////////
// sz_trim

void sz_trim(char *psz)
{
    int len;
    char *pch = psz;

    while (*pch == ' ' || *pch == '\n')
        pch++;

    memmove(psz, pch, strlen(pch) + 1);

    len = strlen(psz);
    if (len == 0)
        return;

    pch = psz + len;
    do
    {
        pch--;
    } while (psz <= pch && (*pch == ' ' || *pch == '\n'));

    pch[1] = '\0';
}

//////////////////////////////////////////////////////////////////////////////
// CenterDialog

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

//////////////////////////////////////////////////////////////////////////////
// setting

VOID LoadSetting(ModeInfo *mi)
{
    HKEY hCompanyKey, hSoftwareKey;
    LONG result;
    INT i;
    CHAR szValue[256];
    DWORD cb;

    ss.screen_mode = SMODE_WHOLE_VIRTUAL;

    result = RegOpenKeyExA(HKEY_CURRENT_USER, pszCompany, 0,
        KEY_READ, &hCompanyKey);
    if (result != ERROR_SUCCESS)
        return;

    result = RegOpenKeyExA(hCompanyKey, progname, 0,
        KEY_READ, &hSoftwareKey);
    if (result == ERROR_SUCCESS)
    {
        // load screen_mode
        cb = 256 * sizeof(CHAR);
        result = RegQueryValueExA(hSoftwareKey,
            "screen_mode", NULL, NULL, szValue, &cb);
        if (result == ERROR_SUCCESS)
            ss.screen_mode = strtol(szValue, NULL, 10);
        // load count
        cb = 256 * sizeof(CHAR);
        result = RegQueryValueExA(hSoftwareKey,
            "count", NULL, NULL, szValue, &cb);
        if (result == ERROR_SUCCESS)
            mi->count = strtol(szValue, NULL, 10);
        // load cycles
        cb = 256 * sizeof(CHAR);
        result = RegQueryValueExA(hSoftwareKey,
            "cycles", NULL, NULL, szValue, &cb);
        if (result == ERROR_SUCCESS)
            mi->cycles = strtol(szValue, NULL, 10);
        // load size
        cb = 256 * sizeof(CHAR);
        result = RegQueryValueExA(hSoftwareKey,
            "size", NULL, NULL, szValue, &cb);
        if (result == ERROR_SUCCESS)
            mi->size = strtol(szValue, NULL, 10);
        // load args
        for (i = 0; i < hack_argcount; i++)
        {
            cb = 256 * sizeof(CHAR);
            result = RegQueryValueExA(hSoftwareKey,
                hack_arginfo[i].name, NULL, NULL, szValue, &cb);
            if (result == ERROR_SUCCESS)
            {
                switch (hack_arginfo[i].type)
                {
                case t_Bool:
                    if (lstrcmpiA(szValue, "True") == 0 || lstrcmpiA(szValue, "1") == 0)
                        *(Bool *)hack_arginfo[i].data = True;
                    if (lstrcmpiA(szValue, "False") == 0 || lstrcmpiA(szValue, "0") == 0)
                        *(Bool *)hack_arginfo[i].data = False;
                    break;

                case t_Int:
                    *(INT *)hack_arginfo[i].data = strtol(szValue, NULL, 10);
                    break;

                case t_Float:
                    *(float *)hack_arginfo[i].data = strtod(szValue, NULL);
                    break;

                case t_String:
                    *(char **)hack_arginfo[i].data = _strdup(szValue);
                    break;
                }
            }
        }
        RegCloseKey(hSoftwareKey);
    }
    RegCloseKey(hCompanyKey);
}

VOID GetSetting(HWND hwnd)
{
    int i;
    CHAR buf[256];
    for (i = 0; i < hack_argcount; i++)
    {
        GetDlgItemTextA(hwnd, IDC_ARGVAL00 + i, buf, 256);
        sz_trim(buf);
        switch (hack_arginfo[i].type)
        {
        case t_Bool:
            if (lstrcmpiA(buf, "True") == 0 || lstrcmpiA(buf, "1") == 0 )
                *(Bool *)hack_arginfo[i].data = True;
            else if (lstrcmpiA(buf, "False") == 0 || lstrcmpiA(buf, "0") == 0 )
                *(Bool *)hack_arginfo[i].data = False;
            break;

        case t_Int:
            *(INT *)hack_arginfo[i].data = strtol(buf, NULL, 10);
            break;

        case t_Float:
            *(float *)hack_arginfo[i].data = strtod(buf, NULL);
            break;

        case t_String:
            *(char **)hack_arginfo[i].data = _strdup(buf);
            break;
        }
    }
    if (hack_count_enabled)
    {
        GetDlgItemTextA(hwnd, IDC_COUNTVAL, buf, 256);
        sz_trim(buf);
        ss.modeinfo.count = strtol(buf, NULL, 10);
    }
    if (hack_cycles_enabled)
    {
        GetDlgItemTextA(hwnd, IDC_CYCLESVAL, buf, 256);
        sz_trim(buf);
        ss.modeinfo.cycles = strtol(buf, NULL, 10);
    }
    if (hack_size_enabled)
    {
        GetDlgItemTextA(hwnd, IDC_SIZEVAL, buf, 256);
        sz_trim(buf);
        ss.modeinfo.size = strtol(buf, NULL, 10);
    }
    i = (INT)SendDlgItemMessageA(hwnd, cmb1, CB_GETCURSEL, 0, 0);
    ss.screen_mode = i;
}

VOID ResetSetting(HWND hwnd)
{
    HKEY hCompanyKey;
    LONG result;

    result = RegOpenKeyExA(HKEY_CURRENT_USER, pszCompany, 0,
        KEY_ALL_ACCESS, &hCompanyKey);
    if (result != ERROR_SUCCESS)
        return;

    RegDeleteKey(hCompanyKey, progname);
    RegCloseKey(hCompanyKey);
}

VOID SaveSetting(ModeInfo *mi)
{
    HKEY hCompanyKey, hSoftwareKey;
    LONG result;
    INT i, n;
    DWORD dwDisp, dwSize;
    float e;
    CHAR szValue[256];

    result = RegCreateKeyExA(HKEY_CURRENT_USER, pszCompany, 0,
        NULL, 0, KEY_ALL_ACCESS, NULL, &hCompanyKey, &dwDisp);
    if (result != ERROR_SUCCESS)
        return;

    result = RegCreateKeyExA(hCompanyKey, progname, 0, NULL, 0,
        KEY_ALL_ACCESS, NULL, &hSoftwareKey, &dwDisp);
    if (result == ERROR_SUCCESS)
    {
        for (i = 0; i < hack_argcount; i++)
        {
            switch (hack_arginfo[i].type)
            {
            case t_Bool:
                if (*(Bool *)hack_arginfo[i].data)
                    strcpy(szValue, "True");
                else
                    strcpy(szValue, "False");
                break;

            case t_Int:
                n = *(INT *)hack_arginfo[i].data;
                sprintf(szValue, "%d", n);
                break;

            case t_Float:
                e = *(float *)hack_arginfo[i].data;
                sprintf(szValue, "%g", e);
                break;

            case t_String:
                strcpy(szValue, *(char **)hack_arginfo[i].data);
                break;
            }
            dwSize = (strlen(szValue) + 1) * sizeof(CHAR);
            RegSetValueExA(hSoftwareKey, hack_arginfo[i].name,
                0, REG_SZ, (LPBYTE)szValue, dwSize);
        }
        if (hack_count_enabled)
        {
            sprintf(szValue, "%d", ss.modeinfo.count);
            dwSize = (strlen(szValue) + 1) * sizeof(CHAR);
            RegSetValueExA(hSoftwareKey, "count",
                0, REG_SZ, (LPBYTE)szValue, dwSize);
        }
        if (hack_cycles_enabled)
        {
            sprintf(szValue, "%d", ss.modeinfo.cycles);
            dwSize = (strlen(szValue) + 1) * sizeof(CHAR);
            RegSetValueExA(hSoftwareKey, "cycles",
                0, REG_SZ, (LPBYTE)szValue, dwSize);
        }
        if (hack_size_enabled)
        {
            sprintf(szValue, "%d", ss.modeinfo.size);
            dwSize = (strlen(szValue) + 1) * sizeof(CHAR);
            RegSetValueExA(hSoftwareKey, "size",
                0, REG_SZ, (LPBYTE)szValue, dwSize);
        }
        {
            sprintf(szValue, "%d", (INT)ss.screen_mode);
            dwSize = (strlen(szValue) + 1) * sizeof(CHAR);
            RegSetValueExA(hSoftwareKey, "screen_mode",
                0, REG_SZ, (LPBYTE)szValue, dwSize);
        }
        RegCloseKey(hSoftwareKey);
    }
    RegCloseKey(hCompanyKey);
}

//////////////////////////////////////////////////////////////////////////////
// GetScreenShotBitmap

HBITMAP GetScreenShotBitmap(VOID)
{
    HWND hwnd;
    HDC hdc, hdcMem;
    HBITMAP hbm;
    HGDIOBJ hbmOld;
    BITMAPINFO bi;
    INT x, y, cx, cy;
    LPVOID pvBits;

    if (ss.screen_mode != SMODE_WHOLE_VIRTUAL)
    {
        HMONITOR hMonitor;
        MONITORINFO mi;

        hMonitor = MonitorFromWindow(NULL, MONITOR_DEFAULTTOPRIMARY);
        mi.cbSize = sizeof(mi);
        GetMonitorInfo(hMonitor, &mi);
        x = mi.rcMonitor.left;
        y = mi.rcMonitor.top;
        cx = mi.rcMonitor.right - x;
        cy = mi.rcMonitor.bottom - y;
    }
    else
    {
        x = GetSystemMetrics(SM_XVIRTUALSCREEN);
        y = GetSystemMetrics(SM_YVIRTUALSCREEN);
        cx = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        cy = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    }

    hwnd = GetDesktopWindow();
    hdc = GetWindowDC(hwnd);
    hdcMem = CreateCompatibleDC(hdc);

    ZeroMemory(&bi, sizeof(bi));
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = cx;
    bi.bmiHeader.biHeight = cy;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    hbm = CreateDIBSection(hdcMem, &bi, DIB_RGB_COLORS, &pvBits, NULL, 0);
    if (hbm != NULL)
    {
        hbmOld = SelectObject(hdcMem, hbm);
        BitBlt(hdcMem, 0, 0, cx, cy, hdc, x, y, SRCCOPY);
        SelectObject(hdcMem, hbmOld);
        GdiFlush();
    }

    DeleteDC(hdcMem);
    ReleaseDC(hwnd, hdc);
    return hbm;
}

//////////////////////////////////////////////////////////////////////////////
// SaveBitmapToFile

typedef struct tagBITMAPINFOEX
{
    BITMAPINFOHEADER bmiHeader;
    RGBQUAD          bmiColors[256];
} BITMAPINFOEX, *LPBITMAPINFOEX;

BOOL SaveBitmapToFile(LPCTSTR pszFileName, HBITMAP hbm)
{
    BOOL f;
    BITMAPFILEHEADER bf;
    BITMAPINFOEX bi;
    BITMAPINFOHEADER *pbmih;
    DWORD cb;
    DWORD cColors, cbColors;
    HDC hDC;
    HANDLE hFile;
    LPVOID pBits;
    BITMAP bm;
    DWORD dwError = 0;

    if (!GetObject(hbm, sizeof(BITMAP), &bm))
        return FALSE;

    pbmih = &bi.bmiHeader;
    ZeroMemory(pbmih, sizeof(BITMAPINFOHEADER));
    pbmih->biSize             = sizeof(BITMAPINFOHEADER);
    pbmih->biWidth            = bm.bmWidth;
    pbmih->biHeight           = bm.bmHeight;
    pbmih->biPlanes           = 1;
    pbmih->biBitCount         = bm.bmBitsPixel;
    pbmih->biCompression      = BI_RGB;
    pbmih->biSizeImage        = bm.bmWidthBytes * bm.bmHeight;

    if (bm.bmBitsPixel < 16)
        cColors = 1 << bm.bmBitsPixel;
    else
        cColors = 0;
    cbColors = cColors * sizeof(RGBQUAD);

    bf.bfType = 0x4d42;
    bf.bfReserved1 = 0;
    bf.bfReserved2 = 0;
    cb = sizeof(BITMAPFILEHEADER) + pbmih->biSize + cbColors;
    bf.bfOffBits = cb;
    bf.bfSize = cb + pbmih->biSizeImage;

    pBits = HeapAlloc(GetProcessHeap(), 0, pbmih->biSizeImage);
    if (pBits == NULL)
        return FALSE;

    f = FALSE;
    hDC = GetDC(NULL);
    if (hDC != NULL)
    {
        if (GetDIBits(hDC, hbm, 0, bm.bmHeight, pBits, (BITMAPINFO*)&bi,
            DIB_RGB_COLORS))
        {
            hFile = CreateFile(pszFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL,
                               CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL |
                               FILE_FLAG_WRITE_THROUGH, NULL);
            if (hFile != INVALID_HANDLE_VALUE)
            {
                f = WriteFile(hFile, &bf, sizeof(BITMAPFILEHEADER), &cb, NULL) &&
                    WriteFile(hFile, &bi, sizeof(BITMAPINFOHEADER), &cb, NULL) &&
                    WriteFile(hFile, bi.bmiColors, cbColors, &cb, NULL) &&
                    WriteFile(hFile, pBits, pbmih->biSizeImage, &cb, NULL);
                if (!f)
                    dwError = GetLastError();
                CloseHandle(hFile);

                if (!f)
                    DeleteFile(pszFileName);
            }
            else
                dwError = GetLastError();
        }
        else
            dwError = GetLastError();
        ReleaseDC(NULL, hDC);
    }
    else
        dwError = GetLastError();

    HeapFree(GetProcessHeap(), 0, pBits);
    SetLastError(dwError);
    return f;
}

//////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK
PrimaryWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        SetTimer(hWnd, 999, hack_delay / 1000, NULL);
        break;

    case WM_DESTROY:
        KillTimer(hWnd, 999);
        break;

    case WM_TIMER:
        hack_draw(&ss.modeinfo);
        break;

    case WM_ERASEBKGND:
        break;

    case WM_SYSCOMMAND:
    case WM_SETCURSOR:
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_MOUSEMOVE:
    case WM_NCACTIVATE:
    case WM_ACTIVATEAPP:
    case WM_ACTIVATE:
        return DefScreenSaverProc(ss.hwnd, uMsg, wParam, lParam);

    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}

BOOL CreatePrimaryWindow(INT x, INT y, INT cx, INT cy)
{
    WNDCLASS wc;

    ss.hwndPrimary = NULL;

    ZeroMemory(&wc, sizeof(wc));
    wc.lpfnWndProc = PrimaryWindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
    wc.lpszClassName = TEXT("XScreenSaverWin Primary Window");
    if (RegisterClass(&wc))
    {
        ss.hwndPrimary = CreateWindowEx(
            WS_EX_NOACTIVATE | WS_EX_TRANSPARENT,
            wc.lpszClassName, NULL, WS_POPUP, x, y, cx, cy,
            ss.hwnd, NULL, GetModuleHandle(NULL), NULL);
        if (ss.hwndPrimary != NULL)
        {
            ShowWindow(ss.hwndPrimary, SW_SHOWNOACTIVATE);
            UpdateWindow(ss.hwndPrimary);
            return TRUE;
        }
    }
    return FALSE;
}

LRESULT CALLBACK
SecondaryWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    RECT rc;
    HDC hdc;

    switch(uMsg)
    {
    case WM_CREATE:
        // 300msec
        SetTimer(hWnd, 999, 300, NULL);
        break;

    case WM_DESTROY:
        KillTimer(hWnd, 999);
        break;

    case WM_TIMER:
        // copy from primary
        GetClientRect(hWnd, &rc);
        hdc = GetWindowDC(hWnd);
        SetStretchBltMode(hdc, COLORONCOLOR);
        StretchBlt(
            hdc, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
            ss.hdc, 0, 0, ss.width, ss.height, SRCCOPY);
        ReleaseDC(hWnd, hdc);
        break;

    case WM_ERASEBKGND:
        break;

    case WM_SYSCOMMAND:
    case WM_SETCURSOR:
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_MOUSEMOVE:
    case WM_NCACTIVATE:
    case WM_ACTIVATEAPP:
    case WM_ACTIVATE:
        return DefScreenSaverProc(ss.hwnd, uMsg, wParam, lParam);

    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}

static BOOL CALLBACK 
MonitorProc(HMONITOR hMonitor, HDC hdc, LPRECT prc, LPARAM lParam)
{
    MONITORINFO mi;
    mi.cbSize = sizeof(mi);
    GetMonitorInfo(hMonitor, &mi);

    ss.monitor_info[ss.cMonitors] = mi;
    ++(ss.cMonitors);

    return (ss.cMonitors < MAX_NUM_MONITORS);
}

BOOL CreateSecondaryWindows(VOID)
{
    WNDCLASS wc;

    ZeroMemory(ss.ahwndSecondary, sizeof(ss.ahwndSecondary));

    ZeroMemory(&wc, sizeof(wc));
    wc.lpfnWndProc = SecondaryWindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
    wc.lpszClassName = TEXT("XScreenSaverWin Secondary Window");
    if (RegisterClass(&wc))
    {
        INT x, y, cx, cy;
        INT i, count;

        // enumerate monitors
        ss.cMonitors = 0;
        EnumDisplayMonitors(NULL, NULL, MonitorProc, 0);
        count = ss.cMonitors;

        for (i = 0; i < count; ++i)
        {
            if (ss.monitor_info[i].dwFlags & MONITORINFOF_PRIMARY)
            {
                // skip primary
                continue;
            }

            // get extent
            x = ss.monitor_info[i].rcMonitor.left;
            y = ss.monitor_info[i].rcMonitor.top;
            cx = ss.monitor_info[i].rcMonitor.right - x;
            cy = ss.monitor_info[i].rcMonitor.bottom - y;

            // create secondary window
            ss.ahwndSecondary[i] = CreateWindowEx(
                WS_EX_NOACTIVATE | WS_EX_TRANSPARENT,
                wc.lpszClassName, NULL, WS_POPUP, x, y, cx, cy,
                ss.hwnd, NULL, GetModuleHandle(NULL), NULL);
            if (ss.ahwndSecondary[i] == NULL)
            {
                return FALSE;
            }
            ShowWindow(ss.ahwndSecondary[i], SW_SHOWNOACTIVATE);
            UpdateWindow(ss.ahwndSecondary[i]);
        }
        return TRUE;
    }
    return FALSE;
}

//////////////////////////////////////////////////////////////////////////////
// screen saver

HANDLE g_hMapping = NULL;

Bool set_saver_name(const char *name)
{
    LPVOID p = NULL;
    SECURITY_ATTRIBUTES sa;
    ZeroMemory(&sa, sizeof(sa));
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;

    g_hMapping = CreateFileMappingA(INVALID_HANDLE_VALUE, &sa, PAGE_READWRITE,
        0, 256, "Katayama Hirofumi MZ XScreenSaverWin");
    if (g_hMapping)
    {
        p = MapViewOfFile(g_hMapping, FILE_MAP_WRITE, 0, 0, 256);
        if (p)
        {
            lstrcpyA((char *)p, progname);
            UnmapViewOfFile(p);
        }
    }
    return p != NULL;
}

BOOL ss_init(HWND hwnd)
{
    RECT rc;

    set_saver_name(progname);

    ss.hwnd = hwnd;

    LoadSetting(&ss.modeinfo);

#undef ya_rand_init
    ya_rand_init(0);

    if (!fChildPreview)
    {
        int x = GetSystemMetrics(SM_XVIRTUALSCREEN);
        int y = GetSystemMetrics(SM_YVIRTUALSCREEN);
        int cx = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        int cy = GetSystemMetrics(SM_CYVIRTUALSCREEN);
        MoveWindow(hwnd, x, y, cx, cy, TRUE);
    }

    ss.hdc = GetWindowDC(hwnd);
    if (ss.hdc == NULL)
    {
        assert(0);
        return FALSE;
    }

    // fill by black
    GetClientRect(hwnd, &rc);
    FillRect(ss.hdc, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));

    ss.cMonitors = GetSystemMetrics(SM_CMONITORS);
    if (!fChildPreview && (ss.cMonitors > 1) &&
        (ss.screen_mode != SMODE_WHOLE_VIRTUAL))
    {
        HMONITOR hMonitor;
        MONITORINFO mi;
        INT x, y, cx, cy;

        // get monitor extent
        hMonitor = MonitorFromWindow(NULL, MONITOR_DEFAULTTOPRIMARY);
        mi.cbSize = sizeof(mi);
        GetMonitorInfo(hMonitor, &mi);
        x = mi.rcMonitor.left;
        y = mi.rcMonitor.top;
        cx = mi.rcMonitor.right - mi.rcMonitor.left;
        cy = mi.rcMonitor.bottom - mi.rcMonitor.top;

        // set primary size
        ss.x = x;
        ss.y = y;
        ss.width = cx;
        ss.height = cy;

        if (!CreatePrimaryWindow(x, y, cx, cy) ||
            !CreateSecondaryWindows())
        {
            assert(0);
            return FALSE;
        }
        ReleaseDC(ss.hwnd, ss.hdc);
        ss.hdc = GetWindowDC(ss.hwndPrimary);
    }
    else
    {
        // single screen
        assert(rc.left == 0);
        assert(rc.top == 0);
        ss.x = 0;
        ss.y = 0;
        ss.width = rc.right - rc.left;
        ss.height = rc.bottom - rc.top;
        ss.hwndPrimary = NULL;
    }

    if (ss.width == 0 || ss.height == 0)
    {
        assert(0);
        return FALSE;
    }

    if (!InitPixelFormat(&ss))
    {
        assert(0);
        return FALSE;
    }

    ss.hbmScreenShot = GetScreenShotBitmap();
    if (ss.hbmScreenShot == NULL)
    {
        assert(0);
        return FALSE;
    }
    //SaveBitmapToFile("screenshot.bmp", ss.hbmScreenShot);

    MakeCurrent(&ss);

    ss.modeinfo.dpy = ss.hdc;
    ss.modeinfo.window = 0;
    ss.modeinfo.width = ss.width;
    ss.modeinfo.height = ss.height;
    ss.modeinfo.num_screen = 1;
    ss.modeinfo.screen_number = 0;
    ss.modeinfo.polygon_count = 0;
    ss.modeinfo.fps_p = False;
    ss.modeinfo.is_drawn = True;
    ss.modeinfo.count = hack_count;
    ss.modeinfo.cycles = hack_cycles;
    ss.modeinfo.size = hack_size;
    ss.modeinfo.xgwa.width = ss.width;
    ss.modeinfo.xgwa.height = ss.height;
    ss.modeinfo.xgwa.depth = 32;
    ss.modeinfo.xgwa.visual = NULL;
    ss.modeinfo.xgwa.colormap = 0;
    ss.modeinfo.xgwa.screen = 0;

    SaveSetting(&ss.modeinfo);

    hack_init(&ss.modeinfo);

    return TRUE;
}

BOOL WINAPI RegisterDialogClasses(HANDLE hInst)
{
    InitCommonControls();
    return TRUE;
}

VOID OnInitDialog(HWND hwnd)
{
    int i, n;
    double e;
    char buf[64], *p;
    HICON hIcon;

    // set big icon
    hIcon = LoadIconA(GetModuleHandleA(NULL), MAKEINTRESOURCEA(1));
    SendMessageA(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    // set small icon
    hIcon = (HICON)LoadImageA(GetModuleHandleA(NULL), MAKEINTRESOURCEA(1),
        IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
    SendMessageA(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    // set window title
    SetWindowText(hwnd, progname);

    for (i = 0; i < hack_argcount; i++)
    {
        SetDlgItemTextA(hwnd, IDC_ARGNAME00 + i, hack_arginfo[i].name);

        switch (hack_arginfo[i].type)
        {
        case t_Bool:
            if (*(Bool *)hack_arginfo[i].data)
                SetDlgItemTextA(hwnd, IDC_ARGVAL00 + i, "True");
            else
                SetDlgItemTextA(hwnd, IDC_ARGVAL00 + i, "False");
            break;

        case t_Int:
            n = *(INT *)hack_arginfo[i].data;
            SetDlgItemInt(hwnd, IDC_ARGVAL00 + i, n, TRUE);
            break;

        case t_Float:
            e = *(float *)hack_arginfo[i].data;
            sprintf(buf, "%g", e);
            SetDlgItemTextA(hwnd, IDC_ARGVAL00 + i, buf);
            break;

        case t_String:
            p = *(char **)hack_arginfo[i].data;
            SetDlgItemTextA(hwnd, IDC_ARGVAL00 + i, p);
            break;
        }
    }

    for (; i <= 23; i++)
    {
        ShowWindow(GetDlgItem(hwnd, IDC_ARGNAME00 + i), SW_HIDE);
        ShowWindow(GetDlgItem(hwnd, IDC_ARGVAL00 + i), SW_HIDE);
    }

    if (hack_count_enabled)
    {
        n = ss.modeinfo.count;
        SetDlgItemInt(hwnd, IDC_COUNTVAL, n, TRUE);
    }
    else
    {
        ShowWindow(GetDlgItem(hwnd, IDC_COUNTNAME), SW_HIDE);
        ShowWindow(GetDlgItem(hwnd, IDC_COUNTVAL), SW_HIDE);
    }

    if (hack_cycles_enabled)
    {
        n = ss.modeinfo.cycles;
        SetDlgItemInt(hwnd, IDC_CYCLESVAL, n, TRUE);
    }
    else
    {
        ShowWindow(GetDlgItem(hwnd, IDC_CYCLESNAME), SW_HIDE);
        ShowWindow(GetDlgItem(hwnd, IDC_CYCLESVAL), SW_HIDE);
    }

    if (hack_size_enabled)
    {
        n = ss.modeinfo.size;
        SetDlgItemInt(hwnd, IDC_SIZEVAL, n, TRUE);
    }
    else
    {
        ShowWindow(GetDlgItem(hwnd, IDC_SIZENAME), SW_HIDE);
        ShowWindow(GetDlgItem(hwnd, IDC_SIZEVAL), SW_HIDE);
    }

    {
        CHAR szText[128];
        INT i;
        for (i = 0; i < 3; ++i)
        {
            LoadStringA(GetModuleHandleA(NULL), 3 + i, szText, 128);
            SendDlgItemMessageA(hwnd, cmb1, CB_ADDSTRING, 0, (LPARAM)szText);
        }
        i = (INT)ss.screen_mode;
        SendDlgItemMessageA(hwnd, cmb1, CB_SETCURSEL, i, 0);
    }

    CenterDialog(hwnd);
}

BOOL WINAPI
ScreenSaverConfigureDialog(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
    case WM_INITDIALOG:
        ss.modeinfo.count = hack_count;
        ss.modeinfo.cycles = hack_cycles;
        ss.modeinfo.size = hack_size;
        LoadSetting(&ss.modeinfo);
        OnInitDialog(hWnd);
        return TRUE;

    case WM_CLOSE:
        EndDialog(hWnd, 0);
        break;

    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case IDOK:
            GetSetting(hWnd);
            SaveSetting(&ss.modeinfo);
            EndDialog(hWnd, 0);
            break;

        case IDC_RESET:
            ResetSetting(hWnd);
            EndDialog(hWnd, 0);
            break;

        case IDCANCEL:
            EndDialog(hWnd, 0);
            break;
        }
        break;
    }
    return FALSE;
}

LRESULT WINAPI
ScreenSaverProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        if (ss_init(hWnd) == 0)
            return -1;

        if (ss.hwndPrimary == NULL)
        {
            SetTimer(hWnd, 999, hack_delay / 1000, NULL);
        }
        break;

    case WM_DESTROY:
        KillTimer(hWnd, 999);
        ss_term();
        break;

    case WM_TIMER:
        hack_draw(&ss.modeinfo);
        break;

    default:
        return DefScreenSaverProc(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}

//////////////////////////////////////////////////////////////////////////////

Display *DisplayOfScreen(Screen *s)
{
    return ss.modeinfo.dpy;
}

int XClearWindow(Display *dpy, Window w)
{
    RECT rc;
    HDC hdc = (HDC)dpy;
    rc.left = rc.top = 0;
    rc.right = ss.width;
    rc.bottom = ss.height;
    return FillRect(hdc, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
}

//////////////////////////////////////////////////////////////////////////////
// trackball

trackball_state *gltrackball_init(void)
{
    return NULL;
}

void gltrackball_rotate(trackball_state *trackball)
{
}

void gltrackball_get_quaternion(char **ppch, float q[4])
{
    q[0] = q[1] = q[2] = q[3] = 0.0;
}

void gltrackball_start(trackball_state* trackball, int n1, int n2, int n3, int n4)
{
}

void gltrackball_track(trackball_state* trackball, int n1, int n2, int n3, int n4)
{
}

//////////////////////////////////////////////////////////////////////////////
// misc

void do_fps(ModeInfo *mi)
{
}

float current_device_rotation(void)
{
    return 0.0;
}

Bool has_writable_cells(Screen *s, Visual *v)
{
    return False;
}

int ffs(int i)
{
    int j;

    if (i == 0)
        return 0;
    for (j = 1; (i & 1) == 0; j++)
        i >>= 1;
    return j;
}

int visual_depth(Screen *screen, Visual *visual)
{
    return 32;
}

void gettimeofday(timeval *t, timezone *tz)
{
    DWORD dwTick = GetTickCount();
    t->tv_sec = dwTick / 1000;
    t->tv_usec = (dwTick % 1000) * 1000;
}

//////////////////////////////////////////////////////////////////////////////

Status XGetGeometry(
    Display *dpy, Drawable d, Window *root,
    int *x, int *y, unsigned int *width, unsigned int *height,
    unsigned int *borderWidth, unsigned int *depth)
{
    *root = 0;
    *x = *y = 0;
    if (d == 0)
    {
        *width = ss.width;
        *height = ss.height;
    }
    else
    {
        BITMAP bm;
        GetObject(d->hbm, sizeof(BITMAP), &bm);
        *width = bm.bmWidth;
        *height = bm.bmHeight;
    }
    *borderWidth = 0;
    *depth = 32;
    return 0;
}

Status XGetWindowAttributes(Display *dpy, Window w, XWindowAttributes *attr)
{
    *attr = ss.modeinfo.xgwa;
    return 0;
}

//////////////////////////////////////////////////////////////////////////////

#ifndef NDEBUG
    #undef printf
    #undef fprintf

    int __cdecl xlockmore_printf(const char *fmt, ...)
    {
        CHAR sz[512];
        va_list va;
        int n;
        va_start(va, fmt);
        n = vsprintf(sz, fmt, va);
        va_end(va);
        OutputDebugStringA(sz);
        return n;
    }

    int __cdecl xlockmore_fprintf(FILE *fp, const char *fmt, ...)
    {
        CHAR sz[512];
        va_list va;
        int n;
        va_start(va, fmt);
        n = vsprintf(sz, fmt, va);
        va_end(va);
        if (fp == stdout || fp == stderr)
            OutputDebugStringA(sz);
        else
            fputs(sz, fp);
        return n;
    }
#endif  // ndef NDEBUG

//////////////////////////////////////////////////////////////////////////////
