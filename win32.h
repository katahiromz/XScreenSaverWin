#ifndef __XSCREENSAVER_WIN32_H__
#define __XSCREENSAVER_WIN32_H__

//////////////////////////////////////////////////////////////////////////////

#ifndef STRICT
    #define STRICT
#endif

#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>
#include <assert.h>

//////////////////////////////////////////////////////////////////////////////

#define USE_GL 1
#define HAVE_GLBINDTEXTURE
#define inline
#define ENTRYPOINT static

#define strcasecmp _stricmp
#define strncasecmp _strnicmp

#define Xmalloc malloc
#define Xcalloc calloc
#define Xrealloc realloc
#define Xfree free

typedef BYTE CARD8;
typedef WORD CARD16;
typedef DWORD CARD32;

#define GL_UNSIGNED_INT_8_8_8_8_REV GL_UNSIGNED_BYTE

#ifndef  GL_TEXTURE_MAX_ANISOTROPY_EXT
# define GL_TEXTURE_MAX_ANISOTROPY_EXT      0x84FE
#endif
#ifndef  GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
# define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT  0x84FF
#endif

#ifndef GL_LIGHT_MODEL_COLOR_CONTROL
# define GL_LIGHT_MODEL_COLOR_CONTROL       0x81F8
#endif
#ifndef GL_SEPARATE_SPECULAR_COLOR
# define GL_SEPARATE_SPECULAR_COLOR         0x81FA
#endif

#define None NULL

#ifdef MSGBOXOUTPUT
    int __cdecl win32_fprintf(FILE *fp, const char *fmt, ...);
    void __cdecl win32_abort(void);
    int __cdecl win32_exit(int n);
    #define fprintf win32_fprintf
    #define abort win32_abort
    #define exit win32_exit
#endif

enum { CoordModeOrigin, CoordModePrevious };
enum { Convex };
enum { OK = 0, BadAlloc, BadGC, BadValue, BadFont, BadMatch, BadPixmap };

//////////////////////////////////////////////////////////////////////////////
// Bool

typedef BOOL Bool;
#define False FALSE
#define True TRUE

//////////////////////////////////////////////////////////////////////////////
// Drawable

typedef INT Drawable;
typedef INT Pixmap;
typedef INT Window;

//////////////////////////////////////////////////////////////////////////////
// Visual

#define Visual int

//////////////////////////////////////////////////////////////////////////////
// Colormap

typedef unsigned long Colormap;

//////////////////////////////////////////////////////////////////////////////
// XPoint, XSegment, XRectangle, XArc

typedef struct {
    short x, y;
} XPoint;

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
    #define Screen struct HWND__
#else
    #define Display void
    #define Screen void
#endif

typedef HGLRC GLXContext;

//////////////////////////////////////////////////////////////////////////////
// XWindowAttributes

typedef struct {
    int width;
    int height;
    int depth;
    Visual *visual;
    Colormap colormap;
    Screen *screen;
} XWindowAttributes;

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

typedef enum
{
    t_Bool, t_Int, t_Float, t_String
} argdatatype;

typedef struct
{
    void *data;
    char *name;
    char *text;
    char *def;
    argdatatype type;
} argtype;

typedef struct
{
    char *opt;
    char *desc;
} OptionStruct;

typedef struct
{
    int nopts;
    XrmOptionDescRec *opts;
    int nargs;
    argtype *args;
    OptionStruct *desc;
} ModeSpecOpt;

typedef struct
{
    char *ignore0;
    char *ignore1;
    char *ignore2;
    char *ignore3;
    char *ignore4;
    char *ignore5;
    char *ignore6;
    void *ignore7;
    int ignore8;
    int ignore9;
    int ignore10;
    int ignore11;
    int ignore12;
    float ignore13;
    char *ignore14;
    char *ignore15;
    int ignore16;
    void *ignore17;
} ModStruct;

//////////////////////////////////////////////////////////////////////////////
// GC

#define GCForeground 0
#define GCBackground 1

enum    // line_style
{
    LineSolid = PS_SOLID,
    LineOnOffDash = PS_DASH,
    LineDoubleDash = PS_DASHDOT
};

enum    // cap_style
{
    CapNotLast = PS_ENDCAP_FLAT,
    CapButt = PS_ENDCAP_SQUARE,
    CapRound = PS_ENDCAP_ROUND,
    CapProjecting
};

enum    // join_style
{
    JoinMiter = PS_JOIN_MITER,
    JoinRound = PS_JOIN_ROUND,
    JoinBevel = PS_JOIN_BEVEL
};

typedef unsigned long GC;

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
} XGCValues;

#define MAX_GC_BUFFER 32

XGCValues *XGetGCValues0(GC gc);

Status XGetGCValues(
    Display *dpy,
    GC gc,
    unsigned long valuemask,
    XGCValues *values_return);

GC XCreateGC(
     Display *dpy, 
     Drawable d,
     unsigned long valuemask, 
     XGCValues *values);

int XFreeGC(Display *dpy, GC gc);

HPEN XCreateWinPen(XGCValues *values);
HBRUSH XCreateWinBrush(XGCValues *values);
HFONT XCreateWinFont(XGCValues *values);

int XSetForeground(Display *dpy, GC gc, unsigned long foreground);

int XCopyArea(
     Display *dpy,
     Drawable src_drawable, Drawable dst_drawable, GC gc,
     int src_x, int src_y,
     unsigned int width, unsigned int height,
     int dst_x, int dst_y);

//////////////////////////////////////////////////////////////////////////////
// ModeInfo

typedef struct
{
    Display *dpy;
    Window window;
    GC gc;
    INT num_screen;
    INT screen_number;
    INT width;
    INT height;
    INT polygon_count;
    INT recursion_depth;
    Bool fps_p;
    Bool is_drawn;
    INT pause;
    INT count;
    INT cycles;
    INT size;
    XWindowAttributes xgwa;
} ModeInfo;

#define MI_DISPLAY(mi) (mi)->dpy
#define MI_WINDOW(mi) 0
#define MI_GC(mi) (mi)->gc
#define MI_NUM_SCREENS(mi) 1
#define MI_SCREEN(mi) 0
#define MI_WIDTH(mi) (mi)->width
#define MI_HEIGHT(mi) (mi)->height
#define MI_WIN_WIDTH(mi) (mi)->width
#define MI_WIN_HEIGHT(mi) (mi)->height
#define MI_NCOLORS(mi) (256 - 20)
#define MI_IS_DRAWN(mi) (mi)->is_drawn
#define MI_IS_FPS(mi) FALSE
#define MI_IS_MONO(mi) FALSE
#define MI_CLEARWINDOW(mi) ss_clear(MI_DISPLAY(mi))
#define MI_IS_WIREFRAME(mi) FALSE
#define MI_COUNT(MI) (mi)->count
#define MI_BATCHCOUNT(MI) MI_COUNT(MI)
#define MI_IS_ICONIC(mi) FALSE
#define MI_CYCLES(mi) (mi)->cycles
#define MI_SIZE(mi) (mi)->size
#ifdef NDEBUG
    #define MI_IS_DEBUG(mi) FALSE
    #define MI_IS_VERBOSE(mi) FALSE
#else
    #define MI_IS_DEBUG(mi) TRUE
    #define MI_IS_VERBOSE(mi) TRUE
#endif
#define MI_NAME(mi) progname
#define MI_DELAY(mi) DELAY
#define MI_VISUAL(mi) NULL
#define MI_COLORMAP(mi) 0
#define MI_WIN_BLACK_PIXEL(mi) 0
#define MI_PIXEL(mi, pixel) pixel
#define MI_BLACK_PIXEL(mi) 0
#define MI_WHITE_PIXEL(mi) 255
#define MI_NPIXELS(mi) (256 - 20)
#define MI_IS_FULLRANDOM(mi) TRUE

#define FreeAllGL(mi) /**/

//////////////////////////////////////////////////////////////////////////////

#define SINF(n)         ((float)sin((double)(n)))
#define COSF(n)         ((float)cos((double)(n)))
#define FABSF(n)        ((float)fabs((double)(n)))

#undef MAX
#undef MIN
#undef ABS
#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))
#define ABS(a) ((a)<0 ? -(a) : (a))

//////////////////////////////////////////////////////////////////////////////

#undef NUMCOLORS
#define NUMCOLORS 256

//////////////////////////////////////////////////////////////////////////////

#include "yarandom.h"

#define LRAND()         ((long) (random() & 0x7fffffff))
#define NRAND(n)        ((int) (LRAND() % (n)))
#define MAXRAND         (2147483648.0) /* unsigned 1<<31 as a float */
#define SRAND(n)        /* already seeded by screenhack.c */

//////////////////////////////////////////////////////////////////////////////

typedef enum COLOR_SCHEME {
    color_scheme_default, color_scheme_uniform, 
    color_scheme_smooth, color_scheme_bright
};

#ifdef WRITABLE_COLORS
# undef WRITABLE_COLORS
# define WRITABLE_COLORS 1
#else
# define WRITABLE_COLORS 0
#endif

//////////////////////////////////////////////////////////////////////////////

# define XSCREENSAVER_LINK(NAME) \
   struct xscreensaver_function_table *xscreensaver_function_table = &NAME;

#if defined(UNIFORM_COLORS)
# define XLOCKMORE_COLOR_SCHEME color_scheme_uniform
#elif defined(SMOOTH_COLORS)
# define XLOCKMORE_COLOR_SCHEME color_scheme_smooth
#elif defined(BRIGHT_COLORS)
# define XLOCKMORE_COLOR_SCHEME color_scheme_bright
#else
# define XLOCKMORE_COLOR_SCHEME color_scheme_default
#endif

typedef void (*HACK_INIT)(ModeInfo *);
typedef void (*HACK_DRAW)(ModeInfo *);
typedef void (*HACK_REFRESH)(ModeInfo *);
typedef void (*HACK_FREE)(ModeInfo *);

typedef void (*HACK_INIT2)(Display *display, Window window);
typedef void (*HACK_DRAW2)(Display *dpy, Window window, void *closure);
typedef void (*HACK_RESHAPE)(Display *dpy, Window window, void *closure, unsigned int w, unsigned int h);
typedef void (*HACK_FREE2)(Display *dpy, Window window, void *closure);

extern int hack_count;
extern int hack_count_enabled;
extern int hack_cycles;
extern int hack_cycles_enabled;
extern int hack_size;
extern int hack_size_enabled;
extern int hack_argcount;
extern argtype *hack_arginfo;

#ifdef COUNT
    #define HACK_COUNT \
        int hack_count = COUNT; \
        int hack_count_enabled = True
#else
    #define HACK_COUNT \
        int hack_count = 0; \
        int hack_count_enabled = False
#endif
#ifdef CYCLES
    #define HACK_CYCLES \
        int hack_cycles = CYCLES; \
        int hack_cycles_enabled = True
#else
    #define HACK_CYCLES \
        int hack_cycles = 0; \
        int hack_cycles_enabled = False
#endif
#ifdef SIZE_
    #define HACK_SIZE \
        int hack_size = SIZE_; \
        int hack_size_enabled = True
#else
    #define HACK_SIZE \
        int hack_size = 0; \
        int hack_size_enabled = False
#endif

#ifdef HACK
    #define XSCREENSAVER_MODULE_2(CLASS,NAME,PREFIX) \
        HACK_INIT2 hack_init = PREFIX ## _init; \
        HACK_DRAW2 hack_draw2 = PREFIX ## _draw; \
        HACK_RESHAPE hack_refresh = PREFIX ## _reshape; \
        HACK_FREE2 hack_free = PREFIX ## _free; \
        char *progname = CLASS;
#elif defined(NOARGS)
    #define XSCREENSAVER_MODULE_2(CLASS,NAME,PREFIX) \
        HACK_INIT hack_init = init_ ## PREFIX; \
        HACK_DRAW hack_draw = draw_ ## PREFIX; \
        HACK_REFRESH hack_refresh = refresh_ ## PREFIX; \
        HACK_FREE hack_free = release_ ## PREFIX; \
        DWORD hack_delay = DELAY; \
        char *progname = CLASS; \
        HACK_COUNT; \
        HACK_CYCLES; \
        HACK_SIZE; \
        argtype *hack_arginfo = NULL; \
        int hack_argcount = 0;
#else
    #define XSCREENSAVER_MODULE_2(CLASS,NAME,PREFIX) \
        HACK_INIT hack_init = init_ ## PREFIX; \
        HACK_DRAW hack_draw = draw_ ## PREFIX; \
        HACK_REFRESH hack_refresh = refresh_ ## PREFIX; \
        HACK_FREE hack_free = release_ ## PREFIX; \
        DWORD hack_delay = DELAY; \
        char *progname = CLASS; \
        HACK_COUNT; \
        HACK_CYCLES; \
        HACK_SIZE; \
        argtype *hack_arginfo = vars; \
        int hack_argcount = sizeof(vars) / sizeof(vars[0]);
#endif

#define XSCREENSAVER_MODULE(CLASS,PREFIX) \
    XSCREENSAVER_MODULE_2(CLASS,PREFIX,PREFIX)

extern HACK_INIT hack_init;
extern HACK_DRAW hack_draw;
extern HACK_REFRESH hack_refresh;
extern HACK_FREE hack_free;

extern HACK_INIT2 hack_init2;
extern HACK_DRAW2 hack_draw2;
extern HACK_RESHAPE hack_reshape;
extern HACK_FREE2 hack_free2;

extern DWORD hack_delay;
extern char *progname;

//////////////////////////////////////////////////////////////////////////////
// OpenGL-related

HGLRC *init_GL(ModeInfo *mi);
void glXMakeCurrent(Display *d, Window w, GLXContext context);
void glXSwapBuffers(Display *d, Window w);
void check_gl_error(const char *name);
void clear_gl_error(void);

//////////////////////////////////////////////////////////////////////////////
// trackball

#define trackball_state char*   // Not implemented yet

trackball_state *gltrackball_init(void);
void gltrackball_rotate(trackball_state *trackball);
void gltrackball_get_quaternion(char **ppch, float q[4]);
void gltrackball_start(trackball_state* trackball, int n1, int n2, int n3, int n4);
void gltrackball_track(trackball_state* trackball, int n1, int n2, int n3, int n4);

//////////////////////////////////////////////////////////////////////////////
// misc

void do_fps(ModeInfo *mi);
extern Bool has_writable_cells(Screen *s, Visual *v);
float current_device_rotation(void);
int ffs(int i);
int visual_cells(Screen *screen, Visual *visual);
int visual_depth(Screen *screen, Visual *visual);
void
load_texture_async(Screen *screen, Window window,
                   GLXContext glx_context,
                   int desired_width, int desired_height,
                   Bool mipmap_p,
                   GLuint texid,
                   void (*callback) (const char *filename,
                                     XRectangle *geometry,
                                     int image_width,
                                     int image_height,
                                     int texture_width,
                                     int texture_height,
                                     void *closure),
                   void *closure);

//////////////////////////////////////////////////////////////////////////////
// XImage

// format
enum { XYBitmap, XYPixmap, ZPixmap, RGBAPixmap };

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
    Display *dpy,
    Drawable d,
    unsigned int width,
    unsigned int height,
    unsigned int depth);
int XFreePixmap(Display *dpy, Pixmap pixmap);
int XSync(Display *dpy, Bool b);

//////////////////////////////////////////////////////////////////////////////
// X Window

Display *DisplayOfScreen(Screen *s);
#define DefaultScreenOfDisplay(dpy) dpy
#define BlackPixelOfScreen(s) 0
#define DefaultColormap(dpy,scr) 0

Status XGetWindowAttributes(
    Display *dpy,
    Window w,
    XWindowAttributes *attr);
int XSetLineAttributes(Display *dpy, GC gc,
    unsigned int line_width, int line_style,
    int cap_style, int join_style);

int XClearWindow(Display *dpy, Window w);

int XDrawPoint(Display *dpy, Drawable d, GC gc,
    int x, int y);
int XDrawPoints(Display *dpy, Drawable w, GC gc,
    XPoint *points, int npoints, int CoordMode);

int XDrawLine(Display *dpy, Drawable d, GC gc,
    int x1, int y1, int x2, int y2);
int XDrawLines(Display *dpy, Drawable d, GC gc,
    XPoint *points, int npoints, int mode);

int XDrawSegments(Display *dpy, Drawable d, GC gc,
    XSegment *segments, int nsegments);

int XDrawArc(Display *dpy, Drawable d, GC gc,
    int x, int y, unsigned int width, unsigned int height,
    int angle1, int angle2);

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

//////////////////////////////////////////////////////////////////////////////
// XColor

#define DoRed           (1<<0)
#define DoGreen         (1<<1)
#define DoBlue          (1<<2)

typedef struct
{
  unsigned long pixel;
  unsigned short red, green, blue;
  char flags;  /* DoRed, DoGreen, DoBlue */
  char pad;
} XColor;

Bool XAllocColor(Display *d, Colormap cmap, XColor *color);
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
int XFlush(Display *d);
int XQueryColor(Display *dpy, Colormap cmap, XColor *def);
int XParseColor(Display *d, Colormap cmap, const char *name, XColor *c);
unsigned long load_color(Display *dpy, Colormap cmap, const char *name);

//////////////////////////////////////////////////////////////////////////////
// screen saver

typedef struct SCREENSAVER
{
    HWND hwnd;
    HDC hdc;
    HGLRC hglrc;
    UINT x0, y0;
    UINT w, h;
    ModeInfo modeinfo;
    HBITMAP hbmScreenShot;
} SCREENSAVER;

extern SCREENSAVER ss;

BOOL InitPixelFormat(SCREENSAVER *ss);
VOID MakeCurrent(SCREENSAVER *ss);
XImage *GetScreenShotXImage(void);
void CreateTextureFromImage(XImage *ximage, GLuint texid);
void ss_term(void);
void ss_clear(Display *d);

//////////////////////////////////////////////////////////////////////////////

#endif  // ndef __XSCREENSAVER_WIN32_H__
