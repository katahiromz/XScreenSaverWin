/* font-ximage.c --- renders text to an XImage for use with OpenGL.
 * xscreensaver, Copyright (c) 2001-2013 Jamie Zawinski <jwz@jwz.org>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  No representations are made about the suitability of this
 * software for any purpose.  It is provided "as is" without express or 
 * implied warranty.
 */

#if 0
    #ifdef HAVE_CONFIG_H
    # include "config.h"
    #endif

    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>

    #ifdef HAVE_COCOA
    # include "jwxyz.h"
    #else  /* !HAVE_COCOA */
    # include <X11/Xlib.h>
    # include <X11/Xutil.h>
    # include <GL/gl.h> /* only for GLfloat */
    #endif /* !HAVE_COCOA */

    #ifdef HAVE_JWZGLES
    # include "jwzgles.h"
    #endif /* HAVE_JWZGLES */
#endif

#include "xws2win.h"
#include <GL/gl.h>

extern char *progname;

#include "font-ximage.h"

#undef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))

#undef countof
#define countof(x) (sizeof((x))/sizeof((*x)))

#if 0
static Bool
bigendian (void)
{
  union { int i; char c[sizeof(int)]; } u;
  u.i = 1;
  return !u.c[0];
}
#endif

/* return the next larger power of 2. */
static unsigned int
to_pow2 (unsigned int i)
{
  static const unsigned int pow2[] = { 
    1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024,
    2048, 4096, 8192, 16384, 32768, 65536 };
  unsigned int j;
  for (j = 0; j < countof(pow2); j++)
    if (pow2[j] >= i) return pow2[j];
  abort();  /* too big! */
  return 0;
}


/* Returns an XImage structure containing the string rendered in the font.
   This XImage will be 32 bits per pixel, 8 each per R, G, and B, with the
   extra byte set to 0xFF.

   Foregroune and background are GL-style color specifiers: 4 floats from
   0.0-1.0.
 */
XImage *
text_to_ximage (Screen *screen, Visual *visual,
                const char *font,
                const char *text_lines,
                GLfloat *texture_fg,
                GLfloat *texture_bg)
{
  Display *dpy = DisplayOfScreen (screen);
  int width, height;
  XFontStruct *f;
  Pixmap bitmap;
  int margin = 2, xoff, yoff;
  GC gc;
  XGCValues gcv;
  COLORREF foreground_rgb = RGB(
      texture_fg[0] * 255.0,
      texture_fg[1] * 255.0,
      texture_fg[2] * 255.0);
  COLORREF background_rgb = RGB(
      texture_bg[0] * 255.0,
      texture_bg[1] * 255.0,
      texture_bg[2] * 255.0);

  f = XLoadQueryFont(dpy, font);
  if (!f)
    {
      f = XLoadQueryFont(dpy, "System");
      if (f)
        fprintf (stderr, "%s: unable to load font \"%s\"; using \"System\".\n",
                 progname, font);
      else
        {
          fprintf (stderr, "%s: unable to load fonts \"%s\" or \"System\"!\n",
                   progname, font);
          exit (1);
        }
    }

    {
        SIZE siz;
        HDC hdc = CreateCompatibleDC(NULL);
        HGDIOBJ hFontOld = SelectObject(hdc, f->fid);
        GetTextExtentPoint32A(hdc, text_lines, lstrlenA(text_lines), &siz);
        SelectObject(hdc, hFontOld);
        DeleteDC(hdc);
        width = siz.cx + margin * 2;
        height = siz.cy + margin * 2;
    }

    /* GL texture sizes must be powers of two. */
    {
      int w2 = to_pow2(width);
      int h2 = to_pow2(height);
      xoff = (w2 - width)  / 2;
      yoff = (h2 - height) / 2;
      width  = w2;
      height = h2;
    }

    bitmap = XCreatePixmap(dpy, RootWindowOfScreen (screen), width, height, 1);

    gcv.font = f->fid;
    gc = XCreateGC (dpy, bitmap, (GCFont | GCForeground), &gcv);
    gc->foreground_rgb = background_rgb;
    XFillRectangle(dpy, bitmap, gc, 0, 0, width, height);

    {
        HDC hdc = CreateCompatibleDC(NULL);
        HGDIOBJ hFontOld = SelectObject(hdc, f->fid);
        HGDIOBJ hbmOld = SelectObject(hdc, bitmap->hbm);
        SetBkMode(hdc, OPAQUE);
        SetTextColor(hdc, foreground_rgb);
        SetBkColor(hdc, background_rgb);
        TextOut(hdc, margin + xoff, margin + yoff, text_lines, lstrlenA(text_lines));
        SelectObject(hdc, hbmOld);
        SelectObject(hdc, hFontOld);
        DeleteDC(hdc);
        GdiFlush();
    }

    XUnloadFont(dpy, f->fid);
    XFree((XPointer) f);
    XFreeGC(dpy, gc);

  /* Convert the server-side Pixmap to a client-side GL-ordered XImage.
   */
  {
    XImage *ximage1;

    ximage1 = XGetImage (dpy, bitmap, 0, 0, width, height, ~0L, RGBAPixmap_);
    XFreePixmap(dpy, bitmap);

    return ximage1;
  }
}
