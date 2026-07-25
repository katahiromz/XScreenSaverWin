#include "xlockmore.h"

typedef struct
{
    BYTE red;
    BYTE green;
    BYTE blue;
    const char *name;
} NAMED_COLOR_ENTRY;

static const NAMED_COLOR_ENTRY named_color_entries[] =
{
#define DEFINE_COLOR(r, g, b, name) { r, g, b, name },
#include "COLORS.txt"
#undef DEFINE_COLOR
};

static xColorItem win_colormap_items[MAX_COLORCELLS] =
{
    {0, 0, 0},                      //[0]
    {0x8000, 0, 0},                 //[1]
    {0, 0x8000, 0},                 //[2]
    {0x8000, 0x8000, 0},            //[3]
    {0, 0, 0x8000},                 //[4]
    {0x8000, 0, 0x8000},            //[5]
    {0, 0x8000, 0x8000},            //[6]
    {0xc000, 0xc000, 0xc000},       //[7]
    {0xc000, 0xdc00, 0xc000},       //[8]
    {0xa600, 0xca00, 0xf000},       //[9]
    // [10]
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    // [20]
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    // [30]
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    // [40]
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    // [50]
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    // [60]
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    // [70]
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    // [80]
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    // [90]
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    // [100]
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    // [110]
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    // [120]
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    // [130]
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    // [140]
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    // [150]
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    // [160]
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    // [170]
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    // [180]
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    // [190]
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    // [200]
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    // [210]
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    // [220]
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    // [230]
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    // [240]
    {0}, {0}, {0}, {0}, {0}, {0},
    // [246]
    {0xff00, 0xfb00, 0xf000},       // [246]
    {0xa000, 0xa000, 0xa400},       // [247]
    {0x8000, 0x8000, 0x8000},       // [248]
    {0xff00, 0, 0},                 // [249]
    {0, 0xff00, 0},                 // [250]
    {0xff00, 0xff00, 0},            // [251]
    {0, 0, 0xff00},                 // [252]
    {0xff00, 0, 0xff00},            // [253]
    {0, 0xff00, 0xff00},            // [254]
    {0xff00, 0xff00, 0xff00}        // [255]
};

static BOOL win_pixel_used[MAX_COLORCELLS] =
{
    // [0]
    TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,
    // [10]
    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
    // [20]
    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
    // [30]
    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
    // [40]
    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
    // [50]
    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
    // [60]
    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
    // [70]
    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
    // [80]
    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
    // [90]
    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
    // [100]
    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
    // [110]
    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
    // [120]
    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
    // [130]
    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
    // [140]
    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
    // [150]
    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
    // [160]
    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
    // [170]
    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
    // [180]
    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
    // [190]
    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
    // [200]
    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
    // [210]
    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
    // [220]
    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
    // [230]
    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
    // [240]
    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
    // [246]
    TRUE,        // [246]
    TRUE,        // [247]
    TRUE,        // [248]
    TRUE,        // [249]
    TRUE,        // [250]
    TRUE,        // [251]
    TRUE,        // [252]
    TRUE,        // [253]
    TRUE,        // [254]
    TRUE         // [255]
};

static int colormap_num = 1;

ColormapData colormaps[MAX_COLORMAP] =
{
    {20, win_colormap_items, win_pixel_used},
};

Colormap XCreateColormap(
    Display *d,
    Window w,
    Visual *v,
    int alloc)
{
    int i;

    if (colormap_num >= MAX_COLORMAP)
        return 0;

    for (i = 0; i < MAX_COLORMAP; i++)
    {
        if (colormaps[i].num_items == 0)
        {
            colormaps[i].num_items = 20;

            // allocate colormap items
            colormaps[i].items =
                (xColorItem *)calloc(256, sizeof(xColorItem));
            if (colormaps[colormap_num].items == NULL)
                return 0;
            memcpy(colormaps[i].items, win_colormap_items, 256 * sizeof(xColorItem));

            // allocate pixel_used
            colormaps[i].pixel_used = (BOOL *)calloc(MAX_COLORCELLS, sizeof(BOOL));
            if (colormaps[i].pixel_used == NULL)
            {
                free(colormaps[i].items);
                return 0;
            }
            memcpy(colormaps[i].pixel_used, win_pixel_used, MAX_COLORCELLS * sizeof(BOOL));

            colormap_num++;
            return i;
        }
    }

    return 0;
}

Bool XFreeColormap(
    Display *d,
    Colormap cmap)
{
    if (cmap == 0)
        return False;

    if (colormaps[cmap].num_items != 0)
    {
        colormaps[cmap].num_items = 0;
        free(colormaps[cmap].items);
        colormaps[cmap].items = NULL;
        free(colormaps[cmap].pixel_used);
        colormaps[cmap].pixel_used = NULL;
        colormap_num--;
        return True;
    }
    return False;
}

static INT FindPixelInColormap(Colormap cmap, WORD red, WORD green, WORD blue)
{
    int pixel;
    for (pixel = 0; pixel < 256; pixel++)
    {
        if (colormaps[cmap].pixel_used[pixel] &&
            colormaps[cmap].items[pixel].red == red &&
            colormaps[cmap].items[pixel].green == green &&
            colormaps[cmap].items[pixel].blue == blue)
        {
            return pixel;
        }
    }
    return -1;
}

static INT AddNewColor(Colormap cmap, WORD red, WORD green, WORD blue)
{
    int pixel;
    for (pixel = 0; pixel < 256; pixel++)
    {
        if (colormaps[cmap].pixel_used[pixel])
            continue;

        colormaps[cmap].pixel_used[pixel] = TRUE;
        colormaps[cmap].items[pixel].red = red;
        colormaps[cmap].items[pixel].green = green;
        colormaps[cmap].items[pixel].blue = blue;
        colormaps[cmap].num_items++;
        return pixel;
    }
    return -1;
}

static INT GetNearestPixel(Colormap cmap, WORD red, WORD green, WORD blue,
    WORD *true_red, WORD *true_green, WORD *true_blue)
{
    int pixel, nearestPixel = -1;
    DWORD norm, nearestNorm = 0xFFFFFFFF;

    for (pixel = 0; pixel < 256; pixel++)
    {
        if (!colormaps[cmap].pixel_used[pixel])
            continue;

        norm =
            abs((INT)colormaps[cmap].items[pixel].red - (INT)red) +
            abs((INT)colormaps[cmap].items[pixel].green - (INT)green) +
            abs((INT)colormaps[cmap].items[pixel].blue - (INT)blue);
        if (norm < nearestNorm)
        {
            nearestPixel = pixel;
            nearestNorm = norm;
        }
    }
    if (nearestNorm < 128)
    {
        *true_red = colormaps[cmap].items[nearestPixel].red;
        *true_green = colormaps[cmap].items[nearestPixel].green;
        *true_blue = colormaps[cmap].items[nearestPixel].blue;
        return nearestPixel;
    }
    return -1;
}

int XParseColor(Display *d, Colormap cmap, const char *name, XColor *c)
{
    int i, size = sizeof(named_color_entries) / sizeof(named_color_entries[0]);
    if (name[0] == '#')
    {
        unsigned long ul = strtoul(&name[1], NULL, 16);
        c->red = ((unsigned char)(ul >> 16)) * 256;
        c->green = ((unsigned char)(ul >> 8)) * 256;
        c->blue = ((unsigned char)ul) * 256;
        return 1;
    }
    for (i = 0; i < size; i++)
    {
        if (_stricmp(named_color_entries[i].name, name) == 0)
        {
            c->red = named_color_entries[i].red * 256;
            c->green = named_color_entries[i].green * 256;
            c->blue = named_color_entries[i].blue * 256;
            return 1;
        }
    }
    return 0;
}

Bool XAllocNamedColor(
    Display *d, Colormap cmap, const char *name,
    XColor *near_color, XColor *true_color)
{
    WORD red, green, blue;
    WORD true_red, true_green, true_blue;
    INT pixel;
    int i, size = sizeof(named_color_entries) / sizeof(named_color_entries[0]);
    for (i = 0; i < size; i++)
    {
        if (strcmp(named_color_entries[i].name, name) == 0)
        {
            red = named_color_entries[i].red * 256;
            green = named_color_entries[i].green * 256;
            blue = named_color_entries[i].blue * 256;
            pixel = FindPixelInColormap(cmap, red, green, blue);
            if (pixel != -1)
            {
                near_color->pixel = true_color->pixel = pixel;
                near_color->red = true_color->red = red;
                near_color->green = true_color->green = green;
                near_color->blue = true_color->blue = blue;
                return True;
            }
            pixel = AddNewColor(cmap, red, green, blue);
            if (pixel != -1)
            {
                near_color->pixel = true_color->pixel = pixel;
                near_color->red = true_color->red = red;
                near_color->green = true_color->green = green;
                near_color->blue = true_color->blue = blue;
                return True;
            }
            pixel = GetNearestPixel(cmap, red, green, blue,
                &true_red, &true_green, &true_blue);
            if (pixel != -1)
            {
                near_color->pixel = pixel;
                near_color->red = red;
                near_color->green = green;
                near_color->blue = blue;
                true_color->pixel = pixel;
                true_color->red = true_red;
                true_color->green = true_green;
                true_color->blue = true_blue;
                return True;
            }
            break;
        }
    }
    return False;
}

Bool XAllocColor(Display *d, Colormap cmap, XColor *color)
{
    WORD red, green, blue;
    WORD true_red, true_green, true_blue;
    INT pixel;

    red = color->red;
    green = color->green;
    blue = color->blue;

    pixel = FindPixelInColormap(cmap, red, green, blue);
    if (pixel != -1)
    {
        color->pixel = pixel;
        return True;
    }
    pixel = AddNewColor(cmap, red, green, blue);
    if (pixel != -1)
    {
        color->pixel = pixel;
        return True;
    }
    pixel = GetNearestPixel(cmap, red, green, blue,
        &true_red, &true_green, &true_blue);
    if (pixel != -1)
    {
        color->pixel = pixel;
        return True;
    }
    return False;
}

Status XAllocColorCells(
    Display*        d,
    Colormap        cmap,
    Bool            contig,
    unsigned long*  plane_masks_return,
    unsigned int    nplanes,
    unsigned long*  pixels_return,
    unsigned int    npixels)
{
    unsigned int i;
    unsigned long pixel;

    if (colormaps[cmap].num_items + npixels > 256)
        return False;

    assert(!contig);
    assert(plane_masks_return == NULL);
    assert(nplanes == 0);
    for (i = 0; i < npixels; i++)
    {
        for (pixel = 0; pixel < 256; pixel++)
        {
            if (colormaps[cmap].pixel_used[pixel])
                continue;

            pixels_return[i] = pixel;
            colormaps[cmap].pixel_used[pixel] = TRUE+
            colormaps[cmap].num_items++;
            break;
        }
    }

    return True;
}

int XFreeColors(
    Display*        d,
    Colormap        cmap,
    unsigned long*  pixels,
    int             npixels,
    unsigned long   planes)
{
    int i;
    for (i = 0; i < npixels; i++)
    {
        if (colormaps[cmap].pixel_used[pixels[i]])
            colormaps[cmap].pixel_used[pixels[i]] = FALSE;
    }
    return 1;
}

int XStoreColors(
    Display*    display,
    Colormap    cmap,
    XColor*     color,
    int         ncolors)
{
    int i, pixel;
    WORD red, green, blue;
    int *pixels = (int *)malloc(ncolors * sizeof(int));
    if (pixels == NULL)
        return 0;

    for (i = 0; i < ncolors; i++)
    {
        red = color[i].red;
        green = color[i].green;
        blue = color[i].blue;

        pixel = FindPixelInColormap(cmap, red, green, blue);
        if (pixel != -1)
        {
            pixels[i] = pixel;
            continue;
        }
        pixel = AddNewColor(cmap, red, green, blue);
        if (pixel != -1)
        {
            pixels[i] = pixel;
            continue;
        }

        free(pixels);
        return False;
    }

    for (i = 0; i < ncolors; i++)
    {
        color[i].pixel = pixels[i];
    }

    free(pixels);
    return True;
}

int XFlush(Display *d)
{
    GdiFlush();
    return 0;
}

int visual_cells(Screen *screen, Visual *visual)
{
    Colormap cmap = DefaultColormap(DisplayOfScreen(screen), screen);
    return colormaps[cmap].num_items;
}

int XQueryColors(Display *dpy, Colormap cmap, XColor *defs, int ncolors)
{
    int i, pixel;
    assert(defs != NULL);

    for (i = 0; i < ncolors; i++)
    {
        pixel = defs[i].pixel;
        defs[i].red = colormaps[cmap].items[pixel].red;
        defs[i].blue = colormaps[cmap].items[pixel].blue;
        defs[i].green = colormaps[cmap].items[pixel].green;
        defs[i].flags = DoRed | DoGreen | DoBlue;
    }
    return 0;
}

unsigned long load_color(Display *dpy, Colormap cmap, const char *name)
{
    XColor color;
    XParseColor(dpy, cmap, name, &color);
    XAllocColor(dpy, cmap, &color);
    return color.pixel;
}

int XSetBackground(Display *dpy, GC gc, unsigned long background)
{
    XColor color;
    XGCValues *values;

    values = XGetGCValues_(gc);
    if (values == NULL)
        return BadGC;

    values->background = background;
    color.pixel = background;
    XQueryColor(dpy, DefaultColormap(dpy, DefaultScreenOfDisplay(dpy)), &color);
    values->background_rgb = RGB(color.red / 256, color.green / 256, color.blue / 256);
    return 0;
}

unsigned long window_background = 0;

int XSetWindowBackground(Display *dpy, Window w, unsigned long pixel)
{
    window_background = pixel;
    return 0;
}
