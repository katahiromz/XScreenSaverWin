#include "xlockmore.h"

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

BOOL InitPixelFormat(SCREENSAVER *ss)
{
    INT iPixelFormat = ChoosePixelFormat(ss->hdc, &pfd);
    SetPixelFormat(ss->hdc, iPixelFormat, &pfd);
    ss->hglrc = wglCreateContext(ss->hdc);

    if (hack_ncolors_enabled)
    {
		int i;
        ModeInfo *mi = &ss->modeinfo;
        mi->colors = (XColor *) calloc(mi->npixels, sizeof(*mi->colors));
        if (mi->colors == NULL)
            return FALSE;

        switch (hack_color_scheme)
        {
        case color_scheme_uniform:
            make_uniform_colormap(mi->xgwa.screen, mi->xgwa.visual,
                mi->xgwa.colormap,
                mi->colors, &mi->npixels,
                True, &mi->writable_p, True);
            break;

        case color_scheme_smooth:
            make_smooth_colormap(mi->xgwa.screen, mi->xgwa.visual,
                mi->xgwa.colormap,
                mi->colors, &mi->npixels,
                True, &mi->writable_p, True);
            break;

        case color_scheme_bright:
        case color_scheme_default:
            make_random_colormap(mi->xgwa.screen, mi->xgwa.visual,
                mi->xgwa.colormap,
                mi->colors, &mi->npixels,
                (hack_color_scheme == color_scheme_bright),
                True, &mi->writable_p, True);
            break;

        default:
            fprintf(stderr, "Bad color scheme\n");
            abort();
        }

        mi->pixels = (unsigned long *)calloc(mi->npixels, sizeof(*mi->pixels));
        if (mi->pixels == NULL)
        {
            free(mi->colors);
            return FALSE;
        }

        for (i = 0; i < mi->npixels; i++)
            mi->pixels[i] = mi->colors[i].pixel;
    }

    return (ss->hglrc != NULL);
}

VOID MakeCurrent(SCREENSAVER *ss)
{
    wglMakeCurrent(ss->hdc, ss->hglrc);
}

extern HANDLE g_hMapping;

void ss_term(void)
{
    free(ss.modeinfo.colors);
    free(ss.modeinfo.pixels);
    hack_free(&ss.modeinfo);
    ReleaseDC(ss.hwnd, ss.hdc);
    DeleteObject(ss.hbmScreenShot);
    CloseHandle(g_hMapping);
}

void ss_clear(Display *d)
{
    glClear(GL_COLOR_BUFFER_BIT);
}

HGLRC *init_GL(ModeInfo *mi)
{
    return &ss.hglrc;
}

void glXMakeCurrent(Display *d, Window w, GLXContext context)
{
    wglMakeCurrent((HDC)d, context);
}

void glXSwapBuffers(Display *d, Window w)
{
    SwapBuffers(d);
}

void check_gl_error(const char *name)
{
}

void clear_gl_error(void)
{
}
