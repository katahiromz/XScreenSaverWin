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

    ss.modeinfo.dpy = ss.hdc;
    ss.modeinfo.window = hwnd;
    ss.modeinfo.num_screen = 1;
    ss.modeinfo.screen_number = 0;
    ss.modeinfo.width = ss.w;
    ss.modeinfo.height = ss.h;
    ss.modeinfo.polygon_count = 0;
    ss.modeinfo.fps_p = False;
    ss.modeinfo.is_drawn = True;

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
