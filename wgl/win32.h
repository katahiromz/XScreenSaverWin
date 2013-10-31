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
#define inline
#define ENTRYPOINT static
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#define Xmalloc malloc
#define Xcalloc calloc
#define Xrealloc realloc
#define Xfree free

enum { XYBitmap, XYPixmap, ZPixmap };
enum { MSBFirst, LSBFirst };

typedef DWORD CARD32;

//#define GL_UNSIGNED_INT_8_8_8_8_REV		0x8367

//////////////////////////////////////////////////////////////////////////////

typedef BOOL Bool;
#define False FALSE
#define True TRUE

//////////////////////////////////////////////////////////////////////////////

#define DoRed           (1<<0)
#define DoGreen         (1<<1)
#define DoBlue          (1<<2)

struct win32_XColor 
{
  unsigned long pixel;
  unsigned short red, green, blue;
  char flags;  /* DoRed, DoGreen, DoBlue */
  char pad;
};
typedef struct win32_XColor XColor;

//////////////////////////////////////////////////////////////////////////////

typedef unsigned long win32_Colormap;
typedef win32_Colormap Colormap;

//////////////////////////////////////////////////////////////////////////////

#define Visual void
#define trackball_state char*	// Not implemented yet

typedef int Status;

#ifdef STRICT
	#define Display struct HDC__
	#define Screen struct HWND__
#else
	#define Display void
	#define Screen void
#endif
typedef HWND  Window;
typedef HGLRC GLXContext;

//////////////////////////////////////////////////////////////////////////////

struct win32_ModeInfo
{
    Display *dpy;
    Window window;
    INT num_screen;
    INT screen_number;
    INT width;
    INT height;
    INT polygon_count;
    INT recursion_depth;
    Bool fps_p;
    Bool is_drawn;
    int pause;
};
typedef struct win32_ModeInfo ModeInfo;

#define MI_DISPLAY(mi) (mi)->dpy
#define MI_WINDOW(mi) (mi)->window
#define MI_NUM_SCREENS(mi) 1
#define MI_SCREEN(mi) 0
#define MI_WIDTH(mi) (mi)->width
#define MI_HEIGHT(mi) (mi)->height
#define MI_IS_DRAWN(mi) (mi)->is_drawn
#define MI_IS_FPS(mi) FALSE
#define MI_IS_MONO(mi) FALSE
#define MI_CLEARWINDOW(mi) glClear(GL_COLOR_BUFFER_BIT)
#define MI_IS_WIREFRAME(mi) FALSE
#define MI_COUNT(MI) GdiGetBatchLimit()
#define MI_BATCHCOUNT(MI) GdiGetBatchLimit()
#define MI_IS_ICONIC(mi) FALSE
#define MI_CYCLES(mi) 10
#define MI_SIZE(mi) 100
#ifdef NDEBUG
#define MI_IS_DEBUG(mi) FALSE
#else
#define MI_IS_DEBUG(mi) TRUE
#endif
#define MI_NAME(mi) progname
#define MI_DELAY(mi) DELAY

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

typedef void (*HACK_INIT) (ModeInfo *);
typedef void (*HACK_DRAW) (ModeInfo *);
typedef void (*HACK_REFRESH) (ModeInfo *);
typedef void (*HACK_FREE) (ModeInfo *);

#define XSCREENSAVER_MODULE_2(CLASS,NAME,PREFIX) \
    HACK_INIT hack_init = init_ ## PREFIX; \
    HACK_DRAW hack_draw = draw_ ## PREFIX; \
    HACK_REFRESH hack_refresh = refresh_ ## PREFIX; \
    HACK_FREE hack_free = release_ ## PREFIX; \
    DWORD hack_delay = DELAY; \
    char *progname = CLASS;

#define XSCREENSAVER_MODULE(CLASS,PREFIX) \
    XSCREENSAVER_MODULE_2(CLASS,PREFIX,PREFIX)

extern HACK_INIT hack_init;
extern HACK_DRAW hack_draw;
extern HACK_REFRESH hack_refresh;
extern HACK_FREE hack_free;
extern DWORD hack_delay;
extern char *progname;

//////////////////////////////////////////////////////////////////////////////

HGLRC *init_GL(ModeInfo *mi);
void do_fps(ModeInfo *mi);

void glXMakeCurrent(Display *d, Window w, GLXContext context);
void glXSwapBuffers(Display *d, Window w);
void check_gl_error(const char *name);
void clear_gl_error(void);
extern Bool has_writable_cells (Screen *s, Visual *v);

trackball_state *gltrackball_init(void);
void gltrackball_rotate(trackball_state *trackball);
void gltrackball_get_quaternion(char **ppch, float q[4]);

float current_device_rotation(void);

int ffs(int i);

//////////////////////////////////////////////////////////////////////////////

typedef char *XPointer;

typedef struct win32_XImage {
    int width, height;		/* size of image */
    int xoffset;		/* number of pixels offset in X direction */
    int format;			/* XYBitmap, XYPixmap, ZPixmap */
    char *data;			/* pointer to image data */
    int byte_order;		/* data byte order, LSBFirst, MSBFirst */
    int bitmap_unit;		/* quant. of scanline 8, 16, 32 */
    int bitmap_bit_order;	/* LSBFirst, MSBFirst */
    int bitmap_pad;		/* 8, 16, 32 either XY or ZPixmap */
    int depth;			/* depth of image */
    int bytes_per_line;		/* accelarator to next line */
    int bits_per_pixel;		/* bits per pixel (ZPixmap) */
    unsigned long red_mask;	/* bits in z arrangment */
    unsigned long green_mask;
    unsigned long blue_mask;
    XPointer obdata;		/* hook for the object routines to hang on */
    struct funcs {		/* image manipulation routines */
	struct win32_XImage *(*create_image)(
		Display* /* display */,
		Visual*		/* visual */,
		unsigned int	/* depth */,
		int		/* format */,
		int		/* offset */,
		char*		/* data */,
		unsigned int	/* width */,
		unsigned int	/* height */,
		int		/* bitmap_pad */,
		int		/* bytes_per_line */);
	int (*destroy_image)        (struct win32_XImage *);
	unsigned long (*get_pixel)  (struct win32_XImage *, int, int);
	int (*put_pixel)            (struct win32_XImage *, int, int, unsigned long);
	struct win32_XImage *(*sub_image)(struct win32_XImage *, int, int, unsigned int, unsigned int);
	int (*add_pixel)            (struct win32_XImage *, long);
	} f;
} XImage;

//////////////////////////////////////////////////////////////////////////////

typedef struct _XExtData {
	int number;		/* number returned by XRegisterExtension */
	struct _XExtData *next;	/* next item on list of data for structure */
	int (*free_private)(	/* called to free private storage */
	struct _XExtData *extension
	);
	XPointer private_data;	/* data private to this extension. */
} XExtData;

//////////////////////////////////////////////////////////////////////////////

typedef struct {
	XExtData *ext_data;	/* hook for extension to hang data */
	int depth;		/* depth of this image format */
	int bits_per_pixel;	/* bits/pixel at this depth */
	int scanline_pad;	/* scanline must padded to this multiple */
} ScreenFormat;

//////////////////////////////////////////////////////////////////////////////

Display *DisplayOfScreen(Screen *s);
#define DefaultScreenOfDisplay(dpy) dpy
#define BlackPixelOfScreen(s) 0
#define DefaultColormap(dpy, scr) 0

//////////////////////////////////////////////////////////////////////////////

extern Status XAllocColor(
    Display*		/* display */,
    Colormap		/* colormap */,
    XColor*		/* screen_in_out */
);
extern Status XAllocColorCells(
    Display*		/* display */,
    Colormap		/* colormap */,
    Bool	        /* contig */,
    unsigned long*	/* plane_masks_return */,
    unsigned int	/* nplanes */,
    unsigned long*	/* pixels_return */,
    unsigned int 	/* npixels */
);
extern int XFreeColors(
    Display*		/* display */,
    Colormap		/* colormap */,
    unsigned long*	/* pixels */,
    int			/* npixels */,
    unsigned long	/* planes */
);
extern int XStoreColors(
    Display*		/* display */,
    Colormap		/* colormap */,
    XColor*		/* color */,
    int			/* ncolors */
);
extern int XFlush(
    Display*		/* display */
);
extern XImage *XCreateImage(
    Display*		/* display */,
    Visual*		/* visual */,
    unsigned int	/* depth */,
    int			/* format */,
    int			/* offset */,
    char*		/* data */,
    unsigned int	/* width */,
    unsigned int	/* height */,
    int			/* bitmap_pad */,
    int			/* bytes_per_line */
);
#define XDestroyImage(ximage) \
	((*((ximage)->f.destroy_image))((ximage)))
#define XGetPixel(ximage, x, y) \
	((*((ximage)->f.get_pixel))((ximage), (x), (y)))
#define XPutPixel(ximage, x, y, pixel) \
	((*((ximage)->f.put_pixel))((ximage), (x), (y), (pixel)))
#define XSubImage(ximage, x, y, width, height)  \
	((*((ximage)->f.sub_image))((ximage), (x), (y), (width), (height)))
#define XAddPixel(ximage, value) \
	((*((ximage)->f.add_pixel))((ximage), (value)))

//////////////////////////////////////////////////////////////////////////////

typedef struct SCREENSAVER
{
    HWND hwnd;
    HDC hdc;
    HGLRC hglrc;
    UINT x0, y0;
    UINT w, h;
    ModeInfo modeinfo;
} SCREENSAVER;

//////////////////////////////////////////////////////////////////////////////

#endif	// ndef __XSCREENSAVER_WIN32_H__