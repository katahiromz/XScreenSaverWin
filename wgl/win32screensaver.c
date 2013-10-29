#include <windows.h>
#include <scrnsave.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "resource.h"

#include "win32.h"

#pragma comment(lib, "scrnsave.lib")

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

static SCREENSAVER ss;

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

    wglMakeCurrent(ss.hdc, ss.hglrc);

    ss.modeinfo.hdc = ss.hdc;
    ss.modeinfo.hwnd = hwnd;
    ss.modeinfo.num_screen = 1;
    ss.modeinfo.screen_number = 0;
    ss.modeinfo.width = ss.w;
    ss.modeinfo.height = ss.h;
    ss.modeinfo.polygon_count = 0;
    ss.modeinfo.fps_p = False;
    ss.modeinfo.is_drawn = True;

    hack_init(&ss.modeinfo);

    return TRUE;
}

void ss_term(void)
{
    hack_free(&ss.modeinfo);
    wglMakeCurrent(NULL, NULL);
    ReleaseDC(ss.hwnd, ss.hdc);
    wglDeleteContext(ss.hglrc);
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

HGLRC init_GL(ModeInfo *mi)
{
    return ss.hglrc;
}

void do_fps(ModeInfo *mi)
{
}
