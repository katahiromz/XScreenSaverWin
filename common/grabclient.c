#include "xws2win.h"
#include "grabscreen.h"
#include "yarandom.h"
#undef FAR
#define FAR
#define INITGUID
#define COBJMACROS
#include <objbase.h>
#include <ocidl.h>
#include <olectl.h>
#include <shlobj.h>
#include "gdipm.h"

extern void *g_gdipm;

HBITMAP GetScreenShotBM(VOID);

static Bool do_load_image(
    async_load_state *state, Screen *screen, Window window, Drawable target,
    HBITMAP hbm, int width, int height)
{
    BITMAP bm;
    HDC hdcSrc, hdcDst;
    Display *dpy = DisplayOfScreen(screen);
    HGDIOBJ hbmOld;
    assert(hbm != NULL);

    GetObject(hbm, sizeof(BITMAP), &bm);
    state->geom.x = state->geom.y = 0;
    state->geom.width = bm.bmWidth;
    state->geom.height = bm.bmHeight;

    hdcSrc = CreateCompatibleDC(dpy);
    hdcDst = XCreateDrawableDC_(dpy, target);
    hbmOld = SelectObject(hdcSrc, hbm);

    assert(hdcSrc != NULL);
    assert(hdcDst != NULL);
    if (width == 0 || height == 0)
    {
        BitBlt(hdcDst, 0, 0, bm.bmWidth, bm.bmHeight, hdcSrc, 0, 0, SRCCOPY);
    }
    else
    {
        SetStretchBltMode(hdcDst, HALFTONE);
        StretchBlt(hdcDst, 0, 0, width, height,
            hdcSrc, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
    }

    SelectObject(hdcSrc, hbmOld);
    XDeleteDrawableDC_(dpy, target, hdcDst);
    DeleteDC(hdcSrc);
    return True;
}

static HBITMAP do_load_picture(LPCSTR file)
{
    WCHAR szFile[MAX_PATH];
    MultiByteToWideChar(CP_ACP, 0, file, -1, szFile, _countof(szFile));
    return gdipm_load_pic(g_gdipm, szFile, MakeARGB(0xFF, 0xFF, 0xFF, 0xFF), NULL, NULL);
}

char **g_ppFiles = NULL;
int g_nFiles = 0;
#define MAX_LOAD_IMAGES 16

void recurse_dir(const char *dir)
{
    CHAR szDir[MAX_PATH], szPath[MAX_PATH];

    if (g_nFiles >= MAX_LOAD_IMAGES)
        return;

    GetCurrentDirectoryA(MAX_PATH, szDir);
    if (SetCurrentDirectoryA(dir))
    {
        LPSTR pch;
        WIN32_FIND_DATAA find;
        HANDLE hFind1 = FindFirstFileA("*", &find);
        if (hFind1 != INVALID_HANDLE_VALUE)
        {
            do
            {
                if (lstrcmpA(find.cFileName, ".") == 0 ||
                    lstrcmpA(find.cFileName, "..") == 0)
                {
                    continue;
                }
                if (find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                    continue;

                pch = strrchr(find.cFileName, '\\');
                if (pch == NULL)
                    pch = strrchr(find.cFileName, '.');
                else
                    pch = strrchr(pch, '.');

                if (!pch)
                    continue;

                if (lstrcmpiA(pch, ".bmp") != 0 &&
                    lstrcmpiA(pch, ".dib") != 0 &&
                    lstrcmpiA(pch, ".jpe") != 0 &&
                    lstrcmpiA(pch, ".jpg") != 0 &&
                    lstrcmpiA(pch, ".jpeg") != 0 &&
                    lstrcmpiA(pch, ".jfif") != 0 &&
                    lstrcmpiA(pch, ".gif") != 0 &&
                    lstrcmpiA(pch, ".png") != 0 &&
                    lstrcmpiA(pch, ".tif") != 0 &&
                    lstrcmpiA(pch, ".tiff") != 0)
                {
                    continue;
                }

                GetCurrentDirectoryA(MAX_PATH, szPath);
                lstrcatA(szPath, "\\");
                lstrcatA(szPath, find.cFileName);

                g_ppFiles = (char **)realloc(g_ppFiles, (g_nFiles + 1) * sizeof(char *));
                g_ppFiles[g_nFiles++] = _strdup(szPath);
                if (g_nFiles >= MAX_LOAD_IMAGES)
                    break;
            } while (FindNextFileA(hFind1, &find));
            FindClose(hFind1);
        }
        if (g_nFiles < MAX_LOAD_IMAGES)
        {
            HANDLE hFind2 = FindFirstFileA("*", &find);
            if (hFind2 != INVALID_HANDLE_VALUE)
            {
                do
                {
                    if (lstrcmpA(find.cFileName, ".") == 0 ||
                        lstrcmpA(find.cFileName, "..") == 0)
                    {
                        continue;
                    }
                    if (find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                    {
                        recurse_dir(find.cFileName);
                    }
                } while (FindNextFileA(hFind2, &find));
                FindClose(hFind2);
            }
        }
        SetCurrentDirectoryA(szDir);
    }
}

static Bool do_load_image_from_dir(
    async_load_state *state, Screen *screen, Window window, Drawable target,
    const char *dir, int width, int height, char **name_ret)
{
    int i;
    HBITMAP hbm;

    recurse_dir(dir);

    if (g_nFiles <= 0)
        return False;

    i = random() % g_nFiles;
    hbm = do_load_picture(g_ppFiles[i]);
    if (hbm != NULL)
    {
        *name_ret = _strdup(strrchr(g_ppFiles[i], '\\') + 1);
        return do_load_image(state, screen, window, target, hbm, width, height);
    }
    return False;
}

async_load_state *load_image_async_simple(
    async_load_state *state, Screen *screen, Window window, Drawable target, 
    char **filename_ret, XRectangle *geometry_ret)
{
    async_load_state state1;
    assert(filename_ret == NULL);
    assert(geometry_ret == NULL);

    do_load_image(&state1, screen, window, target, GetScreenShotBM(), 0, 0);
    return NULL;
}

extern Bool grabDesktopImages;
extern Bool chooseRandomImages;
extern char *imageDirectory;

void
load_image_async (Screen *screen, Window window, Drawable drawable,
                  void (*callback) (Screen *, Window, Drawable,
                                    const char *name, XRectangle *geom,
                                    void *closure),
                  void *closure)
{
  Display *dpy = DisplayOfScreen (screen);
  XWindowAttributes xgwa;
  Bool deskp = grabDesktopImages;
  Bool filep = chooseRandomImages;
  char *dir = 0, *dir2 = 0;
  Bool done = False;
  XRectangle *geom_ret;
  XRectangle geom_ret_2;
  char **name_ret = 0;
  char *name_ret_2 = 0;
  async_load_state state1;
  CHAR szDir[MAX_PATH];

  if (!drawable) abort();

  if (callback) {
    geom_ret = &geom_ret_2;
    name_ret = &name_ret_2;
  }

  XGetWindowAttributes (dpy, window, &xgwa);
  {
    Window r;
    int x, y;
    unsigned int w, h, bbw, d;
    XGetGeometry (dpy, drawable, &r, &x, &y, &w, &h, &bbw, &d);
    xgwa.width = w;
    xgwa.height = h;
  }

  if (name_ret)
    *name_ret = 0;

  if (geom_ret) {
    geom_ret->x = 0;
    geom_ret->y = 0;
    geom_ret->width  = xgwa.width;
    geom_ret->height = xgwa.height;
  }

  if (filep)
    dir = imageDirectory;

  //if (!dir || !*dir)
  //  filep = False;

  if (deskp && filep) {
    deskp = !(random() & 5);    /* if both, desktop 1/5th of the time */
    filep = !deskp;
  }

  if (filep && !done) {
    dir2 = dir;
    if (!dir || !*dir) {
      if (SHGetSpecialFolderPathA(NULL, szDir, CSIDL_MYPICTURES, FALSE))
        dir2 = szDir;
    }
    done = do_load_image_from_dir(&state1, screen, window, drawable, dir2, xgwa.width, xgwa.height, name_ret);
    if (!dir || !*dir) {
      if (SHGetSpecialFolderPathA(NULL, szDir, CSIDL_COMMON_PICTURES, FALSE)) {
        dir2 = szDir;
        done |= do_load_image_from_dir(&state1, screen, window, drawable, dir2, xgwa.width, xgwa.height, name_ret);
      }
    }
  }

  if (!done) {
    do_load_image(&state1, screen, window, drawable, GetScreenShotBM(), xgwa.width, xgwa.height);
  }

  if (callback) {
    callback (screen, window, drawable, name_ret_2, &geom_ret_2, closure);
    if (name_ret_2) free (name_ret_2);
  }
}
