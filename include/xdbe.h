typedef Pixmap XdbeBackBuffer;

typedef enum
{
	XdbeUndefined, XdbeBackground
} XdbeSwapAction;

typedef struct
{
	Window swap_window;
	XdbeSwapAction swap_action;
} XdbeSwapInfo;

XdbeBackBuffer 
xdbe_get_backbuffer(Display *dpy, Window window, XdbeSwapAction action);

void XdbeSwapBuffers(Display *dpy, XdbeSwapInfo *info, int count);
