#include "win32.h"
#include <scrnsave.h>
#include "resource.h"

#ifdef _UNICODE
    #pragma comment(lib, "scrnsavw.lib")
#else
    #pragma comment(lib, "scrnsave.lib")
#endif

//////////////////////////////////////////////////////////////////////////////

SCREENSAVER ss;

static LPCSTR pszCompany = "Software\\Katayama Hirofumi MZ";

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

    result = RegOpenKeyExA(HKEY_CURRENT_USER, pszCompany, 0,
        KEY_READ, &hCompanyKey);
    if (result != ERROR_SUCCESS)
        return;

    result = RegOpenKeyExA(hCompanyKey, progname, 0,
        KEY_READ, &hSoftwareKey);
    if (result == ERROR_SUCCESS)
    {
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
    INT y, cx, cy, count;
    LPVOID pvBits;
    LPBYTE pb, pbBits;

    cx = GetSystemMetrics(SM_CXSCREEN);
    cy = GetSystemMetrics(SM_CYSCREEN);

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
        SetStretchBltMode(hdcMem, COLORONCOLOR);
        StretchBlt(hdcMem, 0, 0, cx, cy, hdc, 0, 0, cx, cy, SRCCOPY);
        SelectObject(hdcMem, hbmOld);
        GdiFlush();
        pbBits = (LPBYTE)pvBits;
        count = cx * cy;
        while (count--)
        {
            BYTE b = pbBits[0];
            pbBits[0] = pbBits[2];
            pbBits[2] = b;
            pbBits++;
            pbBits++;
            pbBits++;
            *pbBits++ = 0xFF;
        }
        pb = (LPBYTE)malloc(cx * 4);
        pbBits = (LPBYTE)pvBits;
        for (y = 0; y < cy / 2; y++)
        {
            memcpy(pb, &pbBits[y * cx * 4], cx * 4);
            memcpy(&pbBits[y * cx * 4], &pbBits[(cy - y - 1) * cx * 4], cx * 4);
            memcpy(&pbBits[(cy - y - 1) * cx * 4], pb, cx * 4);
        }
        free(pb);
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
} BITMAPINFOEX, FAR * LPBITMAPINFOEX;

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
// screen saver

BOOL ss_init(HWND hwnd)
{
    RECT rc;

    ss.hwnd = hwnd;

    GetClientRect(hwnd, &rc);
    ss.x0 = rc.left;
    ss.y0 = rc.top;
    ss.w  = rc.right  - ss.x0;
    ss.h  = rc.bottom - ss.y0;
    if (ss.w == 0 || ss.h == 0)
        return FALSE;

    ss.hdc = GetDC(hwnd);
    if (ss.hdc == NULL)
        return FALSE;

    if (!InitPixelFormat(&ss))
    {
        ReleaseDC(hwnd, ss.hdc);
        return FALSE;
    }

    ss.hbmScreenShot = GetScreenShotBitmap();
    if (ss.hbmScreenShot == NULL)
    {
        ReleaseDC(hwnd, ss.hdc);
        return FALSE;
    }
    //SaveBitmapToFile("screenshot.bmp", ss.hbmScreenShot);

    MakeCurrent(&ss);

    ss.modeinfo.dpy = ss.hdc;
    ss.modeinfo.window = 0;
    ss.modeinfo.gc = XCreateGC(ss.hdc, 0, 0, NULL);
    ss.modeinfo.num_screen = 1;
    ss.modeinfo.screen_number = 0;
    ss.modeinfo.width = ss.w;
    ss.modeinfo.height = ss.h;
    ss.modeinfo.polygon_count = 0;
    ss.modeinfo.fps_p = False;
    ss.modeinfo.is_drawn = True;
    ss.modeinfo.count = hack_count;
    ss.modeinfo.cycles = hack_cycles;
    ss.modeinfo.size = hack_size;
    ss.modeinfo.xgwa.width = ss.w;
    ss.modeinfo.xgwa.height = ss.h;
    ss.modeinfo.xgwa.depth = 32;
    ss.modeinfo.xgwa.visual = NULL;
    ss.modeinfo.xgwa.colormap = 0;
    ss.modeinfo.xgwa.screen = hwnd;

    LoadSetting(&ss.modeinfo);

#undef ya_rand_init
    ya_rand_init(0);
    hack_init(&ss.modeinfo);

    return TRUE;
}

BOOL WINAPI RegisterDialogClasses(HANDLE hInst)
{
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

    CenterDialog(hwnd);
}

BOOL WINAPI ScreenSaverConfigureDialog(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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

LRESULT WINAPI ScreenSaverProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
    case WM_CREATE:
        if (ss_init(hWnd) == 0)
            return -1;

        SetTimer(hWnd, 999, hack_delay / 1000, NULL);
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
// X Window-related

Display *DisplayOfScreen(Screen *s)
{
    return GetWindowDC(s);
}

int XClearWindow(Display *dpy, Window w)
{
    RECT rc;
    HDC hdc = (HDC)dpy;
    HWND hwnd = WindowFromDC(hdc);
    GetClientRect(hwnd, &rc);
    return FillRect(hdc, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
}

static int gc_count = 0;
static XGCValues *gc_buffer[MAX_GC_BUFFER] = {NULL};

XGCValues *XGetGCValues0(GC gc)
{
    assert(gc_buffer[gc] != NULL);
    return gc_buffer[gc];
}

Status XGetGCValues(
    Display *dpy,
    GC gc,
    unsigned long valuemask,
    XGCValues *values_return)
{
    XGCValues *values = XGetGCValues0(gc);
    CopyMemory(values_return, values, sizeof(XGCValues));
    return 1;
}

GC XCreateGC(
     Display *dpy, 
     Drawable d,
     unsigned long valuemask, 
     XGCValues *values)
{
    int i;

    if (gc_count < MAX_GC_BUFFER)
    {
        values = (XGCValues *)calloc(1, sizeof(XGCValues));
        if (values != NULL)
        {
            for (i = 0; i < MAX_GC_BUFFER; i++)
            {
                if (gc_buffer[i] == NULL)
                {
                    gc_buffer[i] = values;
                    gc_count++;
                    return i;
                }
            }
            assert(0);
        }
        assert(0);
    }
    assert(0);
    return 0;
}

int XFreeGC(Display *dpy, GC gc)
{
    assert(gc_buffer[gc] != NULL);
    free(gc_buffer[gc]);
    gc_buffer[gc] = NULL;
    gc_count--;
    assert(gc_count >= 0);
    return 0;
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

//////////////////////////////////////////////////////////////////////////////
// message box output

#ifdef MSGBOXOUTPUT
    #undef fprintf
    #undef abort
    #undef exit

    static CHAR s_szBuffer[2048] = "";

    int __cdecl win32_fprintf(FILE *fp, const char *fmt, ...)
    {
        CHAR sz[512];
        va_list va;
        int n;
        va_start(va, fmt);
        n = wvsprintf(sz, fmt, va);
        va_end(va);
        lstrcatA(s_szBuffer, sz);
        return n;
    }

    void __cdecl win32_abort(void)
    {
        MessageBoxA(NULL, s_szBuffer, progname, MB_ICONERROR);
        exit(-1);
    }

    int __cdecl win32_exit(int n)
    {
        MessageBoxA(NULL, s_szBuffer, progname, MB_ICONERROR);
        exit(n);
    }
#endif  // def MSGBOXOUTPUT

//////////////////////////////////////////////////////////////////////////////
