#ifndef __XWS2WIN_H__
#define __XWS2WIN_H__

//////////////////////////////////////////////////////////////////////////////

#ifndef STRICT
    #define STRICT
#endif

#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>
#include <assert.h>

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
typedef int Visual;
typedef unsigned long Colormap;

DrawableData* XGetDrawableData_(Drawable d);

//////////////////////////////////////////////////////////////////////////////
// XPoint, XSegment, XRectangle, XArc

#if XWS2WIN_SPEED
    typedef POINT XPoint;
#else
    typedef struct {
        short x, y;
    } XPoint;
#endif

typedef struct {
     short x1, y1, x2, y2;
} XSegment;

typedef struct {
    short x, y;
    unsigned short width, height;
} XRectangle;

typedef struct {
    short x, y;
    unsigned short width, height;
    short angle1, angle2;
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
    GXandInverted = R2_NOTMASKPEN
};

//////////////////////////////////////////////////////////////////////////////
// GC

#define GCForeground (1 << 0)
#define GCBackground (1 << 1)
#define GCFillStyle  (1 << 2)
#define GCFunction   (1 << 3)
#define GCStipple    (1 << 4)
#define GCLineWidth  (1 << 5)
#define GCCapStyle   (1 << 6)
#define GCJoinStyle  (1 << 7)
#define GCLineStyle  (1 << 8)
#define GCFillRule   (1 << 9)
#define GCGraphicsExposures (1 << 10)

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
    CapProjecting = CapButt
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
    HBITMAP hbmOld;
} XGCValues;

typedef XGCValues *GC;

#define XGetGCValues_(gc) gc
HDC XCreateDrawableDC_(Display *dpy, Drawable d);
int XDeleteDrawableDC_(Display *dpy, Drawable d, HDC hdc);

GC XCreateGC(Display *dpy, Drawable d,
     unsigned long valuemask, 
     XGCValues *values);
int XChangeGC(Display* dpy, GC gc, unsigned long valuemask, XGCValues* values);

int XFreeGC(Display *dpy, GC gc);

HPEN XCreateWinPen(XGCValues *values);
HBRUSH XCreateWinBrush(XGCValues *values);
HFONT XCreateWinFont(XGCValues *values);
int XSetForeground(Display *dpy, GC gc, unsigned long foreground);
int XSetBackground(Display *dpy, GC gc, unsigned long background);
int XSetWindowBackground(Display *dpy, Window w, unsigned long pixel);

int XCopyArea(
     Display *dpy,
     Drawable src_drawable, Drawable dst_drawable, GC gc,
     int src_x, int src_y,
     unsigned int width, unsigned int height,
     int dst_x, int dst_y);

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

int XQueryColor(Display *dpy, Colormap cmap, XColor *def);
int XParseColor(Display *d, Colormap cmap, const char *name, XColor *c);

unsigned long load_color(Display *dpy, Colormap cmap, const char *name);

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
#define DefaultScreenOfDisplay(dpy) 0
#define DefaultScreen(dpy) 0
#define BlackPixelOfScreen(s) 0
#define WhitePixelOfScreen(s) 255
#define DefaultColormap(dpy,scr) 0
#define BlackPixel(dpy,scr) 0
#define WhitePixel(dpy,scr) 255
#define BitmapPad(dpy) 32
#define ImageByteOrder(dpy) LSBFirst

Status XGetWindowAttributes(Display *dpy, Window w, XWindowAttributes *attr);
int XSetLineAttributes(Display *dpy, GC gc,
    unsigned int line_width, int line_style,
    int cap_style, int join_style);

int XClearWindow(Display *dpy, Window w);

int XDrawPoint(Display *dpy, Drawable d, GC gc,
    int x, int y);
int XDrawPoints(Display *dpy, Drawable d, GC gc,
    XPoint *points, int npoints, int CoordMode);

int XDrawLine(Display *dpy, Drawable d, GC gc,
    int x1, int y1, int x2, int y2);
int XDrawLines(Display *dpy, Drawable d, GC gc,
    XPoint *points, int npoints, int mode);

int XDrawRectangle(
    Display *dpy, Drawable d, GC gc,
    int x, int y, unsigned int width, unsigned int height);

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

int XFillRectangle(
    Display *dpy, Drawable d, GC gc,
    int x, int y, unsigned int width, unsigned int height);
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

int XPutImage(Display *dpy, Drawable d, GC gc,
    XImage *image, int req_xoffset, int req_yoffset,
    int x, int y, unsigned int req_width, unsigned int req_height);

int XFlush(Display *d);

#define XMaxRequestSize(dpy) 100

//////////////////////////////////////////////////////////////////////////////

#endif  // ndef __XWS2WIN_H__
