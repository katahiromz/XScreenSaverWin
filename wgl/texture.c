#include "xlockmore.h"
#include <GL/gl.h>

extern Bool debug_p;

/* return the next larger power of 2. */
static int
to_pow2 (int i)
{
  static const unsigned int pow2[] = { 
    1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 
    2048, 4096, 8192, 16384, 32768, 65536 };
  int j;
  for (j = 0; j < sizeof(pow2)/sizeof(*pow2); j++)
    if (pow2[j] >= i) return pow2[j];
  abort();  /* too big! */
  return 0;
}

static void
halve_image (XImage *ximage, XRectangle *geom)
{
  int w2 = ximage->width/2;
  int h2 = ximage->height/2;
  int x, y;
  XImage *ximage2;

  if (w2 <= 32 || h2 <= 32)   /* let's not go crazy here, man. */
    return;

  if (debug_p)
    fprintf (stderr, "%s: shrinking image %dx%d -> %dx%d\n",
             progname, ximage->width, ximage->height, w2, h2);

  ximage2 = (XImage *) calloc (1, sizeof (*ximage2));
  *ximage2 = *ximage;
  ximage2->width = w2;
  ximage2->height = h2;
  ximage2->bytes_per_line = 0;
  ximage2->data = 0;
  XInitImage (ximage2);

  ximage2->data = (char *) calloc (h2, ximage2->bytes_per_line);
  if (!ximage2->data)
    {
      fprintf (stderr, "%s: out of memory (scaling %dx%d image to %dx%d)\n",
               progname, ximage->width, ximage->height, w2, h2);
      exit (1);
    }

  for (y = 0; y < h2; y++)
    for (x = 0; x < w2; x++)
      XPutPixel (ximage2, x, y, XGetPixel (ximage, x*2, y*2));

  free (ximage->data);
  *ximage = *ximage2;
  ximage2->data = 0;
  XFree (ximage2);

  if (geom)
    {
      geom->x /= 2;
      geom->y /= 2;
      geom->width  /= 2;
      geom->height /= 2;
    }
}

static Bool
ximage_to_texture (XImage *ximage,
                   GLint type, GLint format,
                   int *width_return,
                   int *height_return,
                   XRectangle *geometry,
                   Bool mipmap_p)
{
  int max_reduction = 7;
  int err_count = 0;
  GLenum err = 0;
  int orig_width = ximage->width;
  int orig_height = ximage->height;
  int tex_width = 0;
  int tex_height = 0;

 AGAIN:

  if (mipmap_p)
    {
      /* gluBuild2DMipmaps doesn't require textures to be a power of 2. */
      tex_width  = ximage->width;
      tex_height = ximage->height;

      if (debug_p)
        fprintf (stderr, "%s: mipmap %d x %d\n",
                 progname, ximage->width, ximage->height);

      gluBuild2DMipmaps (GL_TEXTURE_2D, 3, ximage->width, ximage->height,
                         format, type, ximage->data);
      err = glGetError();
    }
  else
    {
      /* glTexImage2D() requires the texture sizes to be powers of 2.
         So first, create a texture of that size (but don't write any
         data into it.)
       */
      tex_width  = to_pow2 (ximage->width);
      tex_height = to_pow2 (ximage->height);

      if (debug_p)
        fprintf (stderr, "%s: texture %d x %d (%d x %d)\n",
                 progname, ximage->width, ximage->height,
                 tex_width, tex_height);

      glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0,
                    format, type, 0);
      err = glGetError();

      /* Now load our non-power-of-2 image data into the existing texture. */
      if (!err)
        {
          glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0,
                           ximage->width, ximage->height,
                           GL_RGBA, GL_UNSIGNED_BYTE, ximage->data);
          err = glGetError();
        }
    }

  if (err)
    {
      char buf[100];
      const char *s = (char *) gluErrorString (err);

      if (!s || !*s)
        {
          sprintf (buf, "unknown error %d", (int) err);
          s = buf;
        }

      while (glGetError() != GL_NO_ERROR)
        ;  /* clear any lingering errors */

      if (++err_count > max_reduction)
        {
          fprintf (stderr,
                   "\n"
                   "%s: %dx%d texture failed, even after reducing to %dx%d:\n"
                   "%s: The error was: \"%s\".\n"
                   "%s: probably this means "
                   "\"your video card is worthless and weak\"?\n\n",
                   progname, orig_width, orig_height,
                   ximage->width, ximage->height,
                   progname, s,
                   progname);
          return False;
        }
      else
        {
          if (debug_p)
            fprintf (stderr, "%s: mipmap error (%dx%d): %s\n",
                     progname, ximage->width, ximage->height, s);
          halve_image (ximage, geometry);
          goto AGAIN;
        }
    }

  if (width_return)  *width_return  = tex_width;
  if (height_return) *height_return = tex_height;
  return True;
}

static double
double_time (void)
{
#if 1
    return GetTickCount() / 1000.0;
#else
      struct timeval now;
    # ifdef GETTIMEOFDAY_TWO_ARGS
      struct timezone tzp;
      gettimeofday(&now, &tzp);
    # else
      gettimeofday(&now);
    # endif

      return (now.tv_sec + ((double) now.tv_usec * 0.000001));
#endif
}

static XImage *
pixmap_to_gl_ximage (Screen *screen, Window window, Pixmap pixmap)
{
  Display *dpy = DisplayOfScreen (screen);
  unsigned int width, height, depth;

  XImage *server_ximage = 0;

  {
    Window root;
    int x, y;
    unsigned int bw;
    XGetGeometry (dpy, pixmap, &root, &x, &y, &width, &height, &bw, &depth);
  }

  if (width < 5 || height < 5)  /* something's gone wrong somewhere... */
    return 0;

  /* Convert the server-side Pixmap to a client-side GL-ordered XImage.
   */

  if (!server_ximage)
    server_ximage = XGetImage (dpy, pixmap, 0, 0, width, height, ~0L, RGBAPixmap_);

  return server_ximage;
}

typedef struct {
  GLXContext glx_context;
  Pixmap pixmap;
  int pix_width, pix_height, pix_depth;
  int texid;
  Bool mipmap_p;
  double load_time;

  /* Used in async mode
   */
  void (*callback) (const char *filename, XRectangle *geometry,
                    int iw, int ih, int tw, int th,
                    void *closure);
  void *closure;

  /* Used in sync mode
   */
  char **filename_return;
  XRectangle *geometry_return;
  int *image_width_return;
  int *image_height_return;
  int *texture_width_return;
  int *texture_height_return;

} img_closure;

static void
load_texture_async_cb (Screen *screen, Window window, Drawable drawable,
                       const char *name, XRectangle *geometry, void *closure)
{
  Display *dpy = DisplayOfScreen (screen);
  Bool ok;
  XImage *ximage;
  GLint type, format;
  int iw=0, ih=0, tw=0, th=0;
  double cvt_time=0, tex_time=0, done_time=0;
  img_closure *data = (img_closure *) closure;
  /* copy closure data to stack and free the original before running cb */
  img_closure dd = *data;
  memset (data, 0, sizeof (*data));
  free (data);
  data = 0;

  if (dd.glx_context)
    glXMakeCurrent (dpy, window, dd.glx_context);

  if (geometry->width <= 0 || geometry->height <= 0)
    {
      /* This can happen if an old version of xscreensaver-getimage
         is installed. */
      geometry->x = 0;
      geometry->y = 0;
      geometry->width  = dd.pix_width;
      geometry->height = dd.pix_height;
    }

  if (geometry->width <= 0 || geometry->height <= 0)
    abort();

  if (debug_p)
    cvt_time = double_time();

  ximage = pixmap_to_gl_ximage (screen, window, dd.pixmap);
  format = GL_RGBA;
  type = GL_UNSIGNED_BYTE;

  XFreePixmap (dpy, dd.pixmap);
  dd.pixmap = 0;

  if (debug_p)
    tex_time = double_time();

  if (! ximage)
    ok = False;
  else
    {
      iw = ximage->width;
      ih = ximage->height;
      if (dd.texid != -1)
        glBindTexture (GL_TEXTURE_2D, dd.texid);

      glPixelStorei (GL_UNPACK_ALIGNMENT, ximage->bitmap_pad / 8);
      ok = ximage_to_texture (ximage, type, format, &tw, &th, geometry,
                              dd.mipmap_p);
      if (ok)
        {
          iw = ximage->width;	/* in case the image was shrunk */
          ih = ximage->height;
        }
    }

  if (ximage) XDestroyImage (ximage);

  if (! ok)
    iw = ih = tw = th = 0;

  if (debug_p)
    done_time = double_time();

  if (debug_p)
    fprintf (stderr,
             /* prints: A + B + C = D
                A = file I/O time (happens in background)
                B = time to pull bits from server (this process)
                C = time to convert bits to GL textures (this process)
                D = total elapsed time from "want image" to "see image"

                B+C is responsible for any frame-rate glitches.
              */
             "%s: loading elapsed: %.2f + %.2f + %.2f = %.2f sec\n",
             progname,
             cvt_time  - dd.load_time,
             tex_time  - cvt_time,
             done_time - tex_time,
             done_time - dd.load_time);

  if (dd.callback)
    /* asynchronous mode */
    dd.callback (name, geometry, iw, ih, tw, th, dd.closure);
  else
    {
      /* synchronous mode */
      if (dd.filename_return)       *dd.filename_return       = (char *) name;
      if (dd.geometry_return)       *dd.geometry_return       = *geometry;
      if (dd.image_width_return)    *dd.image_width_return    = iw;
      if (dd.image_height_return)   *dd.image_height_return   = ih;
      if (dd.texture_width_return)  *dd.texture_width_return  = tw;
      if (dd.texture_height_return) *dd.texture_height_return = th;
    }
}

#if 0
	void CreateTextureFromImage(XImage *ximage, GLuint texid)
	{
	    assert(ximage->format == RGBAPixmap_);
	    glBindTexture(GL_TEXTURE_2D, texid);
	    glPixelStorei(GL_UNPACK_ALIGNMENT, ximage->bitmap_pad / 8);

	    gluBuild2DMipmaps(GL_TEXTURE_2D, 4, ximage->width, ximage->height,
	        GL_RGBA, GL_UNSIGNED_BYTE, ximage->data);
	}
#endif

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
                   void *closure)
{
#if 1
  Display *dpy = DisplayOfScreen (screen);
  XWindowAttributes xgwa;
  img_closure *data = (img_closure *) calloc (1, sizeof(*data));

  if (debug_p)
    data->load_time = double_time();

  data->texid       = texid;
  data->mipmap_p    = mipmap_p;
  data->glx_context = glx_context;
  data->callback    = callback;
  data->closure     = closure;

  XGetWindowAttributes (dpy, window, &xgwa);
  data->pix_width  = xgwa.width;
  data->pix_height = xgwa.height;
  data->pix_depth  = xgwa.depth;

  if (desired_width  && desired_width  < xgwa.width)
    data->pix_width  = desired_width;
  if (desired_height && desired_height < xgwa.height)
    data->pix_height = desired_height;

  data->pixmap = XCreatePixmap (dpy, window, data->pix_width, data->pix_height,
                                data->pix_depth);
  load_image_async (screen, window, data->pixmap, 
                    load_texture_async_cb, data);
#else
    XImage *ximage;
    XRectangle geometry;

    ximage = GetScreenShotXImage();
    CreateTextureFromImage(ximage, texid);
    geometry.x = geometry.y = 0;
    geometry.width = ximage->width;
    geometry.height = ximage->height;

    (*callback)(NULL, &geometry, ximage->width, ximage->height,
        geometry.width, geometry.height, closure);
    XDestroyImage(ximage);
#endif
}
