/* xscreensaver, Copyright (c) 2008-2013 Jamie Zawinski <jwz@jwz.org>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  No representations are made about the suitability of this
 * software for any purpose.  It is provided "as is" without express or 
 * implied warranty.
 *
 * Draws repetitive patterns that should undo burned in LCD screens.
 * Concept shamelessly cloned from
 * http://toastycode.com/blog/2008/02/05/lcd-scrub/
 */

#include "screenhack.h"

#undef countof
#define countof(x) (sizeof((x))/sizeof((*x)))

char *background = "black";
char *foreground = "white";
int delay = 100000;
int spread = 8;
int cycles = 60;
Bool modeHW = True;
Bool modeHB = True;
Bool modeVW = True;
Bool modeVB = True;
Bool modeDW = True;
Bool modeDB = True;
Bool modeW = True;
Bool modeB = True;
Bool modeRGB = True;

static argtype vars[] = 
{
    {&background, "background", NULL, "black", t_String},
    {&foreground, "foreground", NULL, "white", t_String},
    {&delay, "delay", NULL, "100000", t_Int},
    {&spread, "spread", NULL, "8", t_Int},
    {&cycles, "cycles", NULL, "60", t_Int},
    {&modeHW, "modeHW", NULL, "True", t_Bool},
    {&modeHB, "modeHB", NULL, "True", t_Bool},
    {&modeVW, "modeVW", NULL, "True", t_Bool},
    {&modeVB, "modeVB", NULL, "True", t_Bool},
    {&modeDW, "modeDW", NULL, "True", t_Bool},
    {&modeDB, "modeDB", NULL, "True", t_Bool},
    {&modeW, "modeW", NULL, "True", t_Bool},
    {&modeB, "modeB", NULL, "True", t_Bool},
    {&modeRGB, "modeRGB", NULL, "True", t_Bool},
};

struct state {
  Display *dpy;
  Window window;
  XWindowAttributes xgwa;
  enum { HORIZ_W, HORIZ_B, 
         VERT_W, VERT_B, 
         DIAG_W, DIAG_B, 
         WHITE, BLACK,
         RGB,
         END } mode;
  unsigned int enabled_mask;
  int count;
  GC fg, bg, bg2;
  int color_tick;
  int delay;
  int spread;
  int cycles;
};


static void
pick_mode (struct state *st)
{
  st->count = 0;
  while (1)
    {
      if (++st->mode == END)
        st->mode = 0;
      if (st->enabled_mask & (1 << st->mode))
        break;
    }
}

static void *
lcdscrub_init (Display *dpy, Window window)
{
  struct state *st = (struct state *) calloc (1, sizeof(*st));
  XGCValues gcv;
  st->dpy = dpy;
  st->window = window;
  //st->delay  = get_integer_resource (st->dpy, "delay",  "Integer");
  //st->spread = get_integer_resource (st->dpy, "spread", "Integer");
  //st->cycles = get_integer_resource (st->dpy, "cycles", "Integer");
  st->delay  = delay;
  st->spread = spread;
  st->cycles = cycles;

  XGetWindowAttributes (st->dpy, st->window, &st->xgwa);
  gcv.foreground = BlackPixelOfScreen (st->xgwa.screen);
  gcv.background = WhitePixelOfScreen (st->xgwa.screen);
  st->bg  = XCreateGC (st->dpy, st->window, GCForeground, &gcv);
  st->bg2 = XCreateGC (st->dpy, st->window, GCForeground, &gcv);
  gcv.foreground = WhitePixelOfScreen (st->xgwa.screen);
  gcv.background = BlackPixelOfScreen (st->xgwa.screen);
  st->fg = XCreateGC (st->dpy, st->window, GCForeground, &gcv);

#ifdef HAVE_COCOA
  jwxyz_XSetAntiAliasing (st->dpy, st->fg,  False);
  jwxyz_XSetAntiAliasing (st->dpy, st->bg,  False);
  jwxyz_XSetAntiAliasing (st->dpy, st->bg2, False);
#endif

  st->enabled_mask = 0;
#if 1
	# define PREF(R,F) \
	   if (R) st->enabled_mask |= (1 << F)
#else
	# define PREF(R,F) \
	   if (get_boolean_resource (st->dpy, R, "Mode")) st->enabled_mask |= (1 << F)
#endif
  PREF("modeHW", HORIZ_W);
  PREF("modeHB", HORIZ_B);
  PREF("modeVW", VERT_W);
  PREF("modeVB", VERT_B);
  PREF("modeDW", DIAG_W);
  PREF("modeDB", DIAG_B);
  PREF("modeW",  WHITE);
  PREF("modeB",  BLACK);
  PREF("modeRGB", RGB);
# undef PREF
  if (! st->enabled_mask) 
    {
      fprintf (stderr, "%s: no modes enabled\n", progname);
      exit (1);
    }

  pick_mode (st);

  return st;
}

static unsigned long
lcdscrub_draw (Display *dpy, Window window, void *closure)
{
  struct state *st = (struct state *) closure;
  int count = st->count % st->spread;
  int i;
  GC fg = (st->mode & 1 ? st->fg : st->bg);
  GC bg = (st->mode & 1 ? st->bg : st->fg);

  switch (st->mode) {
  case HORIZ_W:
  case HORIZ_B:
    XFillRectangle (st->dpy, st->window, bg, 0, 0,
                    st->xgwa.width, st->xgwa.height);
    for (i = count; i < st->xgwa.height; i += st->spread)
      XDrawLine (st->dpy, st->window, fg, 0, i, st->xgwa.width, i);
    break;
  case VERT_W:
  case VERT_B:
    XFillRectangle (st->dpy, st->window, bg, 0, 0,
                    st->xgwa.width, st->xgwa.height);
    for (i = count; i < st->xgwa.width; i += st->spread)
      XDrawLine (st->dpy, st->window, fg, i, 0, i, st->xgwa.height);
    break;
  case DIAG_W:
  case DIAG_B:
    XFillRectangle (st->dpy, st->window, bg, 0, 0,
                    st->xgwa.width, st->xgwa.height);
    for (i = count; i < st->xgwa.width; i += st->spread)
      XDrawLine (st->dpy, st->window, fg, i, 0, 
                 i + st->xgwa.width, st->xgwa.width);
    for (i = -count; i < st->xgwa.height; i += st->spread)
      XDrawLine (st->dpy, st->window, fg, 0, i,
                 st->xgwa.height, i + st->xgwa.height);
    break;
  case RGB:
    {
      int scale = 10 * 8; /* 8 sec */
      static const unsigned short colors[][3] = {
        { 0xFFFF, 0x0000, 0x0000 },
        { 0x0000, 0xFFFF, 0x0000 },
        { 0x0000, 0x0000, 0xFFFF },
        { 0xFFFF, 0xFFFF, 0x0000 },
        { 0xFFFF, 0x0000, 0xFFFF },
        { 0x0000, 0xFFFF, 0xFFFF },
        { 0xFFFF, 0xFFFF, 0xFFFF },
        { 0x0000, 0x0000, 0x0000 },
      };
      static unsigned long last = 0;
      XColor xc;
      bg = st->bg2;
      xc.red   = colors[st->color_tick / scale][0];
      xc.green = colors[st->color_tick / scale][1];
      xc.blue  = colors[st->color_tick / scale][2];
      if (last) XFreeColors (st->dpy, st->xgwa.colormap, &last, 1, 0);
      XAllocColor (st->dpy, st->xgwa.colormap, &xc);
      last = xc.pixel;
      XSetForeground (st->dpy, bg, xc.pixel);
      st->color_tick = (st->color_tick + 1) % (countof(colors) * scale);
      /* fall through */
    }
  case WHITE:
  case BLACK:
    XFillRectangle (st->dpy, st->window, bg, 0, 0,
                    st->xgwa.width, st->xgwa.height);
    break;
  default: 
    abort(); 
    break;
  }

  st->count++;

  if (st->count > st->spread * st->cycles)
    pick_mode (st);

  return st->delay;
}

static void
lcdscrub_reshape (Display *dpy, Window window, void *closure, 
                 unsigned int w, unsigned int h)
{
}

#if 0
	static Bool
	lcdscrub_event (Display *dpy, Window window, void *closure, XEvent *event)
	{
	  return False;
	}
#endif

static void
lcdscrub_free (Display *dpy, Window window, void *closure)
{
  struct state *st = (struct state *) closure;
  XFreeGC (dpy, st->fg);
  XFreeGC (dpy, st->bg);
  XFreeGC (dpy, st->bg2);
  free (st);
}


static const char *lcdscrub_defaults [] = {
  ".background:		black",
  ".foreground:		white",
  "*delay:		100000",
  "*spread:		8",
  "*cycles:		60",
  "*modeHW:		True",
  "*modeHB:		True",
  "*modeVW:		True",
  "*modeVB:		True",
  "*modeDW:		True",
  "*modeDB:		True",
  "*modeW:		True",
  "*modeB:		True",
  "*modeRGB:		True",
  0
};

static XrmOptionDescRec lcdscrub_options [] = {
  { "-delay",		".delay",	XrmoptionSepArg, 0 },
  { "-spread",		".spread",	XrmoptionSepArg, 0 },
  { "-cycles",		".cycles",	XrmoptionSepArg, 0 },
  { "-no-hw",		".modeHW",	XrmoptionNoArg, "False" },
  { "-no-hb",		".modeHB",	XrmoptionNoArg, "False" },
  { "-no-vw",		".modeVW",	XrmoptionNoArg, "False" },
  { "-no-vb",		".modeVB",	XrmoptionNoArg, "False" },
  { "-no-dw",		".modeDW",	XrmoptionNoArg, "False" },
  { "-no-db",		".modeDB",	XrmoptionNoArg, "False" },
  { "-no-w",		".modeW",	XrmoptionNoArg, "False" },
  { "-no-b",		".modeB",	XrmoptionNoArg, "False" },
  { "-no-rgb",		".modeRGB",	XrmoptionNoArg, "False" },
  { 0, 0, 0, 0 }
};


XSCREENSAVER_MODULE ("LCDscrub", lcdscrub)
