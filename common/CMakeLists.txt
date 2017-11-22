add_library(common STATIC
    colors.c
    hsv.c
    logo.c
    minixpm.c
    mzc2mini.cpp
    pixmap.c
    spline.c
    textclient.cpp
    xcolor.c
    ximage.c
    xpm-pixmap.c
    xws2win.c
    yarandom.c)
target_include_directories(common PRIVATE ../include)
target_link_libraries(common comctl32)
