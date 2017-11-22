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
    HANDLE hFile;
    DWORD dwSize, cbRead;
    HGLOBAL hGlobal;
    LPVOID p;
    LPSTREAM pStream;
    LPPICTURE pPicture;
    HBITMAP hbm;
    HRESULT hr;

    hFile = CreateFileA(file, GENERIC_READ, FILE_SHARE_READ, NULL,
        OPEN_EXISTING, 0, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
        return NULL;

    dwSize = GetFileSize(hFile, NULL);

    hGlobal = GlobalAlloc(GMEM_MOVEABLE, dwSize);
    if (hGlobal == NULL)
    {
        CloseHandle(hFile);
        return NULL;
    }

    p = GlobalLock(hGlobal);

    if (!ReadFile(hFile, p, dwSize, &cbRead, NULL))
    {
        GlobalUnlock(hGlobal);
        GlobalFree(hGlobal);
        CloseHandle(hFile);
        return NULL;
    }

    GlobalUnlock(hGlobal);
    CloseHandle(hFile);

    CreateStreamOnHGlobal(hGlobal, TRUE, &pStream);

    if (!pStream)
        return NULL;

    hbm = NULL;
    hr = OleLoadPicture(pStream, dwSize, FALSE, &IID_IPicture, (void **)&pPicture);
    assert(pPicture);

    if (pPicture != NULL)
    {
        pPicture->lpVtbl->get_Handle(pPicture, (OLE_HANDLE *)&hbm);
        if (hbm != NULL)
        {
            hbm = (HBITMAP)CopyImage(hbm, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
        }
        pPicture->lpVtbl->Release(pPicture);
    }

    pStream->lpVtbl->Release(pStream);
    GlobalFree(hGlobal);

    return hbm;
}

char **g_ppFiles = NULL;
int g_nFiles = 0;

void recurse_dir(const char *dir)
{
    CHAR szDir[MAX_PATH], szPath[MAX_PATH];

    GetCurrentDirectoryA(MAX_PATH, szDir);
    if (SetCurrentDirectoryA(dir))
    {
        WIN32_FIND_DATAA find;
        HANDLE hFind = FindFirstFileA("*", &find);
        if (hFind != INVALID_HANDLE_VALUE)
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
                else
                {
                    LPSTR pch = strrchr(find.cFileName, '\\');
                    if (pch == NULL)
                        pch = strrchr(find.cFileName, '.');
                    else
                        pch = strrchr(pch, '.');

                    if (lstrcmpiA(pch, ".bmp") == 0 ||
                        lstrcmpiA(pch, ".jpg") == 0 ||
                        lstrcmpiA(pch, ".jpeg") == 0 ||
                        lstrcmpiA(pch, ".gif") == 0 ||
                        //lstrcmpiA(pch, ".png") == 0 ||
                        //lstrcmpiA(pch, ".tif") == 0 ||
                        //lstrcmpiA(pch, ".tiff") == 0 ||
                        0)
                    {
                    }
                    else
                        continue;

                    GetCurrentDirectoryA(MAX_PATH, szPath);
                    lstrcatA(szPath, "\\");
                    lstrcatA(szPath, find.cFileName);

                    g_ppFiles = (char **)realloc(g_ppFiles, (g_nFiles + 1) * sizeof(char *));
                    g_ppFiles[g_nFiles++] = _strdup(szPath);
                }
            } while (FindNextFileA(hFind, &find));
            FindClose(hFind);
        }
        SetCurrentDirectoryA(szDir);
    }
}

static Bool do_load_image_from_dir(
    async_load_state *state, Screen *screen, Window window, Drawable target,
    const char *dir, int width, int height, char **name_ret)
{
    static Bool b = False;
    int i;
    HBITMAP hbm;

    if (!b)
    {
        recurse_dir(dir);
        if (g_nFiles > 4)
            b = True;
    }

    if (g_nFiles != 0)
    {
        i = random() % g_nFiles;

        hbm = do_load_picture(g_ppFiles[i]);
        if (hbm != NULL)
        {
            *name_ret = _strdup(strrchr(g_ppFiles[i], '\\') + 1);
            return do_load_image(state, screen, window, target, hbm, width, height);
        }
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
