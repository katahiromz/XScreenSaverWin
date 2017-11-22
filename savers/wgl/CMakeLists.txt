add_xlockmore_screensaver(companion WIN32
    companion.c 
    companion_disc.c 
    companion_heart.c 
    companion_quad.c 
    gllist.c 
    rotator.c
    wgl.c)
target_link_libraries(companion ${OPENGL_LIBRARIES})
