#include "xws2win.h"

BOOL InitPixelFormat(SCREENSAVER *ss)
{
	return TRUE;
}

VOID MakeCurrent(SCREENSAVER *ss)
{
}

void ss_term(void)
{
    XFreeGC(ss.modeinfo.dpy, ss.modeinfo.gc);
    hack_free(&ss.modeinfo);
    ReleaseDC(ss.hwnd, ss.hdc);
    DeleteObject(ss.hbmScreenShot);
    ss.hbmScreenShot = NULL;
}

void ss_clear(Display *d)
{
    XClearWindow(d, 0);
}

int XSetLineAttributes(Display *dpy, GC gc,
    unsigned int line_width, int line_style,
    int cap_style, int join_style)
{
    XGCValues *values;

    values = XGetGCValues0(gc);
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
    COLORREF rgb;

    values = XGetGCValues0(gc);
    if (values == NULL)
        return BadGC;

    rgb = values->foreground_rgb;
    SetPixel(dpy, x, y, rgb);
    return 0;
}

int XDrawPoints(Display *dpy, Drawable w, GC gc,
    XPoint *points, int npoints, int CoordMode)
{
    XGCValues *values;
    COLORREF rgb;
    int i;

    values = XGetGCValues0(gc);
    if (values == NULL)
        return BadGC;

    rgb = values->foreground_rgb;
    if (CoordMode == CoordModeOrigin)
    {
        for (i = 0; i < npoints; i++)
        {
            SetPixel(dpy, points[i].x, points[i].y, rgb);
        }
    }
    else
    {
        int x = points[0].x, y = points[0].y;
        SetPixel(dpy, x, y, rgb);
        for (i = 1; i < npoints; i++)
        {
            x += points[i].x;
            y += points[i].y;
            SetPixel(dpy, x, y, rgb);
        }
    }
    return 0;
}

HPEN XCreateWinPen(XGCValues *values)
{
    LOGBRUSH lb;
    lb.lbColor = values->foreground_rgb;
    lb.lbStyle = BS_SOLID;
    return ExtCreatePen(
        values->line_style | values->cap_style | values->join_style,
        values->line_width, &lb, 0, NULL);
}

HBRUSH XCreateWinBrush(XGCValues *values)
{
    LOGBRUSH lb;
    lb.lbColor = values->foreground_rgb;
    lb.lbStyle = BS_SOLID;
    return CreateBrushIndirect(&lb);
}

HFONT XCreateWinFont(XGCValues *values)
{
    return (HFONT)GetStockObject(DEFAULT_GUI_FONT);
}

int XDrawLine(Display *dpy, Drawable d, GC gc,
    int x1, int y1, int x2, int y2)
{
    XGCValues *values;
    HPEN hPen;
    HGDIOBJ hPenOld;

    values = XGetGCValues0(gc);
    if (values == NULL)
        return BadGC;

    hPen = XCreateWinPen(values);
    if (hPen == NULL)
        return BadAlloc;

    hPenOld = SelectObject(dpy, hPen);
    MoveToEx(dpy, x1, y1, NULL);
    LineTo(dpy, x2, y2);
    SelectObject(dpy, hPenOld);

    DeleteObject(hPen);
    return 0;
}

int XDrawLines(Display *dpy, Drawable d, GC gc,
    XPoint *points, int npoints, int mode)
{
    XGCValues *values;
    LPPOINT lpPoints;
    HPEN hPen;
    HGDIOBJ hPenOld;
    int i;

    values = XGetGCValues0(gc);
    if (values == NULL)
        return BadGC;

    lpPoints = (LPPOINT)calloc(npoints, sizeof(POINT));
    if (lpPoints == NULL)
        return BadAlloc;

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
        int x = points[0].x, y = points[0].y;
        lpPoints[0].x = x;
        lpPoints[0].y = y;
        for (i = 1; i < npoints; i++)
        {
            x += points[i].x;
            y += points[i].y;
            lpPoints[i].x = x;
            lpPoints[i].y = y;
        }
    }

    hPen = XCreateWinPen(values);
    if (hPen == NULL)
        return BadAlloc;

    hPenOld = SelectObject(dpy, hPen);
    Polyline(dpy, lpPoints, npoints);
    SelectObject(dpy, hPenOld);

    free(lpPoints);
    DeleteObject(hPen);
    return 0;
}

int XDrawSegments(Display *dpy, Drawable d, GC gc,
    XSegment *segments, int nsegments)
{
    XGCValues *values;
    HPEN hPen;
    HGDIOBJ hPenOld;
    int i;

    values = XGetGCValues0(gc);
    if (values == NULL)
        return BadGC;

    hPen = XCreateWinPen(values);
    if (hPen == NULL)
        return BadAlloc;

    hPenOld = SelectObject(dpy, hPen);
    for (i = 0; i < nsegments; i++)
    {
        MoveToEx(dpy, segments[i].x1, segments[i].y1, NULL);
        LineTo(dpy, segments[i].x2, segments[i].y2);
    }
    SelectObject(dpy, hPenOld);

    DeleteObject(hPen);
    return 0;
}

int XDrawArc(Display *dpy, Drawable d, GC gc,
    int x, int y, unsigned int width, unsigned int height,
    int angle1, int angle2)
{
    XGCValues *values;
    HPEN hPen;
    HGDIOBJ hPenOld;
    int xStartArc, yStartArc, xEndArc, yEndArc;

    values = XGetGCValues0(gc);
    if (values == NULL)
        return BadGC;

    hPen = XCreateWinPen(values);
    if (hPen == NULL)
        return BadAlloc;

    xStartArc = yStartArc = xEndArc = yEndArc = 0;

    hPenOld = SelectObject(dpy, hPen);
    Arc(dpy, x, y, x + width, y + height,
        xStartArc, yStartArc, xEndArc, yEndArc);
    SelectObject(dpy, hPenOld);

    DeleteObject(hPen);
    return 0;
}

int XDrawString(Display *dpy, Drawable d, GC gc,
    int x, int y, const char *string, int length)
{
    XGCValues *values;
    HFONT hFont;
    HGDIOBJ hFontOld;

    values = XGetGCValues0(gc);
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
    HBRUSH hbr;
    RECT rc;

    values = XGetGCValues0(gc);
    if (values == NULL)
        return BadGC;

    hbr = XCreateWinBrush(values);
    if (hbr == NULL)
        return BadAlloc;

    SetRect(&rc, x, y, x + width, y + height);
    FillRect(dpy, &rc, hbr);
    DeleteObject(hbr);
    return 0;
}

int XFillRectangles(
    Display *dpy, Drawable d, GC gc,
    XRectangle *rectangles, int n_rects)
{
    XGCValues *values;
    HBRUSH hbr;
    RECT rc;
    int i;
    short x, y;
    unsigned short width, height;

    values = XGetGCValues0(gc);
    if (values == NULL)
        return BadGC;

    hbr = XCreateWinBrush(values);
    if (hbr == NULL)
        return BadAlloc;

    for (i = 0; i < n_rects; i++)
    {
        x = rectangles[i].x;
        y = rectangles[i].x;
        width = rectangles[i].width;
        height = rectangles[i].height;
        SetRect(&rc, x, y, x + width, y + height);
        FillRect(dpy, &rc, hbr);
    }

    DeleteObject(hbr);
    return 0;
}

int XFillPolygon(Display *dpy, Drawable d, GC gc,
    XPoint *points, int n_points, int shape, int mode)
{
    XGCValues *values;
    HBRUSH hbr;
    HGDIOBJ hbrOld;
    LPPOINT lpPoints;
    INT i;

    assert(shape == Convex);

    values = XGetGCValues0(gc);
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
        int x = points[0].x, y = points[0].y;
        lpPoints[0].x = x;
        lpPoints[0].y = y;
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

    BeginPath(dpy);
    Polygon(dpy, lpPoints, n_points);
    EndPath(dpy);
    hbrOld = SelectObject(dpy, hbr);
    FillPath(dpy);
    SelectObject(dpy, hbrOld);

    free(lpPoints);
    DeleteObject(hbr);
    return 0;
}

int XFillArc(Display *dpy, Drawable d, GC gc,
    int x, int y, unsigned int width, unsigned int height,
    int angle1, int angle2)
{
    XGCValues *values;
    HBRUSH hbr;
    HGDIOBJ hbrOld;
    int xStartArc, yStartArc, xEndArc, yEndArc;

    values = XGetGCValues0(gc);
    if (values == NULL)
        return BadGC;

    hbr = XCreateWinBrush(values);
    if (hbr == NULL)
        return BadAlloc;

    xStartArc = yStartArc = xEndArc = yEndArc = 0;

    hbrOld = SelectObject(dpy, hbr);
    BeginPath(dpy);
    Arc(dpy, x, y, x + width, y + height,
        xStartArc, yStartArc, xEndArc, yEndArc);
    EndPath(dpy);
    FillPath(dpy);
    SelectObject(dpy, hbrOld);

    DeleteObject(hbr);
    return 0;
}

int XFillArcs(Display *dpy, Drawable d, GC gc,
    XArc *arcs, int n_arcs)
{
    XGCValues *values;
    HBRUSH hbr;
    HGDIOBJ hbrOld;
    int i, x, y;
    unsigned int width, height;
    short angle1, angle2;
    int xStartArc, yStartArc, xEndArc, yEndArc;

    values = XGetGCValues0(gc);
    if (values == NULL)
        return BadGC;

    hbr = XCreateWinBrush(values);
    if (hbr == NULL)
        return BadAlloc;

    hbrOld = SelectObject(dpy, hbr);
    for (i = 0; i < n_arcs; i++)
    {
        x = arcs[i].x; y = arcs[i].y;
        width = arcs[i].width; height = arcs[i].height;
        angle1 = arcs[i].angle1; angle2 = arcs[i].angle2;

        xStartArc = yStartArc = xEndArc = yEndArc = 0;

        BeginPath(dpy);
        Arc(dpy, x, y, x + width, y + height,
            xStartArc, yStartArc, xEndArc, yEndArc);
        EndPath(dpy);
        FillPath(dpy);
    }
    SelectObject(dpy, hbrOld);

    DeleteObject(hbr);
    return 0;
}

Status XGetWindowAttributes(
    Display *dpy,
    Window w,
    XWindowAttributes *attr)
{
    *attr = ss.modeinfo.xgwa;
    return 0;
}

int XCopyArea(Display *dpy,
     Drawable src_drawable, Drawable dst_drawable, GC gc,
     int src_x, int src_y,
     unsigned int width, unsigned int height,
     int dst_x, int dst_y)
{
    assert(src_drawable == dst_drawable);
    BitBlt(dpy, dst_x, dst_y, width, height, dpy, src_x, src_y, SRCCOPY);
    return 0;
}

int XSync(Display *dpy, Bool b)
{
    GdiFlush();
    return 0;
}
