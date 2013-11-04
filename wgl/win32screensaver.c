#include "win32.h"
#include <scrnsave.h>
#include "resource.h"

#ifdef _UNICODE
    #pragma comment(lib, "scrnsavw.lib")
#else
    #pragma comment(lib, "scrnsave.lib")
#endif

PIXELFORMATDESCRIPTOR pfd =
{
    sizeof(PIXELFORMATDESCRIPTOR),   // size
    1,                     // version number
    PFD_DRAW_TO_WINDOW |   // support window
    PFD_SUPPORT_OPENGL |   // support OpenGL
    PFD_DOUBLEBUFFER,      // double buffered
    PFD_TYPE_RGBA,         // RGBA type
    24,                    // 24-bit color depth
    0, 0, 0, 0, 0, 0,      // color bits ignored
    0,                     // no alpha buffer
    0,                     // shift bit ignored
    0,                     // no accumulation buffer
    0, 0, 0, 0,            // accum bits ignored
    32,                    // 32-bit z-buffer
    0,                     // no stencil buffer
    0,                     // no auxiliary buffer
    PFD_MAIN_PLANE,        // main layer
    0,                     // reserved
    0, 0, 0                // layer masks ignored
};

SCREENSAVER ss;

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

BOOL ss_init(HWND hwnd)
{
    RECT rc;
    INT iPixelFormat;

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

    iPixelFormat = ChoosePixelFormat(ss.hdc, &pfd);
    SetPixelFormat(ss.hdc, iPixelFormat, &pfd);
    ss.hglrc = wglCreateContext(ss.hdc);
    if (ss.hglrc == NULL)
        return FALSE;

    ss.hbmScreenShot = GetScreenShotBitmap();
    if (ss.hbmScreenShot == NULL)
    {
        ReleaseDC(hwnd, ss.hdc);
        return FALSE;
    }
    //SaveBitmapToFile("screenshot.bmp", ss.hbmScreenShot);

    wglMakeCurrent(ss.hdc, ss.hglrc);

    ss.modeinfo.dpy = ss.hdc;
    ss.modeinfo.window = 0;
    ss.modeinfo.num_screen = 1;
    ss.modeinfo.screen_number = 0;
    ss.modeinfo.width = ss.w;
    ss.modeinfo.height = ss.h;
    ss.modeinfo.polygon_count = 0;
    ss.modeinfo.fps_p = False;
    ss.modeinfo.is_drawn = True;
    ss.modeinfo.count = 10;
    ss.modeinfo.xgwa.width = ss.w;
    ss.modeinfo.xgwa.height = ss.h;
    ss.modeinfo.xgwa.depth = 32;
    ss.modeinfo.xgwa.visual = NULL;
    ss.modeinfo.xgwa.colormap = 0;
    ss.modeinfo.xgwa.screen = hwnd;

#undef ya_rand_init
    ya_rand_init(0);
    hack_init(&ss.modeinfo);

    return TRUE;
}

void ss_term(void)
{
    hack_free(&ss.modeinfo);
    wglMakeCurrent(NULL, NULL);
    ReleaseDC(ss.hwnd, ss.hdc);
    wglDeleteContext(ss.hglrc);
    DeleteObject(ss.hbmScreenShot);
    ss.hbmScreenShot = NULL;
}

BOOL WINAPI RegisterDialogClasses(HANDLE hInst)
{
    return TRUE;
}

BOOL WINAPI ScreenSaverConfigureDialog(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
    case WM_INITDIALOG:
        return TRUE;

    case WM_CLOSE:
        EndDialog(hWnd, 0);
        break;

    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case IDOK:
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

HGLRC *init_GL(ModeInfo *mi)
{
    return &ss.hglrc;
}

void do_fps(ModeInfo *mi)
{
}

void glXMakeCurrent(Display *d, Window w, GLXContext context)
{
    wglMakeCurrent((HDC)d, context);
}

void glXSwapBuffers(Display *d, Window w)
{
    SwapBuffers(d);
}

trackball_state *gltrackball_init(void)
{
    return NULL;
}

void gltrackball_rotate(trackball_state *trackball)
{
}

float current_device_rotation(void)
{
    return 0.0;
}

void check_gl_error(const char *name)
{
}

void clear_gl_error(void)
{
}

void gltrackball_get_quaternion(char **ppch, float q[4])
{
    q[0] = q[1] = q[2] = q[3] = 0.0;
}

Display *DisplayOfScreen(Screen *s)
{
    return GetWindowDC(s);
}

Bool has_writable_cells (Screen *s, Visual *v)
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

void gltrackball_start(trackball_state* trackball, int n1, int n2, int n3, int n4)
{
}

void gltrackball_track(trackball_state* trackball, int n1, int n2, int n3, int n4)
{
}

#ifdef MSGBOXOUTPUT
int __cdecl win32_fprintf(FILE *fp, const char *fmt, ...)
{
    CHAR sz[512];
    va_list va;
    int n;
    va_start(va, fmt);
    n = wvsprintf(sz, fmt, va);
    va_end(va);
    MessageBoxA(NULL, sz, progname, 0);
    return n;
}
#endif  // def MSGBOXOUTPUT
