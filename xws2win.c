#include "xws2win.h"
#include "colors.h"

//////////////////////////////////////////////////////////////////////////////

DrawableData* XGetDrawableData_(Drawable d)
{
    return d;
}

HDC XCreateDrawableDC_(Display *dpy, Drawable d)
{
    HDC hdc;
    DrawableData* data;

    if (d != NULL)
    {
        hdc = CreateCompatibleDC(dpy);
        if (hdc != NULL)
        {
            data = XGetDrawableData_(d);
            data->hbmOld = SelectObject(hdc, data->hbm);
        }
    }
    else
    {
        hdc = dpy;
    }
    return hdc;
}

int XDeleteDrawableDC_(Display *dpy, Drawable d, HDC hdc)
{
    DrawableData *data;
    if (d != NULL)
    {
        data = XGetDrawableData_(d);
        SelectObject(hdc, data->hbmOld);
        DeleteDC(hdc);
    }
    return 0;
}

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
    newvalues->graphics_exposures = True;
    if (values != NULL)
        XChangeGC(dpy, newvalues, valuemask, values);

    if (d != NULL)
    {
        DrawableData *data = XGetDrawableData_(d);
        newvalues->hbmOld = SelectObject(dpy, data->hbm);
    }

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
    }

    if (valuemask & GCBackground)
    {
        color.pixel = newvalues->background = values->background;
        XQueryColor(dpy, DefaultColormap(dpy, DefaultScreenOfDisplay(dpy)),
                    &color);
        newvalues->background_rgb =
            RGB(color.red / 256, color.green / 256, color.blue / 256);
    }

    if (valuemask & GCFillStyle)
        newvalues->fill_style = values->fill_style;

    if (valuemask & GCFunction)
        newvalues->function = values->function;

    if (valuemask & GCLineWidth)
        newvalues->line_width = values->line_width;

    if (valuemask & GCCapStyle)
        newvalues->cap_style = values->cap_style;

    if (valuemask & GCJoinStyle)
        newvalues->join_style = values->join_style;

    if (valuemask & GCLineStyle)
        newvalues->line_style = values->line_style;

    if (valuemask & GCStipple)
        newvalues->stipple = values->stipple;

    if (valuemask & GCGraphicsExposures)
        newvalues->graphics_exposures = values->graphics_exposures;

    return 0;
}

int XFreeGC(Display *dpy, GC gc)
{
    XGCValues* values = XGetGCValues_(gc);
    if (values == NULL)
        return BadGC;

    if (values->hbmOld)
        SelectObject(dpy, values->hbmOld);

    free(gc);
    return 0;
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
        SetPixelV(dpy, x, y, rgb);
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

HPEN XCreateWinPen(XGCValues *values)
{
    LOGBRUSH lb;
    lb.lbColor = values->foreground_rgb;
    lb.lbStyle = BS_SOLID;
    return ExtCreatePen(
        PS_GEOMETRIC | values->line_style | values->cap_style | values->join_style,
        values->line_width, &lb, 0, NULL);
}

LPVOID XCreatePackedDIBFromPixmap(Pixmap pixmap)
{
    BITMAP bm;
    LPBYTE pb;
    DWORD size = sizeof(BITMAPINFOHEADER);
    LPBITMAPINFO lpbi = (LPBITMAPINFO)calloc(1, size);
    assert(lpbi != NULL);

    GetObject(pixmap->hbm, sizeof(BITMAP), &bm);
    GetDIBits(NULL, pixmap->hbm, 0, bm.bmHeight, NULL, lpbi, DIB_RGB_COLORS);
    size += lpbi->bmiHeader.biSizeImage;
    lpbi = (LPBITMAPINFO)realloc(lpbi, size);
    assert(lpbi != NULL);
    pb = (LPBYTE)lpbi;
    pb += sizeof(BITMAPINFOHEADER);
    GetDIBits(NULL, pixmap->hbm, 0, bm.bmHeight, pb, lpbi, DIB_RGB_COLORS);
    return lpbi;
}

HBRUSH XCreateWinBrush(XGCValues *values)
{
    HBRUSH hbr;
    LOGBRUSH lb;
    if (values->fill_style == FillSolid)
    {
        lb.lbColor = values->foreground_rgb;
        lb.lbStyle = BS_SOLID;
        return CreateBrushIndirect(&lb);
    }
    else if (values->fill_style == FillStippled)
    {
        LPVOID lpPackedDIB = XCreatePackedDIBFromPixmap(values->stipple);
        hbr = CreateDIBPatternBrushPt(lpPackedDIB, DIB_RGB_COLORS);
        free(lpPackedDIB);
        return hbr;
    }
    return NULL;
}

HFONT XCreateWinFont(XGCValues *values)
{
    return (HFONT)GetStockObject(DEFAULT_GUI_FONT);
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

    hPen = XCreateWinPen(values);
    assert(hPen);
    if (hPen == NULL)
        return BadAlloc;

    hdc = XCreateDrawableDC_(dpy, d);
    nR2 = SetROP2(hdc, values->function);
    hPenOld = SelectObject(hdc, hPen);
    MoveToEx(hdc, x1, y1, NULL);
    LineTo(hdc, x2, y2);
    SelectObject(hdc, hPenOld);
    SetROP2(hdc, nR2);
    XDeleteDrawableDC_(dpy, d, hdc);

    DeleteObject(hPen);
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

    hPen = XCreateWinPen(values);
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

    DeleteObject(hPen);

    return 0;
}

int XDrawRectangle(
    Display *dpy, Drawable d, GC gc,
    int x, int y, unsigned int width, unsigned int height)
{
    XGCValues *values;
    HDC hdc;
    HPEN hPen;
    int nR2;

    values = XGetGCValues_(gc);
    if (values == NULL)
        return BadGC;

    hPen = XCreateWinPen(values);
    assert(hPen);
    if (hPen == NULL)
        return BadAlloc;

    hdc = XCreateDrawableDC_(dpy, d);
    nR2 = SetROP2(hdc, values->function);
    SelectObject(hdc, GetStockObject(NULL_BRUSH));
    Rectangle(hdc, x, y, x + width, y + height);
    SetROP2(hdc, nR2);
    XDeleteDrawableDC_(dpy, d, hdc);

    DeleteObject(hPen);
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

    hPen = XCreateWinPen(values);
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

    DeleteObject(hPen);
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

    hPen = XCreateWinPen(values);
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
    Arc(hdc, x, y, x + width, y + height,
        xStartArc, yStartArc, xEndArc, yEndArc);
    SelectObject(hdc, hPenOld);
    SetROP2(hdc, nR2);
    XDeleteDrawableDC_(dpy, d, hdc);

    DeleteObject(hPen);
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

    hPen = XCreateWinPen(values);
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

        Arc(hdc, x, y, x + width, y + height,
            xStartArc, yStartArc, xEndArc, yEndArc);
    }
    SelectObject(hdc, hPenOld);
    SetROP2(hdc, nR2);
    XDeleteDrawableDC_(dpy, d, hdc);

    DeleteObject(hPen);
    return 0;
}

int XDrawString(Display *dpy, Drawable d, GC gc,
    int x, int y, const char *string, int length)
{
    XGCValues *values;
    HFONT hFont;
    HGDIOBJ hFontOld;

    values = XGetGCValues_(gc);
    if (values == NULL)
        return BadGC;

    hFont = XCreateWinFont(values);
    if (hFont == NULL)
        return BadAlloc;

    hFontOld = SelectObject(dpy, hFont);
    TextOut(dpy, x, y, string, length);
    SelectObject(dpy, hFontOld);

    DeleteObject(hFont);
    return 0;
}

int XDrawImageString(Display *dpy, Drawable d, GC gc,
    int x, int y, const char *string, int length)
{
    assert(0);
    return 0;
}

int XFillRectangle(
    Display *dpy, Drawable d, GC gc,
    int x, int y, unsigned int width, unsigned int height)
{
    XGCValues *values;
    HDC hdc;
    HBRUSH hbr;
    RECT rc;
    int nR2;

    values = XGetGCValues_(gc);
    if (values == NULL)
        return BadGC;

    hbr = XCreateWinBrush(values);
    if (hbr == NULL)
        return BadAlloc;

    SetRect(&rc, x, y, x + width, y + height);

    hdc = XCreateDrawableDC_(dpy, d);
    nR2 = SetROP2(hdc, values->function);
    FillRect(hdc, &rc, hbr);
    SetROP2(hdc, nR2);
    XDeleteDrawableDC_(dpy, d, hdc);

    DeleteObject(hbr);
    return 0;
}

int XFillRectangles(
    Display *dpy, Drawable d, GC gc,
    XRectangle *rectangles, int n_rects)
{
    XGCValues *values;
    HDC hdc;
    HBRUSH hbr;
    RECT rc;
    int i;
    short x, y;
    unsigned short width, height;
    int nR2;

    values = XGetGCValues_(gc);
    if (values == NULL)
        return BadGC;

    hbr = XCreateWinBrush(values);
    if (hbr == NULL)
        return BadAlloc;

    hdc = XCreateDrawableDC_(dpy, d);
    nR2 = SetROP2(hdc, values->function);
    for (i = 0; i < n_rects; i++)
    {
        x = rectangles[i].x;
        y = rectangles[i].x;
        width = rectangles[i].width;
        height = rectangles[i].height;
        SetRect(&rc, x, y, x + width, y + height);
        FillRect(hdc, &rc, hbr);
    }
    SetROP2(hdc, nR2);
    XDeleteDrawableDC_(dpy, d, hdc);

    DeleteObject(hbr);
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

    hbr = XCreateWinBrush(values);
    if (hbr == NULL)
    {
        free(lpPoints);
        return BadAlloc;
    }

    hdc = XCreateDrawableDC_(dpy, d);
    nR2 = SetROP2(hdc, values->function);
    hbrOld = SelectObject(hdc, hbr);
    BeginPath(hdc);
    Polygon(hdc, lpPoints, n_points);
    EndPath(hdc);
    FillPath(hdc);
    SelectObject(hdc, hbrOld);
    SetROP2(hdc, nR2);
    XDeleteDrawableDC_(dpy, d, hdc);

    free(lpPoints);
    DeleteObject(hbr);
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

    hbr = XCreateWinBrush(values);
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
    BeginPath(hdc);
    Arc(hdc, x, y, x + width, y + height,
        xStartArc, yStartArc, xEndArc, yEndArc);
    EndPath(hdc);
    FillPath(hdc);
    SelectObject(hdc, hbrOld);
    SetROP2(hdc, nR2);
    XDeleteDrawableDC_(dpy, d, hdc);

    DeleteObject(hbr);
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

    hbr = XCreateWinBrush(values);
    if (hbr == NULL)
        return BadAlloc;

    hdc = XCreateDrawableDC_(dpy, d);
    nR2 = SetROP2(hdc, values->function);
    hbrOld = SelectObject(hdc, hbr);
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
        Arc(hdc, x, y, x + width, y + height,
            xStartArc, yStartArc, xEndArc, yEndArc);
        EndPath(hdc);
        FillPath(hdc);
    }
    SelectObject(hdc, hbrOld);
    SetROP2(hdc, nR2);
    XDeleteDrawableDC_(dpy, d, hdc);

    DeleteObject(hbr);
    return 0;
}

int XCopyArea(Display *dpy,
     Drawable src_drawable, Drawable dst_drawable, GC gc,
     int src_x, int src_y,
     unsigned int width, unsigned int height,
     int dst_x, int dst_y)
{
    HDC hdcSrc = XCreateDrawableDC_(dpy, src_drawable);
    HDC hdcDst = XCreateDrawableDC_(dpy, dst_drawable);
    BitBlt(hdcDst, dst_x, dst_y, width, height, hdcSrc, src_x, src_y, SRCCOPY);
    XDeleteDrawableDC_(dpy, src_drawable, hdcSrc);
    XDeleteDrawableDC_(dpy, dst_drawable, hdcDst);
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

    assert(fill == FillSolid);
    values->fill_style = fill;
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
            for (x = 0; x < (ximage->width + 7) / 8; x++)
            {
                pbBits[y * widthbytes + x] = ximage->data[(ximage->height - y - 1) * ximage->bytes_per_line + x];
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

    return NULL;
}

// NOTE: too slow!!!
int XPutImage(Display *dpy, Drawable d, GC gc,
    XImage *image, int req_xoffset, int req_yoffset,
    int x, int y, unsigned int req_width, unsigned int req_height)
{
    HBITMAP hbm;
    int width = req_width, height = req_height;

    if (req_xoffset < 0)
    {
        width += req_xoffset;
        req_xoffset = 0;
    }
    if (req_yoffset < 0)
    {
        height += req_yoffset;
        req_yoffset = 0;
    }

    if (req_xoffset + width > (int)image->width)
        width = image->width - req_xoffset;
    if (req_yoffset + height > (int)image->height)
        height = image->height - req_yoffset;
    if (width <= 0 || height <= 0)
        return 0;

    hbm = XCreateWinBitmapFromXImage(image);
    if (hbm == NULL)
        return 0;

    if (d == 0)
    {
        HDC hdcSrc = CreateCompatibleDC(dpy);
        HGDIOBJ hbmOld = SelectObject(hdcSrc, hbm);
        BitBlt(dpy, req_xoffset, req_yoffset, width, height, hdcSrc, x, y, SRCCOPY);
        SelectObject(hdcSrc, hbmOld);
        DeleteDC(hdcSrc);
    }
    else
    {
        HDC hdc = XCreateDrawableDC_(dpy, d);
        HDC hdcSrc = CreateCompatibleDC(dpy);
        HGDIOBJ hbmOld = SelectObject(hdcSrc, hbm);
        BitBlt(hdc, req_xoffset, req_yoffset, width, height, hdcSrc, x, y, SRCCOPY);
        SelectObject(hdcSrc, hbmOld);
        DeleteDC(hdcSrc);
        XDeleteDrawableDC_(dpy, d, hdc);
    }

    DeleteObject(hbm);
    return 0;
}

//////////////////////////////////////////////////////////////////////////////
