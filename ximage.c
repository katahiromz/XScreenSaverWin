#include "xlockmore.h"

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
    case RGBAPixmap_:
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

    if (ximage->format == RGBAPixmap_ && ximage->bits_per_pixel == 32)
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

    if (ximage->format == RGBAPixmap_ && ximage->bits_per_pixel == 32)
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
