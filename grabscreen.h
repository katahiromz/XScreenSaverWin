#ifndef __GRABSCREEN_H__
#define __GRABSCREEN_H__

#ifndef __XWS2WIN_H__
    #include "xws2win.h"
#endif

typedef struct async_load_state
{
    Bool done_p;
    char *filename;
    XRectangle geom;
} async_load_state;

async_load_state *load_image_async_simple(
    async_load_state *state, Screen *screen, Window window, Drawable target, 
    char **filename_ret, XRectangle *geometry_ret);

void
load_image_async (Screen *screen, Window window, Drawable drawable,
                  void (*callback) (Screen *, Window, Drawable,
                                    const char *name, XRectangle *geom,
                                    void *closure),
                  void *closure);

#endif  // ndef __GRABSCREEN_H__
