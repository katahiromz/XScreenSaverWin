add_library(screenhack STATIC
    colors.c
    hsv.c
    minixpm.c
    screenhack.c
    xcolor.c
    ximage.c
    xws2win.c
    yarandom.c)
target_link_libraries(screenhack comctl32)
