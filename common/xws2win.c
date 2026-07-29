#include "xws2win.h"
#include "colors.h"
#include <ctype.h>

void *g_gdipm = NULL;

//////////////////////////////////////////////////////////////////////////////

GC XCreateGC(Display *dpy, Drawable d,
     unsigned long valuemask, XGCValues *values)
{
    XGCValues *newvalues;

    newvalues = (XGCValues *)calloc(1, sizeof(XGCValues));
    if (newvalues == NULL)
        return NULL;

    newvalues->foreground = 0;
    newvalues->foreground_rgb = RGB(0, 0, 0);
    newvalues->background = 255;
    newvalues->background_rgb = RGB(255, 255, 255);
    newvalues->function = GXcopy;
    newvalues->fill_style = FillSolid;
    newvalues->cap_style = CapButt;
    newvalues->join_style = JoinMiter;
    newvalues->line_style = LineSolid;
    newvalues->fill_rule = EvenOddRule;
    newvalues->graphics_exposures = True;
    newvalues->font = (HFONT)GetStockObject(ANSI_FIXED_FONT);
    newvalues->stipple = NULL;
    newvalues->cached_pen = NULL;
    newvalues->cached_brush = NULL;
    newvalues->cached_pen_fg = 0;
    newvalues->cached_brush_fg = 0;
    newvalues->cached_pen_attrs = 0;
    newvalues->cached_brush_style = -1;
    if (values != NULL)
        XChangeGC(dpy, newvalues, valuemask, values);

    return newvalues;
}

int XChangeGC(Display* dpy, GC gc, unsigned long valuemask, XGCValues* values)
{
    XColor color;
    XGCValues* newvalues = XGetGCValues_(gc);
    if (newvalues == NULL)
        return BadGC;

    if (valuemask & GCForeground)
    {
        color.pixel = newvalues->foreground = values->foreground;
        XQueryColor(dpy, DefaultColormap(dpy, DefaultScreenOfDisplay(dpy)),
                    &color);
        newvalues->foreground_rgb =
            RGB(color.red / 256, color.green / 256, color.blue / 256);
        /* invalidate pen/brush cache */
        if (newvalues->cached_pen)
        {
            DeleteObject(newvalues->cached_pen);
            newvalues->cached_pen = NULL;
        }
        if (newvalues->cached_brush)
        {
            DeleteObject(newvalues->cached_brush);
            newvalues->cached_brush = NULL;
        }
    }

    if (valuemask & GCBackground)
    {
        color.pixel = newvalues->background = values->background;
        XQueryColor(dpy, DefaultColormap(dpy, DefaultScreenOfDisplay(dpy)),
                    &color);
        newvalues->background_rgb =
            RGB(color.red / 256, color.green / 256, color.blue / 256);
        /* background affects stipple brush */
        if (newvalues->cached_brush)
        {
            DeleteObject(newvalues->cached_brush);
            newvalues->cached_brush = NULL;
        }
    }

    if (valuemask & GCFillStyle)
    {
        newvalues->fill_style = values->fill_style;
        if (newvalues->cached_brush)
        {
            DeleteObject(newvalues->cached_brush);
            newvalues->cached_brush = NULL;
        }
    }

    if (valuemask & GCFunction)
        newvalues->function = values->function;

    if (valuemask & GCLineWidth)
    {
        newvalues->line_width = values->line_width;
        if (newvalues->cached_pen)
        {
            DeleteObject(newvalues->cached_pen);
            newvalues->cached_pen = NULL;
        }
    }

    if (valuemask & GCCapStyle)
    {
        newvalues->cap_style = values->cap_style;
        if (newvalues->cached_pen)
        {
            DeleteObject(newvalues->cached_pen);
            newvalues->cached_pen = NULL;
        }
    }

    if (valuemask & GCJoinStyle)
    {
        newvalues->join_style = values->join_style;
        if (newvalues->cached_pen)
        {
            DeleteObject(newvalues->cached_pen);
            newvalues->cached_pen = NULL;
        }
    }

    if (valuemask & GCLineStyle)
    {
        newvalues->line_style = values->line_style;
        if (newvalues->cached_pen)
        {
            DeleteObject(newvalues->cached_pen);
            newvalues->cached_pen = NULL;
        }
    }

    if (valuemask & GCStipple)
    {
        newvalues->stipple = values->stipple;
        if (newvalues->cached_brush)
        {
            DeleteObject(newvalues->cached_brush);
            newvalues->cached_brush = NULL;
        }
    }

    if (valuemask & GCFillRule)
        newvalues->fill_rule = values->fill_rule;

    if (valuemask & GCGraphicsExposures)
        newvalues->graphics_exposures = values->graphics_exposures;

    if (valuemask & GCFont)
        newvalues->font = values->font;

    if (valuemask & GCClipXOrigin)
        newvalues->clip_x_origin = values->clip_x_origin;

    if (valuemask & GCClipYOrigin)
        newvalues->clip_y_origin = values->clip_y_origin;

    if (valuemask & GCClipMask)
        newvalues->clip_mask_region = values->clip_mask_region;

    return 0;
}

int XFreeGC(Display *dpy, GC gc)
{
    XGCValues* values = XGetGCValues_(gc);
    if (values == NULL)
        return BadGC;

    if (values->cached_pen)
        DeleteObject(values->cached_pen);
    if (values->cached_brush)
        DeleteObject(values->cached_brush);

    if (values->clip_mask_region)
        DeleteObject(values->clip_mask_region);
    free(values);
    return 0;
}

/* ------------------------------------------------------------ */
/* Pen / Brush cache helpers                                    */
/* ------------------------------------------------------------ */

static unsigned int MakePenAttrs(const XGCValues *v)
{
    /* pack line_width (low 16) + style bits */
    return (v->line_width & 0xFFFF) |
           ((unsigned)v->line_style << 16) |
           ((unsigned)v->cap_style  << 20) |
           ((unsigned)v->join_style << 24);
}

HPEN GetCachedPen(XGCValues *v)
{
    unsigned int attrs = MakePenAttrs(v);

    if (v->cached_pen &&
        v->cached_pen_fg == v->foreground &&
        v->cached_pen_attrs == attrs)
    {
        return v->cached_pen;
    }

    if (v->cached_pen)
        DeleteObject(v->cached_pen);

    v->cached_pen = XCreateWinPen_(v);
    v->cached_pen_fg = v->foreground;
    v->cached_pen_attrs = attrs;
    return v->cached_pen;
}

HBRUSH GetCachedBrush(XGCValues *v)
{
    if (v->cached_brush &&
        v->cached_brush_fg == v->foreground &&
        v->cached_brush_style == v->fill_style)
    {
        return v->cached_brush;
    }

    if (v->cached_brush)
        DeleteObject(v->cached_brush);

    v->cached_brush = XCreateWinBrush_(v);
    v->cached_brush_fg = v->foreground;
    v->cached_brush_style = v->fill_style;
    return v->cached_brush;
}

static void get_2_skewed_angles(double *skewed1, double *skewed2,
    int width, int height, int angle1, int angle2)
{
#define M_PI_PER_180_PER_64 (M_PI / 180.0 / 64.0)
#define M_PI_3_PER_2 (3.0 * M_PI / 2.0)
    double radian1, radian2;

    angle1 = angle1 % (360 * 64);
    angle2 = angle2 % (360 * 64);
    angle2 += angle1;
    radian1 = angle1 * M_PI_PER_180_PER_64;
    radian2 = angle2 * M_PI_PER_180_PER_64;
    *skewed1 = atan(tan(radian1) * width / height);
    if (M_PI_2 <= radian1 && radian1 < M_PI_3_PER_2)
        *skewed1 += M_PI;
    else if (M_PI_3_PER_2 <= radian1 && radian1 <= M_2_PI)
        *skewed1 += 2.0 * M_PI;
    *skewed2 = atan(tan(radian2) * width / height);
    if (M_PI_2 <= radian2 && radian2 < M_PI_3_PER_2)
        *skewed2 += M_PI;
    else if (M_PI_3_PER_2 <= radian2 && radian2 <= M_2_PI)
        *skewed2 += M_2_PI;
}

int XSetLineAttributes(Display *dpy, GC gc,
    unsigned int line_width, int line_style,
    int cap_style, int join_style)
{
    XGCValues *values;

    values = XGetGCValues_(gc);
    if (values == NULL)
        return BadGC;

    values->line_width = line_width;
    values->line_style = line_style;
    values->cap_style = cap_style;
    values->join_style = join_style;

    if (values->cached_pen)
    {
        DeleteObject(values->cached_pen);
        values->cached_pen = NULL;
    }
    return 0;
}

int XDrawPoint(Display *dpy, Drawable d, GC gc,
    int x, int y)
{
    XGCValues *values;
    HDC hdc;
    COLORREF rgb;

    values = XGetGCValues_(gc);
    if (values == NULL)
        return BadGC;

    hdc = XCreateDrawableDC_(dpy, d);
    rgb = values->foreground_rgb;
    SetPixelV(hdc, x, y, rgb);
    XDeleteDrawableDC_(dpy, d, hdc);
    return 0;
}

int XDrawPoints(Display *dpy, Drawable d, GC gc,
    XPoint *points, int npoints, int CoordMode)
{
    XGCValues *values;
    HDC hdc;
    COLORREF rgb;
    int i;

    values = XGetGCValues_(gc);
    if (values == NULL)
        return BadGC;

    hdc = XCreateDrawableDC_(dpy, d);
    rgb = values->foreground_rgb;
    if (CoordMode == CoordModeOrigin)
    {
        for (i = 0; i < npoints; i++)
        {
            SetPixelV(hdc, points[i].x, points[i].y, rgb);
        }
    }
    else
    {
        int x = points[0].x, y = points[0].y;
        SetPixelV(hdc, x, y, rgb);
        for (i = 1; i < npoints; i++)
        {
            x += points[i].x;
            y += points[i].y;
            SetPixelV(hdc, x, y, rgb);
        }
    }
    XDeleteDrawableDC_(dpy, d, hdc);
    return 0;
}

HPEN XCreateWinPen_(XGCValues *values)
{
    LOGBRUSH lb;
    lb.lbStyle = BS_SOLID;
    lb.lbColor = values->foreground_rgb;
    return ExtCreatePen(
        PS_GEOMETRIC | values->line_style | values->cap_style | values->join_style,
        values->line_width, &lb, 0, NULL);
}

typedef struct tagBITMAPINFOEX
{
    BITMAPINFOHEADER bmiHeader;
    RGBQUAD          bmiColors[256];
} BITMAPINFOEX, *LPBITMAPINFOEX;

#define WIDTHBYTES(i) (((i) + 31) / 32 * 4)

LPVOID XCreatePackedDIBFromPixmap_(Pixmap pixmap, COLORREF clrFore, COLORREF clrBack)
{
    BITMAP bm;
    LPBYTE pb;
    HDC hdc;
    DWORD size = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 2;
    LPBITMAPINFOEX lpbi = (LPBITMAPINFOEX)calloc(1, size);
    assert(lpbi != NULL);

    GetObject(pixmap->hbm, sizeof(BITMAP), &bm);
    lpbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    lpbi->bmiHeader.biWidth = bm.bmWidth;
    lpbi->bmiHeader.biHeight = bm.bmHeight;
    lpbi->bmiHeader.biPlanes = 1;
    lpbi->bmiHeader.biBitCount = 1;
    lpbi->bmiHeader.biSizeImage = WIDTHBYTES(bm.bmWidth * 1) * bm.bmHeight;
    lpbi->bmiColors[0].rgbBlue = GetBValue(clrBack);
    lpbi->bmiColors[0].rgbGreen = GetGValue(clrBack);
    lpbi->bmiColors[0].rgbRed = GetRValue(clrBack);
    lpbi->bmiColors[0].rgbReserved = 0;
    lpbi->bmiColors[1].rgbBlue = GetBValue(clrFore);
    lpbi->bmiColors[1].rgbGreen = GetGValue(clrFore);
    lpbi->bmiColors[1].rgbRed = GetRValue(clrFore);
    lpbi->bmiColors[1].rgbReserved = 0;

    size += lpbi->bmiHeader.biSizeImage;
    lpbi = (LPBITMAPINFOEX)realloc(lpbi, size);
    assert(lpbi != NULL);

    pb = (LPBYTE)lpbi;
    pb += sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 2;
    hdc = CreateCompatibleDC(NULL);
    GetDIBits(hdc, pixmap->hbm, 0, bm.bmHeight, pb, (LPBITMAPINFO)lpbi, DIB_PAL_COLORS);
    DeleteDC(hdc);
    return lpbi;
}

HBRUSH XCreateWinBrush_(XGCValues *values)
{
    HBRUSH hbr;
    LOGBRUSH lb;
    if (values->fill_style == FillSolid)
    {
        return CreateSolidBrush(values->foreground_rgb);
    }
    else if (values->fill_style == FillStippled)
    {
        LPVOID lpPackedDIB;
        assert(values->stipple != NULL);
        lpPackedDIB = XCreatePackedDIBFromPixmap_(
            values->stipple, values->background_rgb, values->foreground_rgb);
        assert(lpPackedDIB != NULL);
        hbr = CreateDIBPatternBrushPt(lpPackedDIB, DIB_RGB_COLORS);
        free(lpPackedDIB);
        return hbr;
    }
    return NULL;
}

int XDrawLine(Display *dpy, Drawable d, GC gc,
    int x1, int y1, int x2, int y2)
{
    XGCValues *values;
    HDC hdc;
    HPEN hPen;
    HGDIOBJ hPenOld;
    int nR2;

    values = XGetGCValues_(gc);
    if (values == NULL)
        return BadGC;

    hPen = GetCachedPen(values);
    assert(hPen);
    if (hPen == NULL)
        return BadAlloc;

    hdc = XCreateDrawableDC_(dpy, d);
    nR2 = SetROP2(hdc, values->function);
    hPenOld = SelectObject(hdc, hPen);
    MoveToEx(hdc, x1, y1, NULL);
    LineTo(hdc, x2, y2);
    if (values->line_width <= 1 && values->function == GXcopy)
        SetPixelV(hdc, x2, y2, values->foreground_rgb);
    SelectObject(hdc, hPenOld);
    SetROP2(hdc, nR2);
    XDeleteDrawableDC_(dpy, d, hdc);

    return 0;
}

int XDrawLines(Display *dpy, Drawable d, GC gc,
    XPoint *points, int npoints, int mode)
{
    XGCValues *values;
    LPPOINT lpPoints = NULL;
    HDC hdc;
    HPEN hPen;
    HGDIOBJ hPenOld;
    int i, x, y;
    int nR2;

    values = XGetGCValues_(gc);
    if (values == NULL)
        return BadGC;

    lpPoints = (LPPOINT)calloc(npoints, sizeof(POINT));
    if (lpPoints == NULL)
        return BadAlloc;

    hPen = GetCachedPen(values);
    assert(hPen);
    if (hPen == NULL)
    {
        free(lpPoints);
        return BadAlloc;
    }

    if (mode == CoordModeOrigin)
    {
        for (i = 0; i < npoints; i++)
        {
            lpPoints[i].x = points[i].x;
            lpPoints[i].y = points[i].y;
        }
    }
    else
    {
        lpPoints[0].x = x = points[0].x;
        lpPoints[0].y = y = points[0].y;
        for (i = 1; i < npoints; i++)
        {
            x += points[i].x;
            y += points[i].y;
            lpPoints[i].x = x;
            lpPoints[i].y = y;
        }
    }

    hdc = XCreateDrawableDC_(dpy, d);
    nR2 = SetROP2(hdc, values->function);
    hPenOld = SelectObject(hdc, hPen);
    Polyline(hdc, lpPoints, npoints);
    free(lpPoints);
    SelectObject(hdc, hPenOld);
    SetROP2(hdc, nR2);
    XDeleteDrawableDC_(dpy, d, hdc);

    return 0;
}

int XDrawSegments(Display *dpy, Drawable d, GC gc,
    XSegment *segments, int nsegments)
{
    XGCValues *values;
    HDC hdc;
    HPEN hPen;
    HGDIOBJ hPenOld;
    int i;
    int nR2;

    values = XGetGCValues_(gc);
    if (values == NULL)
        return BadGC;

    hPen = GetCachedPen(values);
    assert(hPen);
    if (hPen == NULL)
        return BadAlloc;

    hdc = XCreateDrawableDC_(dpy, d);
    nR2 = SetROP2(hdc, values->function);
    hPenOld = SelectObject(hdc, hPen);
    for (i = 0; i < nsegments; i++)
    {
        MoveToEx(hdc, segments[i].x1, segments[i].y1, NULL);
        LineTo(hdc, segments[i].x2, segments[i].y2);
    }
    SelectObject(hdc, hPenOld);
    SetROP2(hdc, nR2);
    XDeleteDrawableDC_(dpy, d, hdc);

    return 0;
}

int XDrawArc(Display *dpy, Drawable d, GC gc,
    int x, int y, unsigned int width, unsigned int height,
    int angle1, int angle2)
{
    XGCValues *values;
    HDC hdc;
    HPEN hPen;
    HGDIOBJ hPenOld;
    double skewed1, skewed2;
    int xStartArc, yStartArc, xEndArc, yEndArc;
    int nR2;

    values = XGetGCValues_(gc);
    if (values == NULL)
        return BadGC;

    hPen = GetCachedPen(values);
    assert(hPen);
    if (hPen == NULL)
        return BadAlloc;

    get_2_skewed_angles(&skewed1, &skewed2, width, height, angle1, angle2);
    xStartArc = x + width / 2.0 + (width / 2.0) * cos(skewed1);
    yStartArc = y + height / 2.0 + (height / 2.0) * sin(skewed1);
    xEndArc = x + width / 2.0 + (width / 2.0) * cos(skewed2);
    yEndArc = y + height / 2.0 + (height / 2.0) * sin(skewed2);

    hdc = XCreateDrawableDC_(dpy, d);
    nR2 = SetROP2(hdc, values->function);
    hPenOld = SelectObject(hdc, hPen);
    SelectObject(hdc, GetStockObject(NULL_BRUSH));

    if (angle2 < 0)
        SetArcDirection(hdc, AD_CLOCKWISE);
    else
        SetArcDirection(hdc, AD_COUNTERCLOCKWISE);
    Arc(hdc, x, y, x + width, y + height,
        xStartArc, yStartArc, xEndArc, yEndArc);

    SelectObject(hdc, hPenOld);
    SetROP2(hdc, nR2);
    XDeleteDrawableDC_(dpy, d, hdc);

    return 0;
}

int XDrawArcs(Display *dpy, Drawable d, GC gc,
    XArc *arcs, int n_arcs)
{
    XGCValues *values;
    HDC hdc;
    HPEN hPen;
    HGDIOBJ hPenOld;
    int i, x, y;
    unsigned int width, height;
    short angle1, angle2;
    double skewed1, skewed2;
    int xStartArc, yStartArc, xEndArc, yEndArc;
    int nR2;

    values = XGetGCValues_(gc);
    if (values == NULL)
        return BadGC;

    hPen = GetCachedPen(values);
    if (hPen == NULL)
        return BadAlloc;

    hdc = XCreateDrawableDC_(dpy, d);
    nR2 = SetROP2(hdc, values->function);
    hPenOld = SelectObject(hdc, hPen);
    SelectObject(hdc, GetStockObject(NULL_BRUSH));
    for (i = 0; i < n_arcs; i++)
    {
        x = arcs[i].x; y = arcs[i].y;
        width = arcs[i].width; height = arcs[i].height;
        angle1 = arcs[i].angle1; angle2 = arcs[i].angle2;

        get_2_skewed_angles(&skewed1, &skewed2, width, height, angle1, angle2);
        xStartArc = x + width / 2.0 + (width / 2.0) * cos(skewed1);
        yStartArc = y + height / 2.0 + (height / 2.0) * sin(skewed1);
        xEndArc = x + width / 2.0 + (width / 2.0) * cos(skewed2);
        yEndArc = y + height / 2.0 + (height / 2.0) * sin(skewed2);

        if (angle2 < 0)
            SetArcDirection(hdc, AD_CLOCKWISE);
        else
            SetArcDirection(hdc, AD_COUNTERCLOCKWISE);

        Arc(hdc, x, y, x + width, y + height,
            xStartArc, yStartArc, xEndArc, yEndArc);
    }
    SelectObject(hdc, hPenOld);
    SetROP2(hdc, nR2);
    XDeleteDrawableDC_(dpy, d, hdc);

    return 0;
}

int XDrawString(Display *dpy, Drawable d, GC gc,
    int x, int y, const char *string, int length)
{
    XGCValues *values;
    HDC hdc;
    HGDIOBJ hFontOld;
    TEXTMETRICA tm;

    values = XGetGCValues_(gc);
    if (values == NULL)
        return BadGC;

    hdc = XCreateDrawableDC_(dpy, d);
    SetTextColor(hdc, values->foreground_rgb);
    SetBkMode(hdc, TRANSPARENT);

    hFontOld = SelectObject(hdc, values->font);
    GetTextMetricsA(hdc, &tm);
    TextOut(hdc, x, y - tm.tmAscent, string, length);
    SelectObject(hdc, hFontOld);

    XDeleteDrawableDC_(dpy, d, hdc);

    return 0;
}

int XDrawImageString(Display *dpy, Drawable d, GC gc,
    int x, int y, const char *string, int length)
{
    XGCValues *values;
    HDC hdc;
    HGDIOBJ hFontOld;
    TEXTMETRICA tm;

    values = XGetGCValues_(gc);
    if (values == NULL)
        return BadGC;

    hdc = XCreateDrawableDC_(dpy, d);
    SetTextColor(hdc, values->foreground_rgb);
    SetBkColor(hdc, values->background_rgb);
    SetBkMode(hdc, OPAQUE);

    hFontOld = SelectObject(hdc, values->font);
    GetTextMetricsA(hdc, &tm);
    TextOut(hdc, x, y - tm.tmAscent, string, length);
    SelectObject(hdc, hFontOld);

    XDeleteDrawableDC_(dpy, d, hdc);

    return 0;
}

static void
_FillRectangle(HDC hdc, Drawable d, XGCValues *values, HBRUSH hbr, int nR2,
    int x, int y, unsigned int width, unsigned int height)
{
    RECT rc;

    if (values->clip_mask_region)
    {
        SelectClipRgn(hdc, values->clip_mask_region);
        OffsetClipRgn(hdc, values->clip_x_origin, values->clip_y_origin);
    }

    SetRect(&rc, x, y, x + width, y + height);
    FillRect(hdc, &rc, hbr);

    if (values->clip_mask_region)
        SelectClipRgn(hdc, NULL);
}

int XFillRectangle(
    Display *dpy, Drawable d, GC gc,
    int x, int y, unsigned int width, unsigned int height)
{
    XGCValues *values;
    HDC hdc;
    HBRUSH hbr;
    int nR2;

    values = XGetGCValues_(gc);
    hbr = GetCachedBrush(values);
    if (hbr == NULL)
        return BadAlloc;

    hdc = XCreateDrawableDC_(dpy, d);
    nR2 = SetROP2(hdc, values->function);
    SetPolyFillMode(hdc, (values->fill_rule == EvenOddRule ? ALTERNATE : WINDING));

    _FillRectangle(hdc, d, values, hbr, nR2, x, y, width, height);

    SetROP2(hdc, nR2);
    XDeleteDrawableDC_(dpy, d, hdc);
    return 0;
}

int XFillRectangles(
    Display *dpy, Drawable d, GC gc,
    XRectangle *rectangles, int n_rects)
{
    XGCValues *values;
    HDC hdc;
    HBRUSH hbr;
    int i, nR2;

    values = XGetGCValues_(gc);
    hbr = GetCachedBrush(values);
    if (hbr == NULL)
        return BadAlloc;

    hdc = XCreateDrawableDC_(dpy, d);
    nR2 = SetROP2(hdc, values->function);
    SetPolyFillMode(hdc, (values->fill_rule == EvenOddRule ? ALTERNATE : WINDING));

    for (i = 0; i < n_rects; ++i)
    {
        int x = rectangles[i].x;
        int y = rectangles[i].y;
        int width = rectangles[i].width;
        int height = rectangles[i].height;
        _FillRectangle(hdc, d, values, hbr, nR2, x, y, width, height);
    }

    SetROP2(hdc, nR2);
    XDeleteDrawableDC_(dpy, d, hdc);
    return 0;
}

int XFillPolygon(Display *dpy, Drawable d, GC gc,
    XPoint *points, int n_points, int shape, int mode)
{
    XGCValues *values;
    HDC hdc;
    HBRUSH hbr;
    HGDIOBJ hbrOld;
    LPPOINT lpPoints;
    int i, x, y;
    int nR2;

    values = XGetGCValues_(gc);
    if (values == NULL)
        return BadGC;

    lpPoints = calloc(n_points, sizeof(POINT));
    if (lpPoints == NULL)
        return BadAlloc;

    if (mode == CoordModeOrigin)
    {
        for (i = 0; i < n_points; i++)
        {
            lpPoints[i].x = points[i].x;
            lpPoints[i].y = points[i].y;
        }
    }
    else
    {
        lpPoints[0].x = x = points[0].x;
        lpPoints[0].y = y = points[0].y;
        for (i = 1; i < n_points; i++)
        {
            x += points[i].x;
            y += points[i].y;
            lpPoints[i].x = x;
            lpPoints[i].y = y;
        }
    }

    hbr = GetCachedBrush(values);
    if (hbr == NULL)
    {
        free(lpPoints);
        return BadAlloc;
    }

    hdc = XCreateDrawableDC_(dpy, d);
    nR2 = SetROP2(hdc, values->function);
    hbrOld = SelectObject(hdc, hbr);

    if (values->clip_mask_region)
    {
        SelectClipRgn(hdc, values->clip_mask_region);
        OffsetClipRgn(hdc, values->clip_x_origin, values->clip_y_origin);
    }

    SetPolyFillMode(hdc, (values->fill_rule == EvenOddRule ? ALTERNATE : WINDING));
    BeginPath(hdc);
    Polygon(hdc, lpPoints, n_points);
    EndPath(hdc);
    FillPath(hdc);

    if (values->clip_mask_region)
        SelectClipRgn(hdc, NULL);

    SelectObject(hdc, hbrOld);
    SetROP2(hdc, nR2);
    XDeleteDrawableDC_(dpy, d, hdc);

    free(lpPoints);
    return 0;
}

int XFillArc(Display *dpy, Drawable d, GC gc,
    int x, int y, unsigned int width, unsigned int height,
    int angle1, int angle2)
{
    XGCValues *values;
    HDC hdc;
    HBRUSH hbr;
    HGDIOBJ hbrOld;
    double skewed1, skewed2;
    int xStartArc, yStartArc, xEndArc, yEndArc;
    int nR2;

    values = XGetGCValues_(gc);
    if (values == NULL)
        return BadGC;

    hbr = GetCachedBrush(values);
    if (hbr == NULL)
        return BadAlloc;

    get_2_skewed_angles(&skewed1, &skewed2, width, height, angle1, angle2);
    xStartArc = x + width / 2.0 + (width / 2.0) * cos(skewed1);
    yStartArc = y + height / 2.0 + (height / 2.0) * sin(skewed1);
    xEndArc = x + width / 2.0 + (width / 2.0) * cos(skewed2);
    yEndArc = y + height / 2.0 + (height / 2.0) * sin(skewed2);

    hdc = XCreateDrawableDC_(dpy, d);
    nR2 = SetROP2(hdc, values->function);
    hbrOld = SelectObject(hdc, hbr);
    SetPolyFillMode(hdc, (values->fill_rule == EvenOddRule ? ALTERNATE : WINDING));

    BeginPath(hdc);
    if (angle2 < 0)
        SetArcDirection(hdc, AD_CLOCKWISE);
    else
        SetArcDirection(hdc, AD_COUNTERCLOCKWISE);
    Arc(hdc, x, y, x + width, y + height,
        xStartArc, yStartArc, xEndArc, yEndArc);
    EndPath(hdc);
    FillPath(hdc);

    SelectObject(hdc, hbrOld);
    SetROP2(hdc, nR2);
    XDeleteDrawableDC_(dpy, d, hdc);

    return 0;
}

int XFillArcs(Display *dpy, Drawable d, GC gc,
    XArc *arcs, int n_arcs)
{
    XGCValues *values;
    HDC hdc;
    HBRUSH hbr;
    HGDIOBJ hbrOld;
    int i, x, y;
    unsigned int width, height;
    short angle1, angle2;
    double skewed1, skewed2;
    int xStartArc, yStartArc, xEndArc, yEndArc;
    int nR2;

    values = XGetGCValues_(gc);
    if (values == NULL)
        return BadGC;

    hbr = GetCachedBrush(values);
    if (hbr == NULL)
        return BadAlloc;

    hdc = XCreateDrawableDC_(dpy, d);
    nR2 = SetROP2(hdc, values->function);
    hbrOld = SelectObject(hdc, hbr);
    SetPolyFillMode(hdc, (values->fill_rule == EvenOddRule ? ALTERNATE : WINDING));
    for (i = 0; i < n_arcs; i++)
    {
        x = arcs[i].x; y = arcs[i].y;
        width = arcs[i].width; height = arcs[i].height;
        angle1 = arcs[i].angle1; angle2 = arcs[i].angle2;

        get_2_skewed_angles(&skewed1, &skewed2, width, height, angle1, angle2);
        xStartArc = x + width / 2.0 + (width / 2.0) * cos(skewed1);
        yStartArc = y + height / 2.0 + (height / 2.0) * sin(skewed1);
        xEndArc = x + width / 2.0 + (width / 2.0) * cos(skewed2);
        yEndArc = y + height / 2.0 + (height / 2.0) * sin(skewed2);

        BeginPath(hdc);
        if (angle2 < 0)
            SetArcDirection(hdc, AD_CLOCKWISE);
        else
            SetArcDirection(hdc, AD_COUNTERCLOCKWISE);
        Arc(hdc, x, y, x + width, y + height,
            xStartArc, yStartArc, xEndArc, yEndArc);
        EndPath(hdc);
        FillPath(hdc);
    }
    SelectObject(hdc, hbrOld);
    SetROP2(hdc, nR2);
    XDeleteDrawableDC_(dpy, d, hdc);

    return 0;
}

int XCopyArea(Display *dpy,
     Drawable src_drawable, Drawable dst_drawable, GC gc,
     int src_x, int src_y,
     unsigned int width, unsigned int height,
     int dst_x, int dst_y)
{
    XGCValues *values;
    if (src_drawable == dst_drawable)
    {
        HDC hdc = XCreateDrawableDC_(dpy, dst_drawable);
        values = XGetGCValues_(gc);
        if (values && values->clip_mask_region)
        {
            SelectClipRgn(hdc, values->clip_mask_region);
            OffsetClipRgn(hdc, values->clip_x_origin, values->clip_y_origin);
        }
        BitBlt(hdc, dst_x, dst_y, width, height, hdc, src_x, src_y, SRCCOPY);
        if (values && values->clip_mask_region)
            SelectClipRgn(hdc, NULL);
        XDeleteDrawableDC_(dpy, dst_drawable, hdc);
    }
    else
    {
        HDC hdcSrc = XCreateDrawableDC_(dpy, src_drawable);
        HDC hdcDst = XCreateDrawableDC_(dpy, dst_drawable);
        values = XGetGCValues_(gc);
        if (values->clip_mask_region)
        {
            SelectClipRgn(hdcDst, values->clip_mask_region);
            OffsetClipRgn(hdcDst, values->clip_x_origin, values->clip_y_origin);
        }
        BitBlt(hdcDst, dst_x, dst_y, width, height, hdcSrc, src_x, src_y, SRCCOPY);
        if (values->clip_mask_region)
            SelectClipRgn(hdcDst, NULL);
        XDeleteDrawableDC_(dpy, src_drawable, hdcSrc);
        XDeleteDrawableDC_(dpy, dst_drawable, hdcDst);
    }
    return 0;
}

int XSync(Display *dpy, Bool b)
{
    GdiFlush();
    return 0;
}

int XSetFunction(Display *dpy, GC gc, int function)
{
    XGCValues *values;

    values = XGetGCValues_(gc);
    if (values == NULL)
        return BadGC;

    values->function = function;
    return 0;
}

int XSetFillStyle(Display *dpy, GC gc, int fill)
{
    XGCValues *values;

    values = XGetGCValues_(gc);
    if (values == NULL)
        return BadGC;

    values->fill_style = fill;

    if (values->cached_brush)
    {
        DeleteObject(values->cached_brush);
        values->cached_brush = NULL;
    }

    return 0;
}

Bool XQueryPointer(Display *dpy, Window w, Window *root, Window *child,
     int *root_x, int *root_y, int *win_x, int *win_y,
     unsigned int *mask)
{
    *root = *child = NULL;
    *root_x = *root_y = 0;
    *win_x = *win_y = 0;
    *mask = 0;
    return True;
}

int XSetGraphicsExposures(Display *dpy, GC gc, Bool graphics_exposures)
{
    XGCValues *values;

    values = XGetGCValues_(gc);
    if (values == NULL)
        return BadGC;

    values->graphics_exposures = graphics_exposures;
    return 0;
}

int XClearArea(
    Display *dpy, Window w,
    int x, int y, unsigned int width, unsigned int height,
    Bool exposures)
{
    RECT rc;
    HDC hdc = XCreateDrawableDC_(dpy, w);
    SetRect(&rc, x, y, x + width, y + height);
    FillRect(hdc, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
    XDeleteDrawableDC_(dpy, w, hdc);
    return 0;
}

int XClearArea2_(
    Display *dpy, Window w, GC gc,
    int x, int y, unsigned int width, unsigned int height,
    Bool exposures)
{
    RECT rc;
    HBRUSH hbr = CreateSolidBrush(gc->background_rgb);
    HDC hdc = XCreateDrawableDC_(dpy, w);
    SetRect(&rc, x, y, x + width, y + height);
    FillRect(hdc, &rc, hbr);
    XDeleteDrawableDC_(dpy, w, hdc);
    DeleteObject(hbr);
    return 0;
}

int XSetSubwindowMode(Display *dpy, GC gc, int mode)
{
    XGCValues *values;

    values = XGetGCValues_(gc);
    if (values == NULL)
        return BadGC;

    values->subwindow_mode = mode;
    return 0;
}

Window RootWindow(Display *dpy, int scr)
{
    return 0;
}

Bool XTranslateCoordinates(
     Display *dpy, Window src_win, Window dest_win,
     int src_x, int src_y, int *dst_x, int *dst_y, Window *child)
{
    *dst_x = src_x;
    *dst_y = src_y;
    *child = 0;
    return True;
}

int XSetPlaneMask(Display *dpy, GC gc, unsigned long planemask)
{
    return 1;
}

int XSetClipOrigin(Display *dpy, GC gc, int xorig, int yorig)
{
    XGCValues* values = XGetGCValues_(gc);
    if (values)
    {
        values->clip_x_origin = xorig;
        values->clip_y_origin = yorig;
    }
    return 1;
}

int XSetClipMask(Display *dpy, GC gc, Pixmap mask)
{
    if (mask == NULL)
    {
        if (gc->clip_mask_region)
            DeleteObject(gc->clip_mask_region);
        gc->clip_mask_region = NULL;
    }
    else
    {
        INT x, y;
        BITMAP bm;
        LPBYTE pb;
        LPRGNDATA prd;
        DWORD size, i, nCount = 0;
        LPRECT pRects;
        HRGN hRgn;
        int height;

        GetObject(mask->hbm, sizeof(bm), &bm);
        assert(bm.bmBitsPixel == 32);

        /* CreateDIBSection uses top-down DIBs (negative biHeight).
           GetObject may report bmHeight as negative. */
        height = (bm.bmHeight < 0) ? -bm.bmHeight : bm.bmHeight;
        pb = mask->pbBits ? mask->pbBits : (LPBYTE)bm.bmBits;
        if (pb == NULL || height <= 0 || bm.bmWidth <= 0)
        {
            if (gc->clip_mask_region)
                DeleteObject(gc->clip_mask_region);
            gc->clip_mask_region = NULL;
            return 1;
        }

        for (y = 0; y < height; y++)
        {
            for (x = 0; x < bm.bmWidth; x++)
            {
                BYTE *p = pb + y * bm.bmWidthBytes + x * 4;
                if (p[0] | p[1] | p[2])
                    nCount++;
            }
        }

        if (nCount == 0)
        {
            if (gc->clip_mask_region)
                DeleteObject(gc->clip_mask_region);
            gc->clip_mask_region = CreateRectRgn(0, 0, 0, 0);
            return 1;
        }

        size = sizeof(RGNDATAHEADER) + nCount * sizeof(RECT);
        prd = (LPRGNDATA)calloc(1, size);
        assert(prd != NULL);
        prd->rdh.dwSize = sizeof(RGNDATAHEADER);
        prd->rdh.iType = RDH_RECTANGLES;
        prd->rdh.nCount = nCount;
        prd->rdh.nRgnSize = 0;
        SetRect(&prd->rdh.rcBound, 0, 0, bm.bmWidth, height);
        pRects = (LPRECT)((LPBYTE)prd + sizeof(RGNDATAHEADER));
        i = 0;
        for (y = 0; y < height; y++)
        {
            for (x = 0; x < bm.bmWidth;)
            {
                BYTE *p = pb + y * bm.bmWidthBytes + x * 4;
                if (p[0] | p[1] | p[2])
                {
                    pRects->left = x;
                    pRects->right = ++x;
                    pRects->top = y;
                    pRects->bottom = y + 1;
                    pRects++;
                    i++;
                }
                else
                    x++;
            }
        }
        prd->rdh.nCount = i;
        hRgn = ExtCreateRegion(NULL, size, prd);
        free(prd);
        if (hRgn == NULL)
            hRgn = CreateRectRgn(0, 0, bm.bmWidth, height);

        if (gc->clip_mask_region)
            DeleteObject(gc->clip_mask_region);
        gc->clip_mask_region = hRgn;
    }
    return 1;
}

//////////////////////////////////////////////////////////////////////////////

/* Parse XLFD: -foundry-family-weight-slant-setwidth-addstyle-pixel-point-resx-resy-spacing-avg-registry-encoding
   Extracts family (field 2) and pixel size (field 7). Returns 1 if name is XLFD. */
static int parse_xlfd(const char *name, char *family_out, int family_size, int *pixel_out,
                      int *bold_out, int *italic_out)
{
    char buf[512];
    char *fields[15];
    int n = 0;
    char *p;

    if (!name || name[0] != '-')
        return 0;

    lstrcpynA(buf, name, sizeof(buf));
    p = buf;
    /* Leading '-' means fields[0] is empty after first split; skip leading dash */
    if (*p == '-')
        p++;
    fields[n++] = p;
    while (*p && n < 15)
    {
        if (*p == '-')
        {
            *p++ = '\0';
            fields[n++] = p;
        }
        else
            p++;
    }

    /* Need at least foundry, family, weight, slant, ..., pixel */
    if (n < 7)
        return 0;

    lstrcpynA(family_out, fields[1], family_size);
    if (pixel_out)
    {
        if (fields[6][0] == '*' || fields[6][0] == '\0')
            *pixel_out = 0;
        else
            *pixel_out = (int)strtol(fields[6], NULL, 10);
    }
    if (bold_out)
        *bold_out = (fields[2][0] &&
                     (_stricmp(fields[2], "bold") == 0 ||
                      _stricmp(fields[2], "black") == 0 ||
                      _stricmp(fields[2], "demibold") == 0));
    if (italic_out)
        *italic_out = (fields[3][0] == 'i' || fields[3][0] == 'I' ||
                       fields[3][0] == 'o' || fields[3][0] == 'O');
    return 1;
}

XFontStruct *XLoadQueryFont(Display *dpy, const char *name)
{
    LOGFONTA lf;
    HDC hdc;
    TEXTMETRICA tm;
    ABC *pabc;
    HGDIOBJ hFontOld;
    int i, nCount;
    char *p, *q;
    int xlfd_pixel = 0;
    int xlfd_bold = 0;
    int xlfd_italic = 0;
    XFontStruct *fs = (XFontStruct *)calloc(1, sizeof(XFontStruct));
    assert(fs);
    if (fs == NULL)
        return NULL;

    hdc = CreateCompatibleDC(NULL);

    ZeroMemory(&lf, sizeof(lf));
    fprintf(stderr, "Loading font: %s\n", name ? name : "(null)");

    if (name && parse_xlfd(name, lf.lfFaceName, LF_FACESIZE, &xlfd_pixel,
                           &xlfd_bold, &xlfd_italic))
    {
        /* XLFD path: family + pixel size from the pattern */
        if (xlfd_pixel > 0)
            lf.lfHeight = -xlfd_pixel; /* character cell height in pixels */
        else
            lf.lfHeight = -MulDiv(24, GetDeviceCaps(hdc, LOGPIXELSY), 72);
        lf.lfWeight = xlfd_bold ? FW_BOLD : FW_NORMAL;
        lf.lfItalic = xlfd_italic ? TRUE : FALSE;
        fprintf(stderr, "XLFD family='%s' pixel=%d bold=%d italic=%d height=%d\n",
                lf.lfFaceName, xlfd_pixel, xlfd_bold, xlfd_italic, lf.lfHeight);
    }
    else
    {
        /* Tokenize "FaceName [Bold] [Italic] [Size]" style names.
           The size, weight and style words can appear in any order
           (e.g. "Arial Bold 12", "Arial 12 Bold", "Arial Italic Bold 12"),
           so scan every space-separated token instead of only looking at
           the last one; otherwise a trailing style word after the size
           hides the size, and italic is never recognized at all. */
        char *tokens[32];
        int ntok = 0;
        int size_idx = -1;
        int bold_idx = -1;
        int italic_idx = -1;
        int k;
        char out[LF_FACESIZE];
        char *dst = out;
        int first = 1;

        lstrcpynA(lf.lfFaceName, name ? name : "Arial", LF_FACESIZE);

        p = lf.lfFaceName;
        while (ntok < 32)
        {
            while (*p == ' ')
                p++;
            if (*p == '\0')
                break;
            tokens[ntok++] = p;
            while (*p && *p != ' ')
                p++;
            if (*p == ' ')
                *p++ = '\0';
        }

        for (k = 0; k < ntok; k++)
        {
            if (_stricmp(tokens[k], "bold") == 0)
                bold_idx = k;
            else if (_stricmp(tokens[k], "italic") == 0 ||
                     _stricmp(tokens[k], "oblique") == 0)
                italic_idx = k;
            else
            {
                long n = strtoul(tokens[k], &q, 10);
                if (q && *q == '\0' && tokens[k][0] != '\0')
                    size_idx = k; /* keep the rightmost numeric token */
            }
        }

        if (size_idx >= 0)
        {
            long n = strtoul(tokens[size_idx], NULL, 10);
            lf.lfHeight = -MulDiv(n, GetDeviceCaps(hdc, LOGPIXELSY), 72);
            fprintf(stderr, "font size: %d\n", lf.lfHeight);
        }

        lf.lfWeight = (bold_idx >= 0) ? FW_BOLD : FW_NORMAL;
        lf.lfItalic = (italic_idx >= 0) ? TRUE : FALSE;
        if (bold_idx >= 0)
            fprintf(stderr, "font is bold\n");
        if (italic_idx >= 0)
            fprintf(stderr, "font is italic\n");

        /* Rebuild the face name from whatever tokens are left. */
        for (k = 0; k < ntok; k++)
        {
            size_t len;
            if (k == size_idx || k == bold_idx || k == italic_idx)
                continue;
            len = strlen(tokens[k]);
            if ((size_t)(dst - out) + len + 2 > sizeof(out))
                break;
            if (!first)
                *dst++ = ' ';
            memcpy(dst, tokens[k], len);
            dst += len;
            first = 0;
        }
        *dst = '\0';
        lstrcpynA(lf.lfFaceName, out[0] ? out : "Arial", LF_FACESIZE);
    }

    if (lf.lfHeight == 0)
        lf.lfHeight = -MulDiv(24, GetDeviceCaps(hdc, LOGPIXELSY), 72);

    lf.lfQuality = ANTIALIASED_QUALITY;
    lf.lfCharSet = DEFAULT_CHARSET;
    /* GetCharABCWidthsA (used below) only works on outline/scalable fonts.
       If GDI is allowed to substitute a raster (bitmap) font here, every
       per_char metric silently comes back as zero, which downstream
       collapses word pixmaps to 0x0 and crashes. Bias font selection
       toward TrueType/outline fonts to avoid that substitution. */
    lf.lfOutPrecision = OUT_TT_PRECIS;
    lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
    fs->fid = CreateFontIndirectA(&lf);
    assert(fs->fid);
    if (fs->fid == NULL)
    {
        DeleteDC(hdc);
        free(fs);
        return NULL;
    }

    fs->min_char_or_byte2 = 0;
    fs->max_char_or_byte2 = 255;
    nCount = (int)fs->max_char_or_byte2 - (int)fs->min_char_or_byte2 + 1;
    pabc = (ABC *)calloc(nCount, sizeof(ABC));
    assert(pabc);
    fs->per_char = (XCharStruct *)calloc(nCount, sizeof(XCharStruct));
    assert(fs->per_char);

    hFontOld = SelectObject(hdc, fs->fid);
    GetTextMetricsA(hdc, &tm);
    if (!GetCharABCWidthsA(hdc, fs->min_char_or_byte2, fs->max_char_or_byte2, pabc))
    {
        /* Not an outline font (GDI substituted a raster/bitmap font despite
           OUT_TT_PRECIS), or the call otherwise failed. Fall back to plain
           advance widths so lbearing==0, rbearing==width==advance instead
           of silently leaving every glyph at 0x0 (which crashes downstream
           when a 0-width word pixmap is created). */
        UINT i2, nCount2 = fs->max_char_or_byte2 - fs->min_char_or_byte2 + 1;
        INT *widths = (INT *)calloc(nCount2, sizeof(INT));
        fprintf(stderr,
            "%s: font has no ABC widths (bitmap font?), using advance widths\n",
            name ? name : "(null)");
        if (widths && GetCharWidth32A(hdc, fs->min_char_or_byte2,
                                       fs->max_char_or_byte2, widths))
        {
            for (i2 = 0; i2 < nCount2; i2++)
            {
                pabc[i2].abcA = 0;
                pabc[i2].abcB = widths[i2];
                pabc[i2].abcC = 0;
            }
        }
        else
        {
            /* Last resort: use the font's average/max char width so we at
               least never end up with a 0-width glyph. */
            for (i2 = 0; i2 < nCount2; i2++)
            {
                pabc[i2].abcA = 0;
                pabc[i2].abcB = (tm.tmAveCharWidth > 0
                                  ? tm.tmAveCharWidth : tm.tmMaxCharWidth);
                pabc[i2].abcC = 0;
            }
        }
        free(widths);
    }
    SelectObject(hdc, hFontOld);
    DeleteDC(hdc);

    fs->min_bounds.lbearing = 0x7FFF;
    fs->min_bounds.rbearing = 0x7FFF;
    fs->min_bounds.width = 0x7FFF;
    fs->max_bounds.lbearing = -1;
    fs->max_bounds.rbearing = -1;
    fs->max_bounds.width = -1;
    for (i = 0; i < nCount; i++)
    {
        /* GDI ABC widths: abcA = origin-to-ink-left (left bearing, can be
           negative), abcB = ink width, abcC = ink-right-to-next-origin
           (right bearing, can be negative for italic/overhang glyphs).
           X11 XCharStruct: lbearing = origin-to-ink-left = abcA,
           rbearing = origin-to-ink-right = abcA + abcB (NOT the full
           advance -- it must be free to differ from width so italic /
           overhanging glyphs are represented correctly), and
           width = full advance = abcA + abcB + abcC. */
        fs->per_char[i].lbearing = pabc[i].abcA;
        fs->per_char[i].rbearing = pabc[i].abcA + pabc[i].abcB;
        fs->per_char[i].width = pabc[i].abcA + pabc[i].abcB + pabc[i].abcC;
        fs->per_char[i].ascent = tm.tmAscent;
        fs->per_char[i].descent = tm.tmDescent;
        if (fs->per_char[i].width < fs->min_bounds.width)
            fs->min_bounds = fs->per_char[i];
        if (fs->per_char[i].width > fs->max_bounds.width)
            fs->max_bounds = fs->per_char[i];
    }
    free(pabc);
    fs->ascent = tm.tmAscent;
    fs->descent = tm.tmDescent;
    return fs;
}

int XUnloadFont(Display *dpy, Font fid)
{
    DeleteObject(fid);
    return 0;
}

int XFreeFont(Display *dpy, XFontStruct *fs)
{
    DeleteObject(fs->fid);
    free(fs->per_char);
    free(fs);
    return 0;
}

int XTextExtents(XFontStruct *fs, const char *string, int nchars,
    int *dir, int *font_ascent, int *font_descent, XCharStruct *overall)
{
    /* Real X11 XTextExtents derives the *overall* XCharStruct by walking
       the string and accumulating each glyph's per-char metrics -- it does
       NOT just ask for the advance width the way GetTextExtentPoint32A
       does. In particular lbearing/rbearing must reflect the leftmost and
       rightmost ink extent across the whole run (which can differ from
       0 and width for italic/kerned/overhanging glyphs). GDI has no
       single call that returns this, so we reuse the ABC-width-derived
       per_char[] table that XLoadQueryFont() already built (per_char[i]
       .lbearing/.rbearing come from abcA/abcB/abcC) and accumulate it
       exactly like libX11 does. */
    int i;
    long x = 0;
    long lbearing = 0;
    long rbearing = 0;
    int ascent = fs->ascent;
    int descent = fs->descent;
    Bool first = True;

    if (dir)
        *dir = 0;

    for (i = 0; i < nchars; i++)
    {
        unsigned char c = (unsigned char) string[i];
        XCharStruct *pc;

        if (fs->per_char == NULL ||
            c < fs->min_char_or_byte2 || c > fs->max_char_or_byte2)
            continue;

        pc = &fs->per_char[c - fs->min_char_or_byte2];

        {
            long l = x + pc->lbearing;
            long r = x + pc->rbearing;
            if (first || l < lbearing) lbearing = l;
            if (first || r > rbearing) rbearing = r;
        }
        first = False;

        if (pc->ascent  > ascent)  ascent  = pc->ascent;
        if (pc->descent > descent) descent = pc->descent;

        x += pc->width;
    }

    overall->lbearing = (short) lbearing;
    overall->rbearing = (short) rbearing;
    overall->width     = (short) x;
    overall->ascent     = (short) ascent;
    overall->descent    = (short) descent;

    if (font_ascent)  *font_ascent  = ascent;
    if (font_descent) *font_descent = descent;

    return 1;
}

int XTextWidth(XFontStruct *fs, const char *string, int count)
{
    SIZE siz;
    HDC hdc = CreateCompatibleDC(NULL);
    HGDIOBJ hFontOld = SelectObject(hdc, fs->fid);
    GetTextExtentPoint32A(hdc, string, count, &siz);
    SelectObject(hdc, hFontOld);
    DeleteDC(hdc);
    return siz.cx;
}

int XSetFont(Display *dpy, GC gc, Font fid)
{
    XGCValues *values;

    values = XGetGCValues_(gc);
    if (values == NULL)
        return BadGC;

    values->font = fid;
    return 0;
}

VisualID XVisualIDFromVisual(Visual *visual)
{
    return 0;
}

XVisualInfo *XGetVisualInfo(Display *dpy, long visual_info_mask,
    XVisualInfo *visual_info_template, int *nitems)
{
    XVisualInfo *vi;

    vi = (XVisualInfo *)calloc(1, sizeof(XVisualInfo));
    if (vi == NULL)
        return NULL;

    *vi = *visual_info_template;

    vi->visual = 0;
    vi->visualid = 0;
    vi->screen = 0;
    vi->depth = 32;
    vi->c_class = 0;
    vi->red_mask = 0x000000FF;
    vi->green_mask = 0x0000FF00;
    vi->blue_mask = 0x00FF0000;
    vi->colormap_size = MAX_COLORMAP;
    vi->bits_per_rgb = 32;
    *nitems = 1;

    return vi;
}

int XFreeFontNames(char *list[])
{
    int i;

    if (list == NULL)
        return 0;

    for (i = 0; list[i] != NULL; i++)
        free(list[i]);

    free(list);
    return 0;
}

int XFreeFontInfo(char **names, XFontStruct *info, int actualCount)
{
    int i;

    if (info != NULL)
    {
        for (i = 0; i < actualCount; i++)
        {
            if (info[i].fid)
                DeleteObject(info[i].fid);
            free(info[i].per_char);
        }
        free(info);
    }

    /* Also free the name array when the caller passes it
       (per X11: free both names and info, or info only if names is NULL) */
    if (names != NULL)
        XFreeFontNames(names);

    return 0;
}

/* Case-insensitive wildcard match: * = any sequence, ? = any single char */
static int match_pattern(const char *str, const char *pat)
{
    for (;;)
    {
        if (*pat == '\0')
            return *str == '\0';
        if (*pat == '*')
        {
            while (*pat == '*')
                pat++;
            if (*pat == '\0')
                return 1;
            for (; *str; str++)
            {
                if (match_pattern(str, pat))
                    return 1;
            }
            return 0;
        }
        if (*str == '\0')
            return 0;
        if (*pat != '?' &&
            (unsigned char)tolower((unsigned char)*pat) !=
            (unsigned char)tolower((unsigned char)*str))
            return 0;
        pat++;
        str++;
    }
}

typedef struct
{
    char **names;
    int count;
    int maxnames;
    const char *pattern;
} FontEnumData;

static int CALLBACK EnumFontFamExProc(
    const LOGFONTA *lpelfe,
    const TEXTMETRICA *lpntme,
    DWORD FontType,
    LPARAM lParam)
{
    FontEnumData *data = (FontEnumData *)lParam;
    const char *face = lpelfe->lfFaceName;
    int i;
    char *dup;

    /* Skip vertical fonts and empty names */
    if (face[0] == '@' || face[0] == '\0')
        return 1;

    if (data->count >= data->maxnames)
        return 0;   /* stop enumeration */

    /* Deduplicate (EnumFontFamiliesEx can report the same face multiple times) */
    for (i = 0; i < data->count; i++)
    {
        if (_stricmp(data->names[i], face) == 0)
            return 1;
    }

    if (!match_pattern(face, data->pattern))
        return 1;

    dup = _strdup(face);
    if (dup == NULL)
        return 0;

    data->names[data->count++] = dup;
    return 1;
}

/* Face name for the most recently filled XListFontsWithInfo entry.
   fontglide only queries properties of info[0] right after the call. */
static char s_font_prop_face[LF_FACESIZE] = "Arial";

/* Build a lightweight XFontStruct (no per_char metrics) for a face name.
   Matches what XListFontsWithInfo is supposed to return. */
static int fill_font_info_from_face(const char *face, XFontStruct *fs)
{
    LOGFONTA lf;
    HDC hdc;
    TEXTMETRICA tm;
    HGDIOBJ hFontOld;
    HFONT hFont;

    ZeroMemory(fs, sizeof(*fs));
    ZeroMemory(&lf, sizeof(lf));

    hdc = CreateCompatibleDC(NULL);
    if (hdc == NULL)
        return 0;

    lstrcpynA(lf.lfFaceName, face, LF_FACESIZE);
    /* Use the actual device DPI (like XLoadQueryFont does) instead of
       assuming 96 DPI, so metrics stay correct under display scaling. */
    lf.lfHeight = -MulDiv(12, GetDeviceCaps(hdc, LOGPIXELSY), 72); /* nominal 12pt for metrics */
    lf.lfWeight = FW_NORMAL;
    lf.lfQuality = ANTIALIASED_QUALITY;
    lf.lfCharSet = DEFAULT_CHARSET;

    hFont = CreateFontIndirectA(&lf);
    if (hFont == NULL)
    {
        DeleteDC(hdc);
        return 0;
    }

    hFontOld = SelectObject(hdc, hFont);
    GetTextMetricsA(hdc, &tm);
    SelectObject(hdc, hFontOld);
    DeleteDC(hdc);

    /* Do not keep the HFONT open - XListFontsWithInfo is not a loaded font.
       fid is left as 0 so XFreeFontInfo will not DeleteObject it. */
    DeleteObject(hFont);

    fs->fid = 0;
    fs->min_char_or_byte2 = 0;
    fs->max_char_or_byte2 = 255;
    fs->per_char = NULL;   /* intentionally omitted */
    fs->ascent = tm.tmAscent;
    fs->descent = tm.tmDescent;

    fs->min_bounds.lbearing = 0;
    fs->min_bounds.rbearing = 0;
    fs->min_bounds.width = tm.tmAveCharWidth;
    fs->min_bounds.ascent = tm.tmAscent;
    fs->min_bounds.descent = tm.tmDescent;

    fs->max_bounds.lbearing = 0;
    fs->max_bounds.rbearing = tm.tmMaxCharWidth;
    fs->max_bounds.width = tm.tmMaxCharWidth;
    fs->max_bounds.ascent = tm.tmAscent;
    fs->max_bounds.descent = tm.tmDescent;

    return 1;
}

char **XListFontsWithInfo(Display *display, char *pattern, int maxnames,
    int *count_return, XFontStruct **info_return)
{
    char **names;
    char **out_names;
    XFontStruct *infos;
    int count;
    int i, j;

    if (count_return)
        *count_return = 0;
    if (info_return)
        *info_return = NULL;

    names = XListFonts(display, pattern, maxnames, &count);
    if (names == NULL || count <= 0)
        return NULL;

    out_names = (char **)calloc(count + 1, sizeof(char *));
    infos = (XFontStruct *)calloc(count, sizeof(XFontStruct));
    if (out_names == NULL || infos == NULL)
    {
        free(out_names);
        free(infos);
        XFreeFontNames(names);
        return NULL;
    }

    j = 0;
    for (i = 0; i < count; i++)
    {
        if (fill_font_info_from_face(names[i], &infos[j]))
        {
            /* Remember only the face that becomes info[0], since that is
               the entry XGetFontProperty(FAMILY_NAME) is documented to
               describe right after this call. */
            if (j == 0 && names[i] && names[i][0])
                lstrcpynA(s_font_prop_face, names[i], LF_FACESIZE);
            out_names[j] = names[i];  /* take ownership of the string */
            names[i] = NULL;
            j++;
        }
        else
        {
            free(names[i]);
            names[i] = NULL;
        }
    }

    /* Free the original name array shell (all strings already handled) */
    free(names);

    if (j == 0)
    {
        free(out_names);
        free(infos);
        return NULL;
    }

    out_names[j] = NULL;

    /* Shrink arrays to actual size */
    {
        char **n = (char **)realloc(out_names, (j + 1) * sizeof(char *));
        if (n)
            out_names = n;
    }
    if (j < count)
    {
        XFontStruct *s = (XFontStruct *)realloc(infos, j * sizeof(XFontStruct));
        if (s)
            infos = s;
    }

    if (count_return)
        *count_return = j;
    if (info_return)
        *info_return = infos;
    else
        free(infos);   /* caller only wanted the name list */

    return out_names;
}

/* True if pattern looks like an X11 XLFD (e.g. -*-*-*-*-*-*-0-0-0-0-p-0-iso8859-1).
   Face names from EnumFontFamiliesEx never match such patterns, so treat them
   as "match all faces" (scalable font query). */
static int is_xlfd_pattern(const char *pat)
{
    return (pat && pat[0] == '-');
}

char **XListFonts(Display *display, char *pattern, int maxnames, int *actual_count_return)
{
    HDC hdc;
    LOGFONTA lf;
    FontEnumData data;
    char **result;
    const char *pat;

    (void)display;

    if (actual_count_return)
        *actual_count_return = 0;

    if (maxnames <= 0)
        return NULL;

    pat = (pattern && pattern[0]) ? pattern : "*";

    /* XLFD patterns request scalable fonts by face; match every face name. */
    if (is_xlfd_pattern(pat))
        pat = "*";

    /* Allocate maxnames + 1 so we can always place a trailing NULL */
    data.names = (char **)calloc(maxnames + 1, sizeof(char *));
    if (data.names == NULL)
        return NULL;
    data.count = 0;
    data.maxnames = maxnames;
    data.pattern = pat;

    ZeroMemory(&lf, sizeof(lf));
    lf.lfCharSet = DEFAULT_CHARSET;
    /* empty lfFaceName => enumerate all faces */

    hdc = CreateCompatibleDC(NULL);
    if (hdc)
    {
        EnumFontFamiliesExA(hdc, &lf, EnumFontFamExProc, (LPARAM)&data, 0);
        DeleteDC(hdc);
    }

    if (data.count == 0)
    {
        free(data.names);
        return NULL;
    }

    /* data.names[data.count] is already NULL because of calloc */
    /* Shrink the array to the actual count + trailing NULL */
    result = (char **)realloc(data.names, (data.count + 1) * sizeof(char *));
    if (result == NULL)
        result = data.names;   /* realloc failed, keep original (still has NULL) */

    if (actual_count_return)
        *actual_count_return = data.count;

    return result;
}

/* Local atom table so empty strings and property names work without
   relying on the limited system Global atom table. */
#define LOCAL_ATOM_BASE  0xA0000
#define LOCAL_ATOM_MAX   64

static struct {
    char *name;
} s_local_atoms[LOCAL_ATOM_MAX];
static int s_local_atom_count = 0;

static Atom local_intern_atom(const char *atom_name, Bool only_if_exists)
{
    int i;
    char *dup;
    for (i = 0; i < s_local_atom_count; i++)
    {
        if (strcmp(s_local_atoms[i].name, atom_name) == 0)
            return (Atom)(LOCAL_ATOM_BASE + i);
    }
    if (only_if_exists)
        return 0;
    if (s_local_atom_count >= LOCAL_ATOM_MAX)
        return 0;
    dup = _strdup(atom_name);
    if (!dup)
        return 0;
    s_local_atoms[s_local_atom_count].name = dup;
    return (Atom)(LOCAL_ATOM_BASE + s_local_atom_count++);
}

static const char *local_atom_name(Atom atom)
{
    int i = (int)(atom - LOCAL_ATOM_BASE);
    if (i >= 0 && i < s_local_atom_count)
        return s_local_atoms[i].name;
    return NULL;
}

char *XGetAtomName(Display *display, Atom atom)
{
    const char *local;
    char buf[MAX_PATH];
    (void)display;

    local = local_atom_name(atom);
    if (local)
        return _strdup(local);

    if (!GlobalGetAtomNameA((ATOM)atom, buf, _countof(buf)))
        return NULL;
    /* X11 returns a newly allocated string; callers XFree it. */
    return _strdup(buf);
}

Atom XInternAtom(Display *display, const char *atom_name, Bool only_if_exists)
{
    Atom a;
    (void)display;
    if (!atom_name)
        return 0;
    /* Prefer local table (supports empty string and avoids Global atom limits). */
    a = local_intern_atom(atom_name, only_if_exists);
    if (a)
        return a;
    if (only_if_exists)
        return 0;
    /* Fallback */
    a = GlobalFindAtomA(atom_name);
    if (a)
        return a;
    return GlobalAddAtomA(atom_name);
}

Bool XGetFontProperty(XFontStruct *font_struct, Atom atom, unsigned long *value_return)
{
    char name[MAX_PATH];
    const char *aname;
    const char *str_val = NULL;
    unsigned long int_val = 0;
    int is_string = 0;

    (void)font_struct;

    if (!atom || !value_return)
        return False;

    aname = local_atom_name(atom);
    if (aname)
        lstrcpynA(name, aname, _countof(name));
    else if (!GlobalGetAtomNameA((ATOM)atom, name, _countof(name)))
        return False;

    /* String-valued XLFD fields (returned as Atoms) */
    if (_stricmp(name, "FOUNDRY") == 0)
    {
        str_val = "windows";
        is_string = 1;
    }
    else if (_stricmp(name, "FAMILY_NAME") == 0)
    {
        str_val = s_font_prop_face;
        is_string = 1;
    }
    else if (_stricmp(name, "WEIGHT_NAME") == 0)
    {
        str_val = "medium";
        is_string = 1;
    }
    else if (_stricmp(name, "SLANT") == 0)
    {
        str_val = "r";
        is_string = 1;
    }
    else if (_stricmp(name, "SETWIDTH_NAME") == 0)
    {
        str_val = "normal";
        is_string = 1;
    }
    else if (_stricmp(name, "ADD_STYLE_NAME") == 0)
    {
        str_val = ""; /* empty XLFD field */
        is_string = 1;
    }
    else if (_stricmp(name, "SPACING") == 0)
    {
        /* Proportional: fontglide skips fixed-width faces when possible */
        str_val = "p";
        is_string = 1;
    }
    else if (_stricmp(name, "CHARSET_REGISTRY") == 0)
    {
        str_val = "iso8859";
        is_string = 1;
    }
    else if (_stricmp(name, "CHARSET_ENCODING") == 0)
    {
        str_val = "1";
        is_string = 1;
    }
    /* Integer-valued XLFD fields */
    else if (_stricmp(name, "PIXEL_SIZE") == 0)
    {
        int_val = 0; /* scalable */
    }
    else if (_stricmp(name, "POINT_SIZE") == 0)
    {
        int_val = 0;
    }
    else if (_stricmp(name, "RESOLUTION_X") == 0)
    {
        int_val = 96;
    }
    else if (_stricmp(name, "RESOLUTION_Y") == 0)
    {
        int_val = 96;
    }
    else if (_stricmp(name, "AVERAGE_WIDTH") == 0)
    {
        int_val = 0;
    }
    else
    {
        return False;
    }

    if (is_string)
    {
        Atom a = XInternAtom(NULL, str_val, False);
        if (!a)
            return False;
        *value_return = (unsigned long)a;
    }
    else
    {
        *value_return = int_val;
    }
    return True;
}

//////////////////////////////////////////////////////////////////////////////
