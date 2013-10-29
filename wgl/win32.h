#define ENTRYPOINT static

typedef BOOL Bool;
#define False FALSE
#define True TRUE

#define DoRed			(1<<0)
#define DoGreen			(1<<1)
#define DoBlue			(1<<2)

typedef struct win32_XColor {
  unsigned long pixel;
  unsigned short red, green, blue;
  char flags;  /* DoRed, DoGreen, DoBlue */
  char pad;
} XColor;

#define Visual void
#define Screen void

typedef struct ModeInfo
{
    HDC hdc;
    HWND hwnd;
    INT num_screen;
    INT screen_number;
    INT width;
    INT height;
    INT polygon_count;
	INT recursion_depth;
    Bool fps_p;
    Bool is_drawn;
	int pause;
} ModeInfo;

#define MI_DISPLAY(mi) (mi)->hdc
#define MI_WINDOW(mi) (mi)->hwnd
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
#define MI_CYCLES(mi) 0
#define MI_SIZE(mi) 0

#define FreeAllGL(mi) /**/

#define SINF(n)         ((float)sin((double)(n)))
#define COSF(n)         ((float)cos((double)(n)))
#define FABSF(n)        ((float)fabs((double)(n)))

#undef MAX
#undef MIN
#undef ABS
#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))
#define ABS(a) ((a)<0 ? -(a) : (a))

#undef NUMCOLORS
#define NUMCOLORS 256

#include "yarandom.h"

#define LRAND()         ((long) (random() & 0x7fffffff))
#define NRAND(n)        ((int) (LRAND() % (n)))
#define MAXRAND         (2147483648.0) /* unsigned 1<<31 as a float */
#define SRAND(n)        /* already seeded by screenhack.c */

typedef struct SCREENSAVER
{
    HWND hwnd;
    HDC hdc;
    HGLRC hglrc;
    UINT x0, y0;
    UINT w, h;
    ModeInfo modeinfo;
} SCREENSAVER;

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
	DWORD hack_delay = DELAY;

#define XSCREENSAVER_MODULE(CLASS,PREFIX) \
	XSCREENSAVER_MODULE_2(CLASS,PREFIX,PREFIX)

extern HACK_INIT hack_init;
extern HACK_DRAW hack_draw;
extern HACK_REFRESH hack_refresh;
extern HACK_FREE hack_free;
extern DWORD hack_delay;

#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif
#ifndef M_PI_2
#define M_PI_2		1.57079632679489661923
#endif

HGLRC init_GL(ModeInfo *mi);
void do_fps(ModeInfo *mi);
