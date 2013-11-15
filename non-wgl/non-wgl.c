#include "xlockmore.h"

BOOL InitPixelFormat(SCREENSAVER *ss)
{
    ModeInfo *mi;
    int i;

    ss->modeinfo.gc = XCreateGC(ss->hdc, 0, 0, NULL);
    assert(ss->modeinfo.gc != NULL);

    if (hack_ncolors <= 0)
        hack_ncolors = 64;
    else if (hack_ncolors > MAX_COLORCELLS)
        hack_ncolors = MAX_COLORCELLS;

    mi = &ss->modeinfo;

	mi->install_p = fChildPreview;

    mi->writable_p = True;

    mi->black_pixel = 0;
    mi->white_pixel = 255;
    mi->npixels = hack_ncolors;
    mi->colors = NULL;
    mi->pixels = NULL;

    mi->threed = False;
    mi->threed_delta = 1.5;
	mi->threed_right_color = load_color(NULL, 0, "red");
    mi->threed_left_color = load_color(NULL, 0, "blue");
    mi->threed_both_color = load_color(NULL, 0, "magenta");
    mi->threed_none_color = load_color(NULL, 0, "black");

    if (hack_ncolors_enabled)
    {
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

    return TRUE;
}

VOID MakeCurrent(SCREENSAVER *ss)
{
}

void ss_term(void)
{
    free(ss.modeinfo.colors);
    free(ss.modeinfo.pixels);
    XFreeGC(ss.modeinfo.dpy, ss.modeinfo.gc);
    hack_free(&ss.modeinfo);
    ReleaseDC(ss.hwnd, ss.hdc);
    DeleteObject(ss.hbmScreenShot);
}

void ss_clear(Display *d)
{
    XClearWindow(d, 0);
}

Status XGetWindowAttributes(Display *dpy, Window w, XWindowAttributes *attr)
{
    *attr = ss.modeinfo.xgwa;
    return 0;
}
