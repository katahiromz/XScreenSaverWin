#ifndef __XLOCKMORE_H__
#define __XLOCKMORE_H__

#ifndef __XWS2WIN_H__
    #include "xws2win.h"
#endif
#include <scrnsave.h>

#define MAX_COLORCELLS 300
#define GETTIMEOFDAY_TWO_ARGS 1

#define inline  /*empty*/

//////////////////////////////////////////////////////////////////////////////

#define ENTRYPOINT static

#ifndef NDEBUG
    int __cdecl xlockmore_printf(const char *fmt, ...);
    int __cdecl xlockmore_fprintf(FILE *fp, const char *fmt, ...);

    #define printf xlockmore_printf
    #define fprintf xlockmore_fprintf
#endif

//////////////////////////////////////////////////////////////////////////////

typedef struct xlockmore_timeval
{
    DWORD tv_sec;
    DWORD tv_usec;
} xlockmore_timeval;

typedef struct xlockmore_timezone
{
	char dummy1, dummy2;
} xlockmore_timezone;

#define timeval xlockmore_timeval
#define timezone xlockmore_timezone

#define HAVE_GETTIMEOFDAY 1
void gettimeofday(timeval *t, timezone *tz);

//////////////////////////////////////////////////////////////////////////////

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
// ModeInfo

typedef struct
{
    Display *dpy;
    Window window;
    GC gc;
    int num_screen;
    int screen_number;
    int width;
    int height;
    int polygon_count;
    int recursion_depth;
    Bool fps_p;
    Bool is_drawn;
    int pause;
    int count;
    int cycles;
    int size;
    XWindowAttributes xgwa;
    int npixels;
    XColor *colors;
    unsigned long *pixels;
    Bool writable_p;
    unsigned long white_pixel;
    unsigned long black_pixel;
    Bool threed;
    float threed_delta;
    unsigned long threed_right_color;
    unsigned long threed_left_color;
    unsigned long threed_both_color;
    unsigned long threed_none_color;
    Bool install_p;
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
#define MI_NCOLORS(mi) (mi)->npixels
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
#define MI_PIXEL(mi,n) ((mi)->pixels[n])
#define MI_NPIXELS(mi) (mi)->npixels
#define MI_IS_FULLRANDOM(mi) True
#define MI_WHITE_PIXEL(mi) (mi)->white_pixel
#define MI_BLACK_PIXEL(mi) (mi)->black_pixel
#define MI_WIN_WHITE_PIXEL(mi) (mi)->white_pixel
#define MI_WIN_BLACK_PIXEL(mi) (mi)->black_pixel
#define MI_PAUSE(mi) hack_delay
#define MI_WIN_DEPTH(mi) 32
#define MI_WIN_IS_USE3D(mi) ((mi)->threed)
#define MI_LEFT_COLOR(mi)   ((mi)->threed_left_color)
#define MI_RIGHT_COLOR(mi)  ((mi)->threed_right_color)
#define MI_BOTH_COLOR(mi)   ((mi)->threed_both_color)
#define MI_NONE_COLOR(mi)   ((mi)->threed_none_color)
#define MI_DELTA3D(mi)      ((mi)->threed_delta)
#define MI_WIN_IS_INSTALL(mi) ((mi)->install_p)
#define MI_IS_INSTALL(mi)  MI_WIN_IS_INSTALL(mi)
#define MI_DEPTH(mi)  32

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

extern char *progname;
extern DWORD hack_delay;
extern int hack_count;
extern int hack_count_enabled;
extern int hack_cycles;
extern int hack_cycles_enabled;
extern int hack_size;
extern int hack_size_enabled;
extern int hack_ncolors;
extern int hack_ncolors_enabled;
extern int hack_color_scheme;

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
#ifdef NCOLORS
    #define HACK_NCOLORS \
        int hack_ncolors = NCOLORS; \
        int hack_ncolors_enabled = True
#else
    #define HACK_NCOLORS \
        int hack_ncolors = 0; \
        int hack_ncolors_enabled = False
#endif

#ifdef NOARGS
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
        HACK_NCOLORS; \
        argtype *hack_arginfo = NULL; \
        int hack_argcount = 0; \
        int hack_color_scheme = XLOCKMORE_COLOR_SCHEME;
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
        HACK_NCOLORS; \
        argtype *hack_arginfo = vars; \
        int hack_argcount = sizeof(vars) / sizeof(vars[0]); \
        int hack_color_scheme = XLOCKMORE_COLOR_SCHEME;
#endif

#define XSCREENSAVER_MODULE(CLASS,PREFIX) \
    XSCREENSAVER_MODULE_2(CLASS,PREFIX,PREFIX)

extern HACK_INIT hack_init;
extern HACK_DRAW hack_draw;
extern HACK_REFRESH hack_refresh;
extern HACK_FREE hack_free;

//////////////////////////////////////////////////////////////////////////////
// OpenGL-related

#include <GL/gl.h>
#include <GL/glu.h>

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

void do_fps(ModeInfo *mi);
extern Bool has_writable_cells(Screen *s, Visual *v);
float current_device_rotation(void);
int ffs(int i);
int visual_cells(Screen *screen, Visual *visual);
int visual_depth(Screen *screen, Visual *visual);

//////////////////////////////////////////////////////////////////////////////

#include "grabscreen.h"

//////////////////////////////////////////////////////////////////////////////
// screen saver

typedef struct SCREENSAVER
{
    HWND        hwnd;
    HDC         hdc;
    HWND        hwndPrimary;
    HGLRC       hglrc;
    INT         x, y;
    UINT        width, height;
    ModeInfo    modeinfo;
    HBITMAP     hbmScreenShot;
    INT         cMonitors;
    BOOL        primary_only;
} SCREENSAVER;

extern SCREENSAVER ss;

BOOL InitPixelFormat(SCREENSAVER *ss);
VOID MakeCurrent(SCREENSAVER *ss);
XImage *GetScreenShotXImage(void);
void CreateTextureFromImage(XImage *ximage, GLuint texid);
BOOL ss_init(HWND hwnd);
void ss_term(void);
void ss_clear(Display *d);

HBITMAP GetScreenShotBM(VOID);

//////////////////////////////////////////////////////////////////////////////

#include "colors.h"

//////////////////////////////////////////////////////////////////////////////

#endif  // __XLOCKMORE_H__
