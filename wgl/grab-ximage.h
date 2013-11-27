#ifndef __GRAB_XIMAGE_H__
#define __GRAB_XIMAGE_H__

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

#endif  // ndef __GRAB_XIMAGE_H__
