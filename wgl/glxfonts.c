/* glxfonts, Copyright (c) 2001-2012 Jamie Zawinski <jwz@jwz.org>
 * Loads X11 fonts for use with OpenGL.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  No representations are made about the suitability of this
 * software for any purpose.  It is provided "as is" without express or 
 * implied warranty.
 *
 * Draws 2D text over the GL scene, e.g., the FPS displays.
 *
 * There are two implementations here: one using glBitmap (the OpenGL 1.1 way)
 * and one using textures via texfont.c (since OpenGLES doesn't have glBitmap).
 */

#if 0
	#ifdef HAVE_CONFIG_H
	# include "config.h"
	#endif
#endif

#include "xlockmore.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

//# include <GL/glx.h>
# include <GL/glu.h>

#if 0
	#ifdef HAVE_COCOA
	# include "jwxyz.h"
	/*# include <AGL/agl.h>*/
	#else
	# include <GL/glx.h>
	# include <GL/glu.h>
	#endif

	#ifdef HAVE_JWZGLES
	# include "jwzgles.h"
	#endif /* HAVE_JWZGLES */

	#include "resources.h"
#endif

#include "glxfonts.h"
//#include "fps.h"

#ifndef HAVE_GLBITMAP
# include "texfont.h"
#endif /* HAVE_GLBITMAP */


/* These are in xlock-gl.c */
extern void clear_gl_error (void);
extern void check_gl_error (const char *type);

/* screenhack.h */
extern char *progname;


#undef DEBUG  /* Defining this causes check_gl_error() to be called inside
                 time-critical sections, which could slow things down (since
                 it might result in a round-trip, and stall of the pipeline.)
               */


#undef countof
#define countof(x) (sizeof((x))/sizeof((*x)))


/* Width (and optionally height) of the string in pixels.
 */
int
string_width (XFontStruct *f, const char *c, int *height_ret)
{
    SIZE siz;
    HGDIOBJ hFontOld;
    HDC hdc = CreateCompatibleDC(NULL);
    hFontOld = SelectObject(hdc, f->fid);
    GetTextExtentPoint32A(hdc, c, lstrlenA(c), &siz);
    SelectObject(hdc, hFontOld);
    DeleteDC(hdc);

    if (height_ret)
        *height_ret = siz.cy;
    return siz.cx;
}


#ifdef HAVE_GLBITMAP

/* Mostly lifted from the Mesa implementation of glXUseXFont(), since
   Mac OS 10.6 no longer supports aglUseFont() which was their analog
   of that.  This code could be in libjwxyz instead, but we might as
   well use the same text-drawing code on both X11 and Cocoa.
 */
static void
fill_bitmap (Display *dpy, Window win, GC gc,
	     unsigned int width, unsigned int height,
	     int x0, int y0, char c, GLubyte *bitmap)
{
  XImage *image;
  int x, y;
  Pixmap pixmap;

  pixmap = XCreatePixmap (dpy, win, 8*width, height, 1);
  XSetForeground(dpy, gc, 0);
  XFillRectangle (dpy, pixmap, gc, 0, 0, 8*width, height);
  XSetForeground(dpy, gc, 255);
  XDrawString (dpy, pixmap, gc, x0, y0, &c, 1);

  image = XGetImage (dpy, pixmap, 0, 0, 8*width, height, 1, XYPixmap);

  /* Fill the bitmap (X11 and OpenGL are upside down wrt each other).  */
  for (y = 0; y < height; y++)
    for (x = 0; x < 8*width; x++)
      if (XGetPixel (image, x, y))
	bitmap[width*(height - y - 1) + x/8] |= (1 << (7 - (x % 8)));
  
  XFreePixmap (dpy, pixmap);
  XDestroyImage (image);
}


#if 0
static void
dump_bitmap (unsigned int width, unsigned int height, GLubyte *bitmap)
{
  int x, y;

  printf ("    ");
  for (x = 0; x < 8*width; x++)
    printf ("%o", 7 - (x % 8));
  putchar ('\n');
  for (y = 0; y < height; y++)
    {
      printf ("%3o:", y);
      for (x = 0; x < 8*width; x++)
        putchar ((bitmap[width*(height - y - 1) + x/8] & (1 << (7 - (x % 8))))
		 ? '#' : '.');
      printf ("   ");
      for (x = 0; x < width; x++)
	printf ("0x%02x, ", bitmap[width*(height - y - 1) + x]);
      putchar ('\n');
    }
}
#endif


void
xscreensaver_glXUseXFont (Display *dpy, Font font, 
                          int first, int count, int listbase)
{
    HGDIOBJ hFontOld;
    hFontOld = SelectObject(dpy, font);
    wglUseFontBitmaps(dpy, first, count, listbase);
    SelectObject(dpy, hFontOld);
}


/* Loads the font named by the X resource "res".
   Returns an XFontStruct.
   Also converts the font to a set of GL lists and returns the first list.
*/
void
load_font (Display *dpy, char *res, XFontStruct **font_ret, GLuint *dlist_ret)
{
  XFontStruct *f;

  //const char *font = get_string_resource (dpy, res, "Font");
  //const char *def1 = "-*-helvetica-medium-r-normal-*-180-*";
  //const char *def2 = "fixed";
  const char *font = res;
  const char *def1 = "Arial 90";
  const char *def2 = "System";
  Font id;
  int first, last;

  if (!res || !*res) abort();
  if (!font_ret && !dlist_ret) abort();

  if (!font) font = def1;

  f = XLoadQueryFont(dpy, font);
  if (!f && !!strcmp (font, def1))
    {
      fprintf (stderr, "%s: unable to load font \"%s\", using \"%s\"\n",
               progname, font, def1);
      font = def1;
      f = XLoadQueryFont(dpy, font);
    }

  if (!f && !!strcmp (font, def2))
    {
      fprintf (stderr, "%s: unable to load font \"%s\", using \"%s\"\n",
               progname, font, def2);
      font = def2;
      f = XLoadQueryFont(dpy, font);
    }

  if (!f)
    {
      fprintf (stderr, "%s: unable to load fallback font \"%s\" either!\n",
               progname, font);
      exit (1);
    }

  id = f->fid;
  first = f->min_char_or_byte2;
  last = f->max_char_or_byte2;
  

  if (dlist_ret)
    {
      clear_gl_error ();
      *dlist_ret = glGenLists ((GLuint) last+1);
      check_gl_error ("glGenLists");
      xscreensaver_glXUseXFont (dpy, id, first, last-first+1,
                                *dlist_ret + first);
    }

  if (font_ret)
    *font_ret = f;
}

#endif /* HAVE_GLBITMAP */


/* Draws the string on the window at the given pixel position.
   Newlines and tab stops are honored.
   Any text inside [] will be rendered as a subscript.
   Assumes the font has been loaded as with load_font().
 */
void
print_gl_string (Display *dpy,
                 XFontStruct *font, GLuint font_dlist,
                 int window_width, int window_height,
                 GLfloat x, GLfloat y,
                 const char *string,
                 Bool clear_background_p)
{

  /* If window_width was specified, we're drawing ortho in pixel coordinates.
     Otherwise, we're just dropping the text at the current position in the
     scene, billboarded. */
  Bool in_scene_p = (window_width == 0);

  GLfloat line_height = font->ascent + font->descent;
  int cw = string_width (font, "m", 0);

  GLfloat sub_shift = (line_height * 0.3);
  int tabs = cw * 7;
  int lines = 0;
  const char *c;
  GLfloat color[4];

  /* Sadly, this causes a stall of the graphics pipeline (as would the
     equivalent calls to glGet*.)  But there's no way around this, short
     of having each caller set up the specific display matrix we need
     here, which would kind of defeat the purpose of centralizing this
     code in one file.
   */
  glPushAttrib (GL_TRANSFORM_BIT |  /* for matrix contents */
                GL_ENABLE_BIT |     /* for various glDisable calls */
                GL_CURRENT_BIT |    /* for glColor3f() */
                GL_LIST_BIT);       /* for glListBase() */
#  ifdef DEBUG
    check_gl_error ("glPushAttrib");
#  endif

    glGetFloatv (GL_CURRENT_COLOR, color);

  for (c = string; *c; c++)
    if (*c == '\n') lines++;

  y -= line_height;

  {

    /* disable lighting and texturing when drawing bitmaps!
       (glPopAttrib() restores these.)
     */
    glDisable (GL_TEXTURE_2D);
    glDisable (GL_LIGHTING);

      if (!in_scene_p)
        glDisable (GL_DEPTH_TEST);
    glDisable (GL_CULL_FACE);
    glDisable (GL_FOG);

    /* glPopAttrib() does not restore matrix changes, so we must
       push/pop the matrix stacks to be non-intrusive there.
     */
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    {
# ifdef DEBUG
      check_gl_error ("glPushMatrix");
# endif
      if (!in_scene_p)
        glLoadIdentity();

      /* Each matrix mode has its own stack, so we need to push/pop
         them separately. */
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      {
        unsigned int i;
        int x2 = x;
        Bool sub_p = False;

# ifdef DEBUG
        check_gl_error ("glPushMatrix");
# endif

        if (!in_scene_p)
          {
            double rot = current_device_rotation();

            glLoadIdentity();
            glOrtho (0, window_width, 0, window_height, -1, 1);

            if (rot > 135 || rot < -135)
              {
                glTranslatef (window_width, window_height, 0);
                glRotatef (180, 0, 0, 1);
              }
            else if (rot > 45)
              {
                glTranslatef (window_width, 0, 0);
                glRotatef (90, 0, 0, 1);
                y -= (window_height - window_width);
                if (y < line_height * lines + 10)
                  y = line_height * lines + 10;
              }
            else if (rot < -45)
              {
                glTranslatef(0, window_height, 0);
                glRotatef (-90, 0, 0, 1);
                y -= (window_height - window_width);
                if (y < line_height * lines + 10)
                  y = line_height * lines + 10;
              }
          }
# ifdef DEBUG
        check_gl_error ("glOrtho");
# endif

        /* Let's always dropshadow the FPS and Title text. */
        if (! in_scene_p)
          clear_background_p = True;

        /* draw the text */

        for (i = 0; i < strlen(string); i++)
          {
            unsigned char c = (unsigned char) string[i];
            if (c == '\n')
              {
                y -= line_height;
                x2 = x;
              }
            else if (c == '\t')
              {
                x2 -= x;
                x2 = ((x2 + tabs) / tabs) * tabs;  /* tab to tab stop */
                x2 += x;
              }
            else if (c == '[' && (isdigit (string[i+1])))
              {
                sub_p = True;
                y -= sub_shift;
              }
            else if (c == ']' && sub_p)
              {
                sub_p = False;
                y += sub_shift;
              }
            else
              {
                /* outline font in black */
                const XPoint offsets[] = {{ -1, -1 },
                                          { -1,  1 },
                                          {  1,  1 },
                                          {  1, -1 },
                                          {  0,  0 }};
                int j;

                glColor3f (0, 0, 0);
                for (j = 0; j < countof(offsets); j++)
                  {
                    if (! clear_background_p)
                      j = countof(offsets)-1;
                    if (offsets[j].x == 0)
                      glColor4fv (color);

                    glRasterPos2f (x2 + offsets[j].x, y + offsets[j].y);
                    glCallList (font_dlist + (int)(c));
                  }
                x2 += (font->per_char
                       ? font->per_char[c - font->min_char_or_byte2].width
                       : font->min_bounds.width);
              }
          }
# ifdef DEBUG
        check_gl_error ("print_gl_string");
# endif
      }
      glPopMatrix();
    }
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
  }
  glPopAttrib();
#  ifdef DEBUG
  check_gl_error ("glPopAttrib");
#  endif

  glMatrixMode(GL_MODELVIEW);
}
