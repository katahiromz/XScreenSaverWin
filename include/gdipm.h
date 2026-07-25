// gdipm.h --- GDI plus minus
// Author: katahiromz
// License: MIT

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef DWORD ARGB;

static __inline ARGB
MakeARGB(BYTE a, BYTE r, BYTE g, BYTE b)
{
    ARGB a0 = a, r0 = r, g0 = g, b0 = b;
    return (a0 << 24) | (r0 << 16) | (g0 << 8) | b0;
}

#ifdef __cplusplus
    #define GDIPM_OPTIONAL(opt) = (opt)
#else
    #define GDIPM_OPTIONAL(opt)
#endif

HRESULT gdipm_init_ex(void **pgdipm);
void gdipm_exit_ex(void *gdipm);

#ifdef GDIPM_NO_DPI
    HBITMAP gdipm_load_pic(void *gdipm, const WCHAR *image_filename,
                           ARGB back_color GDIPM_OPTIONAL(MakeARGB(0xFF, 0xFF, 0xFF, 0xFF)));
    BOOL gdipm_save_pic(void *gdipm, const WCHAR *image_filename, HBITMAP hBitmap);
#else
    HBITMAP gdipm_load_pic(void *gdipm, const WCHAR *image_filename,
                           ARGB color GDIPM_OPTIONAL(MakeARGB(0xFF, 0xFF, 0xFF, 0xFF)),
                           float *x_dpi GDIPM_OPTIONAL(NULL),
                           float *y_dpi GDIPM_OPTIONAL(NULL));
    BOOL gdipm_save_pic(void *gdipm, const WCHAR *image_filename, HBITMAP hBitmap,
                        float x_dpi GDIPM_OPTIONAL(0),
                        float y_dpi GDIPM_OPTIONAL(0));
#endif

#ifdef __cplusplus
} // extern "C"
#endif
