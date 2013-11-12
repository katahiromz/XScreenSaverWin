#include "screenhack.h"
#include "xdbe.h"

static Pixmap pixmap = NULL;

XdbeBackBuffer 
xdbe_get_backbuffer(Display *dpy, Window window, XdbeSwapAction action)
{
	pixmap = XCreatePixmap(dpy, window, ss.width, ss.height, 32);
	return pixmap;
}

void XdbeSwapBuffers(Display *dpy, XdbeSwapInfo *info, int count)
{
	HDC hdc = CreateCompatibleDC(dpy);
	HGDIOBJ hbmOld = SelectObject(hdc, pixmap->hbm);
	BitBlt(dpy, 0, 0, ss.width, ss.height, hdc, 0, 0, SRCCOPY);
	SelectObject(hdc, hbmOld);
	DeleteDC(hdc);
}
