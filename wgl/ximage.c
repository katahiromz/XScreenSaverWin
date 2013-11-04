#include "win32.h"

void XInitImage(XImage *image)
{
	int width = image->width, depth = image->depth;

    assert(image != NULL);

    if (image->bytes_per_line == 0)
    {
        switch (image->bitmap_pad)
        {
        case 8:
            image->bytes_per_line = (width * depth + (8 - 1)) / 8;
            break;

        case 16:
            image->bytes_per_line = (width * depth + (16 - 1)) / 16 * 2;
            break;

        case 24:
            image->bytes_per_line = (width * depth + (24 - 1)) / 24 * 3;
            break;

        case 32:
            image->bytes_per_line = (width * depth + (32 - 1)) / 32 * 4;
            break;
        }
    }

    if (depth == 8)
    {
        image->red_mask = 0x07;
        image->green_mask = 0x38;
        image->blue_mask = 0xC0;
    }
    else if (depth == 16)
    {
        image->red_mask = 0x001F;
        image->green_mask = 0x03E0;
        image->blue_mask = 0x7C00;
    }
    else if (depth == 32)
    {
        image->red_mask = 0x000000FF;
        image->green_mask = 0x0000FF00;
        image->blue_mask = 0x00FF0000;
    }
    else if (depth == 1)
    {
        image->red_mask = 1;
        image->green_mask = 1;
        image->blue_mask = 1;
    }
}

XImage *XCreateImage(Display *dpy, Visual *visual, int depth, int format,
    int offset, char *data, unsigned int width, unsigned int height,
    int xpad, int image_bytes_per_line)
{
    XImage *image;
    assert(offset == 0);
    assert(depth != 0);
    assert(width != 0);
    assert(height != 0);

    if (depth == 0 || width == 0 || height == 0)
        return NULL;

    switch (format)
    {
    case XYBitmap:
    case XYPixmap:
    case ZPixmap:
    case RGBAPixmap:
        break;

    default:
        return NULL;
    }

    image = (XImage *)calloc(1, sizeof(XImage));
    if (image == NULL)
        return NULL;

    image->depth = image->bits_per_pixel = depth;
    image->width = width;
    image->height = height;
    image->format = format;
    image->bitmap_bit_order = image->byte_order = LSBFirst;
    image->data = data;
    image->bytes_per_line = image_bytes_per_line;
    image->xoffset = image->yoffset = 0;
    image->bitmap_pad = xpad;

    XInitImage(image);

    return image;
}

int XDestroyImage(XImage *image)
{
    free(image->data);
    free(image);
    return 1;
}

unsigned long XGetPixel(XImage *ximage, int x, int y)
{
    unsigned char bit, pixel;
    int xoff, yoff;
    unsigned char *addr;
    static unsigned long const low_bits_table[] =
    {
        0x00000000, 0x00000001, 0x00000003, 0x00000007,
        0x0000000f, 0x0000001f, 0x0000003f, 0x0000007f,
        0x000000ff, 0x000001ff, 0x000003ff, 0x000007ff,
        0x00000fff, 0x00001fff, 0x00003fff, 0x00007fff,
        0x0000ffff, 0x0001ffff, 0x0003ffff, 0x0007ffff,
        0x000fffff, 0x001fffff, 0x003fffff, 0x007fffff,
        0x00ffffff, 0x01ffffff, 0x03ffffff, 0x07ffffff,
        0x0fffffff, 0x1fffffff, 0x3fffffff, 0x7fffffff,
        0xffffffff
    };

    if ((ximage->bits_per_pixel | ximage->depth) == 1)
    {
        xoff = x + ximage->xoffset;
        yoff = y * ximage->bytes_per_line + (xoff >> 3);
        xoff &= 7;
        bit = 1 << xoff;
        return (ximage->data[yoff] & bit) ? 1 : 0;
    }

    if (ximage->format == ZPixmap)
    {
        if (ximage->bits_per_pixel == 8)
        {
            pixel = ((unsigned char *)ximage->data)[y * ximage->bytes_per_line + x];
            if (ximage->depth != 8)
                pixel &= low_bits_table[ximage->depth];
            return pixel;
        }
        if (ximage->bits_per_pixel == 16)
        {
            addr = &((unsigned char *)ximage->data)
                [y * ximage->bytes_per_line + (x << 1)];
            pixel = addr[1] << 8 | addr[0];
            if (ximage->depth != 16)
                pixel &= low_bits_table[ximage->depth];
            return pixel;
        }
        if (ximage->bits_per_pixel == 32)
        {
            addr = &((unsigned char *)ximage->data)[y * ximage->bytes_per_line + (x << 2)];
            pixel = ((unsigned long)addr[3] << 24 |
                     (unsigned long)addr[2] << 16 |
                     (unsigned long)addr[1] << 8 |
                 addr[0]);
            if (ximage->depth != 32)
                pixel &= low_bits_table[ximage->depth];
            return pixel;
        }
    }

    if (ximage->format == RGBAPixmap && ximage->bits_per_pixel == 32)
    {
        addr = &((unsigned char *)ximage->data)[y * ximage->bytes_per_line + (x << 2)];
        return addr[0] | (addr[1] << 8) | (addr[2] << 16) | (addr[2] << 24);
    }

    return 0;
}

int XPutPixel(XImage *ximage, int x, int y, unsigned long pixel)
{
    int xoff, yoff;
    unsigned char bit, *addr;

    if ((ximage->bits_per_pixel | ximage->depth) == 1)
    {
        xoff = x + ximage->xoffset;
        yoff = y * ximage->bytes_per_line + (xoff >> 3);
        xoff &= 7;
        bit = 1 << xoff;
        if (pixel & 1)
            ximage->data[yoff] |= bit;
        else
            ximage->data[yoff] &= ~bit;
        return 1;
    }

    if (ximage->format == ZPixmap && ximage->bits_per_pixel == 8)
    {
        ximage->data[y * ximage->bytes_per_line + x] = pixel;
        return 1;
    }

    if (ximage->format == ZPixmap && ximage->bits_per_pixel == 16)
    {
        addr = &((unsigned char *)ximage->data)
            [y * ximage->bytes_per_line + (x << 1)];
        if (ximage->byte_order == MSBFirst)
        {
            addr[0] = pixel >> 8;
            addr[1] = pixel;
        }
        else
        {
            addr[1] = pixel >> 8;
            addr[0] = pixel;
        }
        return 1;
    }

    if (ximage->format == ZPixmap && ximage->bits_per_pixel == 32)
    {
        addr = &((unsigned char *)ximage->data)[y * ximage->bytes_per_line + (x << 2)];
        if (ximage->byte_order == MSBFirst)
        {
            addr[0] = pixel >> 24;
            addr[1] = pixel >> 16;
            addr[2] = pixel >> 8;
            addr[3] = pixel;
        }
        else
        {
            addr[3] = pixel >> 24;
            addr[2] = pixel >> 16;
            addr[1] = pixel >> 8;
            addr[0] = pixel;
        }
        return 1;
    }

    if (ximage->format == RGBAPixmap && ximage->bits_per_pixel == 32)
    {
        addr = &((unsigned char *)ximage->data)[y * ximage->bytes_per_line + (x << 2)];
        if (ximage->byte_order == MSBFirst)
        {
            addr[0] = pixel >> 24;
            addr[1] = pixel >> 16;
            addr[2] = pixel >> 8;
            addr[3] = pixel;
        }
        else
        {
            addr[3] = pixel >> 24;
            addr[2] = pixel >> 16;
            addr[1] = pixel >> 8;
            addr[0] = pixel;
        }
        return 1;
    }

    assert(0);
    return 0;
}

GC XCreateGC(
     Display *dpy, 
     Drawable d,
     unsigned long valuemask, 
     XGCValues *values)
{
    GC hdc;
    if (values == NULL)
        return NULL;

    hdc = CreateCompatibleDC((HDC)dpy);
    return hdc;
}

int XFreeGC(Display *dpy, GC gc)
{
    DeleteDC(gc);
    return 1;
}

XImage *GetScreenShotXImage(void)
{
    XImage *image;
    INT cx, cy, size;
    BITMAP bm;
    HBITMAP hbm = ss.hbmScreenShot;

    GetObject(hbm, sizeof(bm), &bm);
    cx = bm.bmWidth;
    cy = bm.bmHeight;

    image = XCreateImage(NULL, NULL, 32, RGBAPixmap, 0, NULL, cx, cy, 32, 0);
    if (image)
    {
        size = image->bytes_per_line * image->height;
        image->data = (char *)calloc(size, 1);
        if (image->data != NULL)
        {
            memcpy(image->data, bm.bmBits, size);
            return image;
        }
        free(image->data);
        free(image);
        image = NULL;
    }
    return image;
}

static int to_pow2(int value)
{
  int i = 1;
  while (i < value) i <<= 1;
  return i;
}

void CreateTextureFromImage(XImage *ximage, GLuint texid)
{
    assert(ximage->format == RGBAPixmap);
    glBindTexture(GL_TEXTURE_2D, texid);
    glPixelStorei(GL_UNPACK_ALIGNMENT, ximage->bitmap_pad / 8);

    gluBuild2DMipmaps(GL_TEXTURE_2D, 4, ximage->width, ximage->height,
        GL_RGBA, GL_UNSIGNED_BYTE, ximage->data);
}

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
}

#if 0
    #define MAX_PIXMAPS 256

    static int num_pixmaps = 0;
    static HBITMAP pixmaps[MAX_PIXMAPS] = {NULL};

    Pixmap XCreatePixmap(
        Display *dpy,
        Drawable d,
        unsigned int width,
        unsigned int height,
        unsigned int depth)
    {
        BITMAPINFO bi;
        LPVOID pvBits;
        INT i;

        if (num_pixmaps >= MAX_PIXMAPS)
            return 0;

        ZeroMemory(&bi, sizeof(bi));
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = width;
        bi.bmiHeader.biHeight = height;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 24;   // always 24

        for (i = 0; i < MAX_PIXMAPS; i++)
        {
            if (pixmaps[i] == NULL)
            {
                pixmaps[i] = CreateDIBSection((HDC)dpy, &bi, 
                    DIB_RGB_COLORS, &pvBits, NULL, 0);
                if (pixmaps[i] != NULL)
                {
                    num_pixmaps++;
                    return i;
                }
                break;
            }
        }
        return 0;
    }

    int XFreePixmap(Display *dpy, Pixmap pixmap)
    {
        if (pixmaps[pixmap] != NULL)
        {
            DeleteObject(pixmaps[pixmap]);
            pixmaps[pixmap] = NULL;
            return 1;
        }
        return 0;
    }

    int XPutImage(
        Display*    display,
        Drawable    d,
        GC          gc,
        XImage*     image,
        int         src_x,
        int         src_y,
        int         dest_x,
        int         dest_y,
        unsigned int    width,
        unsigned int    height)
    {
        int x, y;
        XColor color;
        HGDIOBJ hbmOld;

        hbmOld = SelectObject(gc, pixmaps[d]);
        for (y = src_y; y < src_y + height; ++y)
        {
            for (x = src_x; x < src_x + width; ++x)
            {
                color.pixel = XGetPixel(image, x, y);
                XQueryColor(display, 0, &color);
                SetPixel(gc, x, y, RGB(color.red / 256, color.green / 256, color.blue / 256));
            }
        }
        SelectObject(gc, hbmOld);

        return 1;
    }

    int XCopyArea(
         Display *dpy,
         Drawable src_drawable, Drawable dst_drawable, GC gc,
         int src_x, int src_y,
         unsigned int width, unsigned int height,
         int dst_x, int dst_y)
    {
        HDC hdcDst = CreateCompatibleDC(gc);
        HDC hdcSrc = CreateCompatibleDC(gc);
        HGDIOBJ hbmOld1 = SelectObject(hdcDst, pixmaps[dst_drawable]);
        HGDIOBJ hbmOld2 = SelectObject(hdcSrc, pixmaps[src_drawable]);
        BitBlt(hdcDst, dst_x, dst_y, width, height,
            hdcSrc, src_x, src_y, SRCCOPY);
        SelectObject(hdcDst, hbmOld1);
        SelectObject(hdcSrc, hbmOld2);
        DeleteDC(hdcSrc);
        DeleteDC(hdcDst);
        return 1;
    }
#endif
