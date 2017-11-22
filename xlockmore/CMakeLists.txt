add_library(xlockmore STATIC
    colors.c
    hsv.c
    minixpm.c
    wgl.c
    xcolor.c
    ximage.c
    xlockmore.c
    xws2win.c
    yarandom.c)
target_link_libraries(xlockmore comctl32)
