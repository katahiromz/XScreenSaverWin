// pixmap.c
// Author: katahiromz
// License: GPL v3
#include "xws2win.h"

//////////////////////////////////////////////////////////////////////////////

Pixmap XCreatePixmap(
    Display* dpy, Drawable d,
    unsigned int width, unsigned int height,
    unsigned int depth)
{
    BITMAPINFO bi;
    DrawableData *data;

    /* Callers occasionally compute a 0 (or, via unsigned wraparound, huge)
       width/height from bogus font metrics. CreateDIBSection() then fails
       and returns NULL, but the old code returned the DrawableData wrapper
       anyway with hbm/pbBits == NULL -- every later GDI/pbBits access on
       that "pixmap" is a NULL dereference (SIGSEGV). Clamp to a harmless
       minimum instead of propagating a broken handle. */
    if (width < 1 || width > 32768)  width  = 1;
    if (height < 1 || height > 32768) height = 1;

    data = (DrawableData *)calloc(1, sizeof(DrawableData));
    if (data == NULL)
        return 0;

    ZeroMemory(&bi, sizeof(bi));
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = width;
    bi.bmiHeader.biHeight = -(long)height;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    data->hbm = CreateDIBSection(dpy, &bi, DIB_RGB_COLORS,
        (LPVOID *)&data->pbBits, NULL, 0);

    if (data->hbm == NULL)
    {
        /* CreateDIBSection can still fail (e.g. GDI object exhaustion).
           Don't hand back a pixmap whose backing store doesn't exist. */
        fprintf(stderr,
            "XCreatePixmap: CreateDIBSection failed (%ux%u)\n",
            width, height);
        free(data);
        return 0;
    }

    return data;
}

#if 0
    Pixmap XCreateBitmapFromData(
        Display* dpy, Drawable d,
        const char* data,
        unsigned int width, unsigned int height)
    {
        assert(0);
        return 0;
    }
#endif

Pixmap XCreatePixmapFromBitmapData(
    Display* dpy, Drawable d, char* data,
    unsigned int width, unsigned int height,
    unsigned long fg, unsigned long bg,
    unsigned int depth)
{
    Pixmap pixmap;
    XImage *ximage;
    GC gc;
    XGCValues values;

    ximage = XCreateImage(dpy, NULL, 1, ZPixmap,
        0, NULL, width, height, 8, 0);
	ximage->data = data;
    pixmap = XCreatePixmap(dpy, d, width, height, 32);

    values.background = bg == 1 ? 255 : 0;
    values.foreground = fg == 1 ? 255 : 0;
    gc = XCreateGC(dpy, pixmap, GCBackground | GCForeground, &values);
    XPutImage(dpy, pixmap, gc, ximage, 0, 0, 0, 0, width, height);
    XFreeGC(dpy, gc);

    ximage->data = NULL;
    XDestroyImage(ximage);

    return pixmap;
}

int XFreePixmap(Display *dpy, Pixmap pixmap)
{
    if (!pixmap)
        return 0;

    XDestroyDrawableDC_(pixmap);

    if (pixmap->hbm)
        DeleteObject(pixmap->hbm);
    free(pixmap);
    return 0;
}

XPixmapFormatValues *XListPixmapFormats(Display *dpy, int *count)
{
    XPixmapFormatValues *values;
    assert(count != NULL);
    *count = 0;
    values = (XPixmapFormatValues *)calloc(1, sizeof(XPixmapFormatValues));
    if (values != NULL)
    {
        values->depth = 32;
        values->bits_per_pixel = 32;
        *count = 1;
    }
    return values;
}

//////////////////////////////////////////////////////////////////////////////
