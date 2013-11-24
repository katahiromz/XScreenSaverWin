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

        default:
            assert(0);
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

//////////////////////////////////////////////////////////////////////////////

typedef struct tagBITMAPINFOEX
{
    BITMAPINFOHEADER bmiHeader;
    RGBQUAD          bmiColors[256];
} BITMAPINFOEX, * LPBITMAPINFOEX;

#define WIDTHBYTES(i) (((i) + 31) / 32 * 4)

HBITMAP XCreateWinBitmapFromXImage(XImage *ximage)
{
    BITMAPINFOEX bi;
    HBITMAP hbm;
    LPBYTE pbBits;
    unsigned int i, x, y, widthbytes;
    XColor color;

    ZeroMemory(&bi, sizeof(bi));
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = ximage->width;
    bi.bmiHeader.biHeight = ximage->height;
    bi.bmiHeader.biPlanes = 1;
    if ((ximage->bits_per_pixel | ximage->depth) == 1)
    {
        bi.bmiHeader.biBitCount = 1;
        bi.bmiColors[0].rgbBlue = 0;
        bi.bmiColors[0].rgbGreen = 0;
        bi.bmiColors[0].rgbRed = 0;
        bi.bmiColors[0].rgbReserved = 0;
        bi.bmiColors[1].rgbBlue = 255;
        bi.bmiColors[1].rgbGreen = 255;
        bi.bmiColors[1].rgbRed = 255;
        bi.bmiColors[1].rgbReserved = 0;
        hbm = CreateDIBSection(NULL, (LPBITMAPINFO)&bi, DIB_RGB_COLORS, 
                               (LPVOID *)&pbBits, NULL, 0);
        widthbytes = WIDTHBYTES(ximage->width * 1);
        for (y = 0; y < ximage->height; y++)
        {
        	for (x = 0; x < widthbytes; x++)
        	{
        		int i;
				BYTE b1 = ximage->data[(ximage->height - y - 1) * ximage->bytes_per_line + x];
        		BYTE b2 = 0;
        		for (i = 0; i < 8; i++)
        		{
        			if (b1 & (1 << i))
	        			b2 |= 1 << (7 - i);
        		}
	            pbBits[y * widthbytes + x] = b2;
        	}
        }
        return hbm;
    }

    if (ximage->format == ZPixmap && ximage->bits_per_pixel == 8)
    {
        bi.bmiHeader.biBitCount = 8;
        for (i = 0; i < 256; i++)
        {
            color.pixel = i;
            XQueryColor(NULL, 0, &color);
            bi.bmiColors[i].rgbBlue = color.blue / 256;
            bi.bmiColors[i].rgbGreen = color.green / 256;
            bi.bmiColors[i].rgbRed = color.red / 256;
            bi.bmiColors[i].rgbReserved = 0;
        }
        hbm = CreateDIBSection(NULL, (LPBITMAPINFO)&bi, DIB_RGB_COLORS, 
                               (LPVOID *)&pbBits, NULL, 0);
        widthbytes = WIDTHBYTES(ximage->width * 8);
        for (y = 0; y < ximage->height; y++)
        {
            for (x = 0; x < ximage->width; x++)
            {
                pbBits[y * widthbytes + x] = ximage->data[(ximage->height - y - 1) * ximage->bytes_per_line + x];
            }
        }
        return hbm;
    }

    if (ximage->format == ZPixmap && ximage->bits_per_pixel == 32)
    {
        bi.bmiHeader.biBitCount = 32;
        hbm = CreateDIBSection(NULL, (LPBITMAPINFO)&bi, DIB_RGB_COLORS, 
                               (LPVOID *)&pbBits, NULL, 0);
        widthbytes = WIDTHBYTES(ximage->width * 32);
        for (y = 0; y < ximage->height; y++)
        {
            for (x = 0; x < ximage->width; x++)
            {
                color.pixel = *(LPDWORD)&ximage->data[(ximage->height - y - 1) * ximage->bytes_per_line + x * 4];
                XQueryColor(NULL, 0, &color);
                pbBits[y * widthbytes + x * 4 + 0] = color.blue / 256;
                pbBits[y * widthbytes + x * 4 + 1] = color.green / 256;
                pbBits[y * widthbytes + x * 4 + 2] = color.red / 256;
                pbBits[y * widthbytes + x * 4 + 3] = 0xFF;
            }
        }
        return hbm;
    }

    if (ximage->format == RGBAPixmap_ && ximage->bits_per_pixel == 32)
    {
        bi.bmiHeader.biBitCount = 32;
        hbm = CreateDIBSection(NULL, (LPBITMAPINFO)&bi, DIB_RGB_COLORS, 
                               (LPVOID *)&pbBits, NULL, 0);
        widthbytes = WIDTHBYTES(ximage->width * 32);
        for (y = 0; y < ximage->height; y++)
        {
            for (x = 0; x < ximage->width; x++)
            {
                pbBits[y * widthbytes + x * 4 + 0] = ximage->data[(ximage->height - y - 1) * ximage->bytes_per_line + x * 4 + 2];
                pbBits[y * widthbytes + x * 4 + 1] = ximage->data[(ximage->height - y - 1) * ximage->bytes_per_line + x * 4 + 1];
                pbBits[y * widthbytes + x * 4 + 2] = ximage->data[(ximage->height - y - 1) * ximage->bytes_per_line + x * 4 + 0];
                pbBits[y * widthbytes + x * 4 + 3] = 0xFF;
            }
        }
        return hbm;
    }

    return NULL;
}

XImage *XGetImage(Display *dpy, Drawable d,
     int x, int y, unsigned int width, unsigned int height,
     unsigned long plane_mask, int format)
{
    HBITMAP hbm;
    BITMAPINFO bi;
    LPBYTE pbBits;
    HGDIOBJ hbmOld;
    XImage *ximage;
    unsigned int x2, y2, widthbytes, xoff, yoff, bit;
    HDC hdcDst, hdcSrc;
    XColor color;

    ZeroMemory(&bi, sizeof(bi));
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = width;
    bi.bmiHeader.biHeight = height;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;

    hbm = CreateDIBSection(dpy, &bi, DIB_RGB_COLORS, (LPVOID *)&pbBits, NULL, 0);
    if (hbm == NULL)
        return NULL;

    hdcDst = CreateCompatibleDC(dpy);
    hdcSrc = XCreateDrawableDC_(dpy, d);
    hbmOld = SelectObject(hdcDst, hbm);
    BitBlt(hdcDst, 0, 0, width, height, hdcSrc, x, y, SRCCOPY);
    SelectObject(hdcDst, hbmOld);
    XDeleteDrawableDC_(dpy, d, hdcSrc);
    DeleteDC(hdcDst);
    GdiFlush();

    switch (format)
    {
    case XYPixmap:
        ximage = XCreateImage(dpy, NULL, 1, format, 0, NULL, width, height, 8, 0);
        widthbytes = WIDTHBYTES(width * 1);
        ximage->data = (char *)calloc(ximage->bytes_per_line, height);
        if (ximage->data != NULL)
        {
            for (y2 = 0; y2 < height; y2++)
            {
                for (x2 = 0; x2 < width; x2++)
                {
                    if (pbBits[(height - y2 - 1) * widthbytes + x2 * 4 + 0] ||
                        pbBits[(height - y2 - 1) * y2 * widthbytes + x2 * 4 + 1] ||
                        pbBits[(height - y2 - 1) * y2 * widthbytes + x2 * 4 + 2])
                    {
                        xoff = x2;
                        yoff = y2 * ximage->bytes_per_line + (xoff >> 3);
                        xoff &= 7;
                        bit = 1 << xoff;
                        ximage->data[yoff] |= bit;
                    }
                }
            }
        }
        else
        {
            XDestroyImage(ximage);
            ximage = NULL;
        }
        break;

    case ZPixmap:
        ximage = XCreateImage(dpy, NULL, 32, format, 0, NULL, width, height, 8, 0);
        ximage->data = (char *)calloc(ximage->bytes_per_line, height);
        widthbytes = WIDTHBYTES(width * 32);
        if (ximage->data != NULL)
        {
            for (y2 = 0; y2 < height; y2++)
            {
                for (x2 = 0; x2 < width; x2++)
                {
                    color.red = pbBits[(height - y2 - 1) * widthbytes + x2 * 4 + 2] * 256;
                    color.green = pbBits[(height - y2 - 1) * widthbytes + x2 * 4 + 1] * 256;
                    color.blue = pbBits[(height - y2 - 1) * widthbytes + x2 * 4 + 0] * 256;
                    XAllocColor(dpy, 0, &color);
                    XPutPixel(ximage, x2, y2, color.pixel);
                }
            }
        }
        else
        {
            XDestroyImage(ximage);
            ximage = NULL;
        }
        break;

    case RGBAPixmap_:
        ximage = XCreateImage(dpy, NULL, 32, format, 0, NULL, width, height, 8, 0);
        ximage->data = (char *)calloc(ximage->bytes_per_line, height);
        widthbytes = WIDTHBYTES(width * 32);
        if (ximage->data != NULL)
        {
            for (y2 = 0; y2 < height; y2++)
            {
                for (x2 = 0; x2 < width; x2++)
                {
                    ximage->data[ximage->bytes_per_line * y2 + x2 * 4 + 0] = pbBits[(height - y2 - 1) * widthbytes + x2 * 4 + 2];
                    ximage->data[ximage->bytes_per_line * y2 + x2 * 4 + 1] = pbBits[(height - y2 - 1) * widthbytes + x2 * 4 + 1];
                    ximage->data[ximage->bytes_per_line * y2 + x2 * 4 + 2] = pbBits[(height - y2 - 1) * widthbytes + x2 * 4 + 0];
                    ximage->data[ximage->bytes_per_line * y2 + x2 * 4 + 3] = 0xFF;
                }
            }
        }
        else
        {
            XDestroyImage(ximage);
            ximage = NULL;
        }
        break;

    default:
        ximage = NULL;
        break;
    }

    DeleteObject(hbm);
    return ximage;
}

// NOTE: too slow!!!
int XPutImage(Display *dpy, Drawable d, GC gc,
    XImage *image, int req_xoffset, int req_yoffset,
    int x, int y, unsigned int req_width, unsigned int req_height)
{
    HBITMAP hbm;

    hbm = XCreateWinBitmapFromXImage(image);
    assert(hbm != NULL);
    if (hbm == NULL)
        return 0;

    if (d == 0)
    {
        HDC hdcSrc = CreateCompatibleDC(dpy);
        HGDIOBJ hbmOld = SelectObject(hdcSrc, hbm);
        BitBlt(dpy, x, y, req_width, req_height, hdcSrc, req_xoffset, req_yoffset, SRCCOPY);
        SelectObject(hdcSrc, hbmOld);
        DeleteDC(hdcSrc);
    }
    else
    {
        HDC hdc = XCreateDrawableDC_(dpy, d);
        HDC hdcSrc = CreateCompatibleDC(dpy);
        HGDIOBJ hbmOld = SelectObject(hdcSrc, hbm);
        BitBlt(hdc, x, y, req_width, req_height, hdcSrc, req_xoffset, req_yoffset, SRCCOPY);
        SelectObject(hdcSrc, hbmOld);
        DeleteDC(hdcSrc);
        XDeleteDrawableDC_(dpy, d, hdc);
    }

    DeleteObject(hbm);
    return 0;
}

XImage *XSubImage(XImage *ximage,
    int x, int y, unsigned int width, unsigned int height)
{
    XImage *newimage;
    unsigned int x2, y2, pixel;

    newimage = XCreateImage(NULL, NULL, ximage->bits_per_pixel, ximage->format,
        0, NULL, width, height, 8, 0);
    if (newimage == NULL)
        return NULL;

    newimage->data = (char *)calloc(newimage->bytes_per_line, height);
    if (newimage->data == NULL)
    {
        XDestroyImage(newimage);
        return NULL;
    }

    for (y2 = 0; y2 < height; y2++)
    {
        for (x2 = 0; x2 < width; x2++)
        {
            pixel = XGetPixel(ximage, x + x2, y + y2);
            XPutPixel(newimage, x2, y2, pixel);
        }
    }
    return newimage;
}

//////////////////////////////////////////////////////////////////////////////

int XCopyPlane(Display *dpy, Drawable src_drawable, Drawable dst_drawable, GC gc,
     int src_x, int src_y, unsigned int width, unsigned int height,
     int dst_x, int dst_y, unsigned long bit_plane)
{
    XGCValues *values;
    COLORREF rgb, rgbBack, rgbFore;
    unsigned int x, y;
    HDC hdc, hdcSrc;

    assert(bit_plane == 1);
    assert(src_drawable != NULL);

    values = XGetGCValues_(gc);
    if (values == NULL)
        return BadGC;

    rgbBack = values->background_rgb;
    rgbFore = values->foreground_rgb;

    if (dst_drawable == NULL)
    {
        hdcSrc = XCreateDrawableDC_(dpy, src_drawable);
        for (y = 0; y < height; y++)
        {
            for (x = 0; x < width; x++)
            {
                rgb = GetPixel(hdcSrc, src_x + x, src_y + y);
                if (rgb)
                    SetPixelV(dpy, dst_x + x, dst_y + y, rgbFore);
                else
                    SetPixelV(dpy, dst_x + x, dst_y + y, rgbBack);
            }
        }
        XDeleteDrawableDC_(dpy, src_drawable, hdcSrc);
    }
    else
    {
        assert(dst_drawable != src_drawable);
        hdc = XCreateDrawableDC_(dpy, dst_drawable);
        hdcSrc = XCreateDrawableDC_(dpy, src_drawable);
        for (y = 0; y < height; y++)
        {
            for (x = 0; x < width; x++)
            {
                rgb = GetPixel(hdcSrc, src_x + x, src_y + y);
                if (rgb)
                    SetPixelV(hdc, dst_x + x, dst_y + y, rgbFore);
                else
                    SetPixelV(hdc, dst_x + x, dst_y + y, rgbBack);
            }
        }
        XDeleteDrawableDC_(dpy, dst_drawable, hdc);
        XDeleteDrawableDC_(dpy, src_drawable, hdcSrc);
    }

    return 0;
}

//////////////////////////////////////////////////////////////////////////////
