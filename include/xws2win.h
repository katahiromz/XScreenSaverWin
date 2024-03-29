#ifndef __XWS2WIN_H__
#define __XWS2WIN_H__

//////////////////////////////////////////////////////////////////////////////

#ifndef STRICT
    #define STRICT
#endif

#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <commctrl.h>
#define _USE_MATH_DEFINES
#ifdef __cplusplus
    #include <cstdlib>
    #include <cstdio>
    #include <cstring>
    #include <cmath>
    #include <ctime>
    #include <cassert>
#else
    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>
    #include <math.h>
    #include <time.h>
    #include <assert.h>
#endif

#undef min
#undef max
#undef far
#undef hyper
#undef IN
#undef OUT

//////////////////////////////////////////////////////////////////////////////

#define USE_GL 1
#define XWS2WIN_SPEED 1

#define HAVE_GLBINDTEXTURE 1
#define HAVE_DOUBLE_BUFFER_EXTENSION 1

#define strcasecmp _stricmp
#define strncasecmp _strnicmp

#define Xmalloc malloc
#define Xcalloc calloc
#define Xrealloc realloc
#define Xfree free

#define XFree free

typedef BYTE CARD8;
typedef WORD CARD16;
typedef DWORD CARD32;

#define None NULL

#define rint(e) ((int)(e + 0.5))

#define MAX_COLORCELLS 300

//////////////////////////////////////////////////////////////////////////////
// Bool

typedef BOOL Bool;
#define False FALSE
#define True TRUE

//////////////////////////////////////////////////////////////////////////////

typedef struct
{
    HBITMAP hbm;
    LPBYTE pbBits;
    HBITMAP hbmOld;
} DrawableData;

typedef DrawableData* Drawable;
typedef DrawableData* Pixmap;
typedef DrawableData* Window;

enum { TrueColor, DirectColor, PseudoColor, StaticColor, GrayScale };

typedef struct
{
    unsigned long red_mask;
    unsigned long green_mask;
    unsigned long blue_mask;
    int class_;
    int bits_per_rgb;
} Visual;

typedef unsigned long Colormap;

typedef char *XPointer;

#define MAX_COLORMAP 16

//////////////////////////////////////////////////////////////////////////////
// XPoint, XSegment, XRectangle, XArc

typedef struct
{
    int x, y;
} XPoint;

typedef struct
{
     int x1, y1, x2, y2;
} XSegment;

typedef struct
{
    int x, y;
    unsigned int width, height;
} XRectangle;

typedef struct
{
    int x, y;
    unsigned int width, height;
    int angle1, angle2;
} XArc;

//////////////////////////////////////////////////////////////////////////////

typedef int Status;

#ifdef STRICT
    #define Display struct HDC__
#else
    #define Display void
#endif

typedef int Screen;
typedef HGLRC GLXContext;

typedef int VisualID;

typedef struct
{
    Visual *visual;
    VisualID visualid;
    int screen;
    int depth;
    int c_class;
    unsigned long red_mask;
    unsigned long green_mask;
    unsigned long blue_mask;
    int colormap_size;
    int bits_per_rgb;
} XVisualInfo;

VisualID XVisualIDFromVisual(Visual *visual);

enum { VisualScreenMask, VisualIDMask };

XVisualInfo *XGetVisualInfo(Display *dpy, long visual_info_mask,
    XVisualInfo *visual_info_template, int *nitems);

//////////////////////////////////////////////////////////////////////////////
// XWindowAttributes

typedef struct
{
    int width;
    int height;
    int depth;
    Visual *visual;
    Colormap colormap;
    Screen *screen;
} XWindowAttributes;

//////////////////////////////////////////////////////////////////////////////

typedef struct
{
    int depth;
    int bits_per_pixel;
} XPixmapFormatValues;

//////////////////////////////////////////////////////////////////////////////
// XrmOptionDescRec, argtype, OptionStruct, ModeSpecOpt, ModStruct

typedef enum
{
    XrmoptionNoArg,
    XrmoptionSepArg
} Xrmoption;

typedef struct
{
    char *opt;
    char *prop;
    Xrmoption type;
    char *def;
} XrmOptionDescRec;

//////////////////////////////////////////////////////////////////////////////

enum { CoordModeOrigin, CoordModePrevious };
enum { Convex, Nonconvex, Complex };
enum { BadAlloc = 1, BadGC, BadValue, BadFont, BadMatch, BadPixmap };
enum {
    GXcopy = R2_COPYPEN, GXor = R2_MERGEPEN, GXxor = R2_XORPEN,
    GXandInverted = R2_NOTMASKPEN, GXset = R2_WHITE, GXclear = R2_BLACK,
    GXand = R2_MASKPEN
};
enum { AllPlanes };

//////////////////////////////////////////////////////////////////////////////
// fonts

typedef struct 
{
    short   lbearing;
    short   rbearing;
    short   width;
    short   ascent;
    short   descent;
    unsigned short attributes;
} XCharStruct;

typedef HFONT Font;

typedef struct 
{
    Font        fid;
    unsigned    min_char_or_byte2;
    unsigned    max_char_or_byte2;
    XCharStruct min_bounds;
    XCharStruct max_bounds;
    XCharStruct *per_char;
    int     ascent;
    int     descent;
} XFontStruct;

//////////////////////////////////////////////////////////////////////////////
// GC

#define GCForeground        (1 << 0)
#define GCBackground        (1 << 1)
#define GCFillStyle         (1 << 2)
#define GCFunction          (1 << 3)
#define GCStipple           (1 << 4)
#define GCLineWidth         (1 << 5)
#define GCCapStyle          (1 << 6)
#define GCJoinStyle         (1 << 7)
#define GCLineStyle         (1 << 8)
#define GCFillRule          (1 << 9)
#define GCGraphicsExposures (1 << 10)
#define GCSubwindowMode     (1 << 11)
#define GCFont              (1 << 12)
#define GCPlaneMask         (1 << 13)
#define GCClipXOrigin       (1 << 14)
#define GCClipYOrigin       (1 << 15)
#define GCClipMask          (1 << 16)

enum    // line_style
{
    LineSolid = PS_SOLID,
    LineOnOffDash = PS_DASH,
};

enum    // cap_style
{
    CapNotLast = PS_ENDCAP_FLAT,
    CapButt = PS_ENDCAP_SQUARE,
    CapRound = PS_ENDCAP_ROUND,
    CapProjecting = PS_ENDCAP_FLAT
};

enum    // join_style
{
    JoinMiter = PS_JOIN_MITER,
    JoinRound = PS_JOIN_ROUND,
    JoinBevel = PS_JOIN_BEVEL
};

enum    // fill_style
{
    FillSolid = BS_SOLID, FillStippled = BS_DIBPATTERN,
    FillOpaqueStippled = BS_DIBPATTERN
};

enum    // fill_rule
{
    EvenOddRule, WindingRule
};

enum    // subwindow_mode
{
    ClipByChildren, IncludeInferiors
};

typedef struct
{
    unsigned long foreground;
    COLORREF foreground_rgb;
    unsigned long background;
    COLORREF background_rgb;
    unsigned int line_width;
    int line_style;
    int cap_style;
    int join_style;
    int function;
    int fill_style;
    Pixmap stipple;
    int fill_rule;
    Bool graphics_exposures;
    int subwindow_mode;
    unsigned long plane_mask;
    HBITMAP hbmOld;
    Font font;
    int clip_x_origin;
    int clip_y_origin;
    HRGN clip_mask_region;
} XGCValues;

typedef XGCValues *GC;

#define XGetGCValues_(gc) gc

static inline HDC XCreateDrawableDC_(Display *dpy, Drawable d)
{
    HDC hdc;

    if (d != NULL)
    {
        hdc = CreateCompatibleDC(dpy);
        assert(hdc != NULL);
        d->hbmOld = (HBITMAP)SelectObject(hdc, d->hbm);
    }
    else
    {
        hdc = dpy;
    }

    return hdc;
}

static inline int XDeleteDrawableDC_(Display *dpy, Drawable d, HDC hdc)
{
    if (d != NULL)
    {
        SelectObject(hdc, d->hbmOld);
        DeleteDC(hdc);
    }
    return 0;
}

GC XCreateGC(Display *dpy, Drawable d,
     unsigned long valuemask, 
     XGCValues *values);
int XChangeGC(Display* dpy, GC gc, unsigned long valuemask, XGCValues* values);

int XFreeGC(Display *dpy, GC gc);

HPEN XCreateWinPen_(XGCValues *values);
HBRUSH XCreateWinBrush_(XGCValues *values);

int XCopyArea(
     Display *dpy,
     Drawable src_drawable, Drawable dst_drawable, GC gc,
     int src_x, int src_y,
     unsigned int width, unsigned int height,
     int dst_x, int dst_y);
int XCopyPlane(Display *dpy, Drawable src_drawable, Drawable dst_drawable, GC gc,
     int src_x, int src_y, unsigned int width, unsigned int height,
     int dst_x, int dst_y, unsigned long bit_plane);

//////////////////////////////////////////////////////////////////////////////

XFontStruct *XLoadQueryFont(Display *dpy, const char *name);
int XUnloadFont(Display *dpy, Font fid);
int XFreeFont(Display *dpy, XFontStruct *fs);
int XTextExtents(XFontStruct *fs, const char *string, int nchars,
    int *dir, int *font_ascent, int *font_descent, XCharStruct *overall);
int XTextWidth(XFontStruct *fs, const char *string, int count);
int XSetFont(Display *dpy, GC gc, Font fid);
int XFreeFontInfo(char **names, XFontStruct *info, int actualCount);

//////////////////////////////////////////////////////////////////////////////
// XColor

#define DoRed    (1 << 0)
#define DoGreen  (1 << 1)
#define DoBlue   (1 << 2)

typedef struct
{
  unsigned long pixel;
  unsigned short red, green, blue;
  char flags;  /* DoRed, DoGreen, DoBlue */
  char pad;
} XColor;

Bool XAllocColor(Display *d, Colormap cmap, XColor *color);

Bool XAllocNamedColor(
    Display *d, Colormap cmap, const char *name,
    XColor *near_color, XColor *true_color);

Status XAllocColorCells(
    Display*        d,
    Colormap        cmap,
    Bool            contig,
    unsigned long*  plane_masks_return,
    unsigned int    nplanes,
    unsigned long*  pixels_return,
    unsigned int    npixels);

int XFreeColors(
    Display*        d,
    Colormap        cmap,
    unsigned long*  pixels,
    int             npixels,
    unsigned long   planes);

int XStoreColors(
    Display*    display,
    Colormap    cmap,
    XColor*     color,
    int         ncolors);

unsigned long load_color(Display *dpy, Colormap cmap, const char *name);

#define CellsOfScreen(s) 300
#define DefaultScreenOfDisplay(dpy) 0
#define DefaultColormap(dpy,scr) 0

typedef struct
{
    WORD red;
    WORD green;
    WORD blue;
} xColorItem;

typedef struct
{
    int num_items;
    xColorItem *items;
    BOOL *pixel_used;
} ColormapData;

extern ColormapData colormaps[MAX_COLORMAP];

static inline int XQueryColor(Display *dpy, Colormap cmap, XColor *def)
{
    int pixel;
    assert(def != NULL);
    if (def->pixel < MAX_COLORCELLS && colormaps[cmap].pixel_used[def->pixel])
    {
        pixel = def->pixel;
        def->red = colormaps[cmap].items[pixel].red;
        def->blue = colormaps[cmap].items[pixel].blue;
        def->green = colormaps[cmap].items[pixel].green;
        def->flags = DoRed | DoGreen | DoBlue;
        return 1;
    }
    return 0;
}

static inline int XSetForeground(Display *dpy, GC gc, unsigned long foreground)
{
    XColor color;
    XGCValues *values;

    values = XGetGCValues_(gc);
    values->foreground = foreground;
    color.pixel = foreground;
    XQueryColor(dpy, DefaultColormap(dpy, DefaultScreenOfDisplay(dpy)), &color);
    values->foreground_rgb = RGB(color.red / 256, color.green / 256, color.blue / 256);
    return 0;
}

int XQueryColors(Display *dpy, Colormap cmap, XColor *defs, int ncolors);
int XParseColor(Display *d, Colormap cmap, const char *name, XColor *c);
int XSetBackground(Display *dpy, GC gc, unsigned long background);
int XSetWindowBackground(Display *dpy, Window w, unsigned long pixel);

//////////////////////////////////////////////////////////////////////////////
// XImage

// format
enum { XYBitmap, XYPixmap, ZPixmap, RGBAPixmap_ };

// byte_order/bitmap_bit_order
enum { MSBFirst, LSBFirst };

typedef struct
{
    int depth;
    unsigned int width, height;
    int format;
    int bytes_per_line;
    int bits_per_pixel;
    int bitmap_bit_order;
    int byte_order;
    char *data;
    int xoffset, yoffset;
    unsigned int red_mask, green_mask, blue_mask;
    int bitmap_pad;
} XImage;

#define MASK_LH(l, h)       (((1 << (1 + (h) - (l))) - 1) << (l))
#define BM_ARGB(a, r, g, b) MASK_LH(0, (b) - 1)
#define GM_ARGB(a, r, g, b) MASK_LH(b, (b) + (g) - 1)
#define RM_ARGB(a, r, g, b) MASK_LH((b) + (g), (b) + (g) + (r) - 1)
#define AM_ARGB(a, r, g, b) MASK_LH((b) + (g) + (r), \
                                    (b) + (g) + (r) + (a) - 1)

XImage *XCreateImage(Display *dpy, Visual *visual, int depth, int format,
    int offset, char *data, unsigned int width, unsigned int height,
    int xpad, int image_bytes_per_line);
int XDestroyImage(XImage *image);

unsigned long XGetPixel(XImage *image, int x, int y);
int XPutPixel(XImage *ximage, int x, int y, unsigned long pixel);

Pixmap XCreatePixmap(
    Display* dpy, Drawable d,
    unsigned int width, unsigned int height,
    unsigned int depth);
Pixmap XCreateBitmapFromData(
    Display* dpy, Drawable d,
    const char* data,
    unsigned int width, unsigned int height);
Pixmap XCreatePixmapFromBitmapData(
    Display* dpy, Drawable d,
    char* data,
    unsigned int width, unsigned int height,
    unsigned long fg, unsigned long bg,
    unsigned int depth);
int XFreePixmap(Display *dpy, Pixmap pixmap);

int XSync(Display *dpy, Bool b);

XPixmapFormatValues *XListPixmapFormats(Display *dpy, int *count);

//////////////////////////////////////////////////////////////////////////////
// X Window

Display *DisplayOfScreen(Screen *s);
#define DefaultScreen(dpy) 0
#define BlackPixelOfScreen(s) 0
#define WhitePixelOfScreen(s) 255
#define BlackPixel(dpy,scr) 0
#define WhitePixel(dpy,scr) 255
#define BitmapPad(dpy) 32
#define ImageByteOrder(dpy) LSBFirst
#define XScreenNumberOfScreen(screen) 1
int XDisplayWidth(Display *dpy, int scr);
int XDisplayHeight(Display *dpy, int scr);
Window RootWindow(Display *dpy, int scr);
#define RootWindowOfScreen(scr) 0
#define ScreenOfDisplay(dpy,i) 0

Status XGetWindowAttributes(Display *dpy, Window w, XWindowAttributes *attr);
int XSetLineAttributes(Display *dpy, GC gc,
    unsigned int line_width, int line_style,
    int cap_style, int join_style);

int XClearWindow(Display *dpy, Window w);
int XClearWindow2_(Display *dpy, Window w, GC gc);

int XDrawPoint(Display *dpy, Drawable d, GC gc,
    int x, int y);
int XDrawPoints(Display *dpy, Drawable d, GC gc,
    XPoint *points, int npoints, int CoordMode);

int XDrawLine(Display *dpy, Drawable d, GC gc,
    int x1, int y1, int x2, int y2);
int XDrawLines(Display *dpy, Drawable d, GC gc,
    XPoint *points, int npoints, int mode);

static inline int XDrawRectangle(
    Display *dpy, Drawable d, GC gc,
    int x, int y, unsigned int width, unsigned int height)
{
    XGCValues *values;
    HDC hdc;
    HPEN hPen;
    HGDIOBJ hPenOld;
    int nR2;

    values = XGetGCValues_(gc);
    hPen = XCreateWinPen_(values);
    assert(hPen);
    if (hPen == NULL)
        return BadAlloc;

    hdc = XCreateDrawableDC_(dpy, d);
    nR2 = SetROP2(hdc, values->function);
    hPenOld = SelectObject(hdc, hPen);

    SelectObject(hdc, GetStockObject(NULL_BRUSH));
    Rectangle(hdc, x, y, x + width, y + height);

    SelectObject(hdc, hPenOld);
    SetROP2(hdc, nR2);
    XDeleteDrawableDC_(dpy, d, hdc);

    DeleteObject(hPen);
    return 0;
}

static inline int XDrawRectangleSimplified(
    Display *dpy, Drawable d, GC gc,
    int x, int y, unsigned int width, unsigned int height)
{
    XGCValues *values;
    HDC hdc;
    HPEN hPen;
    HGDIOBJ hPenOld;
    int nR2;

    values = XGetGCValues_(gc);
    hPen = XCreateWinPen_(values);
    //assert(hPen);
    //if (hPen == NULL)
    //    return BadAlloc;

    hdc = XCreateDrawableDC_(dpy, d);
    //nR2 = SetROP2(hdc, values->function);
    hPenOld = SelectObject(hdc, hPen);

    SelectObject(hdc, GetStockObject(NULL_BRUSH));
    Rectangle(hdc, x, y, x + width, y + height);

    SelectObject(hdc, hPenOld);
    //SetROP2(hdc, nR2);
    XDeleteDrawableDC_(dpy, d, hdc);

    DeleteObject(hPen);
    return 0;
}

int XDrawSegments(Display *dpy, Drawable d, GC gc,
    XSegment *segments, int nsegments);

int XDrawArc(Display *dpy, Drawable d, GC gc,
    int x, int y, unsigned int width, unsigned int height,
    int angle1, int angle2);
int XDrawArcs(Display *dpy, Drawable d, GC gc,
    XArc *arcs, int n_arcs);

int XDrawString(Display *dpy, Drawable d, GC gc,
    int x, int y, const char *string, int length);
int XDrawImageString(Display *dpy, Drawable d, GC gc,
    int x, int y, const char *string, int length);

static inline int XFillRectangle(
    Display *dpy, Drawable d, GC gc,
    int x, int y, unsigned int width, unsigned int height)
{
    XGCValues *values;
    HDC hdc;
    HBRUSH hbr;
    RECT rc;
    int nR2;

    values = XGetGCValues_(gc);
    hbr = XCreateWinBrush_(values);
    if (hbr == NULL)
        return BadAlloc;

    SetRect(&rc, x, y, x + width, y + height);

    hdc = XCreateDrawableDC_(dpy, d);
    nR2 = SetROP2(hdc, values->function);

    if (values->clip_mask_region)
    {
        SelectClipRgn(hdc, values->clip_mask_region);
        OffsetClipRgn(hdc, values->clip_x_origin, values->clip_y_origin);
    }

    SetPolyFillMode(hdc, (values->fill_rule == EvenOddRule ? ALTERNATE : WINDING));
    FillRect(hdc, &rc, hbr);

    if (values->clip_mask_region)
        SelectClipRgn(hdc, NULL);

    SetROP2(hdc, nR2);
    XDeleteDrawableDC_(dpy, d, hdc);

    DeleteObject(hbr);
    return 0;
}

static inline int XFillRectangleSimplified(
    Display *dpy, Drawable d, GC gc,
    int x, int y, unsigned int width, unsigned int height)
{
    XGCValues *values;
    HDC hdc;
    HBRUSH hbr;
    RECT rc;
    //int nR2;

    values = XGetGCValues_(gc);

    hbr = CreateSolidBrush(values->foreground_rgb); //XCreateWinBrush_(values);
    //if (hbr == NULL)
    //    return BadAlloc;

    rc.left = x;
    rc.top = y;
    rc.right = x + width;
    rc.bottom = y + height;

    hdc = XCreateDrawableDC_(dpy, d);
    //nR2 = SetROP2(hdc, values->function);

    //if (values->clip_mask_region)
    //{
    //    SelectClipRgn(hdc, values->clip_mask_region);
    //    OffsetClipRgn(hdc, values->clip_x_origin, values->clip_y_origin);
    //}

    //SetPolyFillMode(hdc, (values->fill_rule == EvenOddRule ? ALTERNATE : WINDING));
    FillRect(hdc, &rc, hbr);

    //if (values->clip_mask_region)
    //    SelectClipRgn(hdc, NULL);

    //SetROP2(hdc, nR2);
    XDeleteDrawableDC_(dpy, d, hdc);

    DeleteObject(hbr);
    return 0;
}

int XFillRectangles(
    Display *dpy, Drawable d, GC gc,
    XRectangle *rectangles, int n_rects);

int XFillPolygon(
    Display *dpy, Drawable d, GC gc,
    XPoint *points, int n_points,
    int shape, int mode);

int XFillArc(Display *dpy, Drawable d, GC gc,
    int x, int y, unsigned int width, unsigned int height,
    int angle1, int angle2);
int XFillArcs(Display *dpy, Drawable d, GC gc,
    XArc *arcs, int n_arcs);

int XSetFunction(Display *dpy, GC gc, int function);
int XSetFillStyle(Display *dpy, GC gc, int fill);

Bool XQueryPointer(Display *dpy, Window w, Window *root, Window *child,
     int *root_x, int *root_y, int *win_x, int *win_y,
     unsigned int *mask);

int XSetGraphicsExposures(Display *dpy, GC gc, Bool graphics_exposures);

int XClearArea(
    Display *dpy, Window w,
    int x, int y, unsigned int width, unsigned int height,
    Bool exposures);
int XClearArea2_(
    Display *dpy, Window w, GC gc,
    int x, int y, unsigned int width, unsigned int height,
    Bool exposures);

void XInitImage(XImage *image);
XImage *XGetImage(Display *dpy, Drawable d,
     int x, int y, unsigned int width, unsigned int height,
     unsigned long plane_mask, int format);
int XPutImage(Display *dpy, Drawable d, GC gc,
    XImage *image, int req_xoffset, int req_yoffset,
    int x, int y, unsigned int req_width, unsigned int req_height);
int XPutImage_(Display *dpy, Drawable d, GC gc,
    XImage *image, int req_xoffset, int req_yoffset,
    int x, int y, unsigned int req_width, unsigned int req_height);
int XPutImage_2(Display *dpy, Drawable d, GC gc,
    XImage *image, int req_xoffset, int req_yoffset,
    int x, int y, unsigned int req_width, unsigned int req_height);
XImage *XSubImage(XImage *ximage,
    int x, int y, unsigned int width, unsigned int height);

int XFlush(Display *d);

int XSetSubwindowMode(Display *dpy, GC gc, int mode);

Bool XTranslateCoordinates(
     Display *dpy, Window src_win, Window dest_win,
     int src_x, int src_y, int *dst_x, int *dst_y, Window *child);
int XSetPlaneMask(Display *dpy, GC gc, unsigned long planemask);
int XSetClipOrigin(Display *dpy, GC gc, int xorig, int yorig);
int XSetClipMask(Display *dpy, GC gc, Pixmap mask);
Status XGetGeometry(
    Display *dpy, Drawable d, Window *root,
    int *x, int *y, unsigned int *width, unsigned int *height,
    unsigned int *borderWidth, unsigned int *depth);

#define XMaxRequestSize(dpy) 100

LPVOID XCreatePackedDIBFromPixmap_(
    Pixmap pixmap, COLORREF clrFore, COLORREF clrBack);

//////////////////////////////////////////////////////////////////////////////

#endif  // ndef __XWS2WIN_H__
