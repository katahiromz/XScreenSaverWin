add_executable(xscreensaver WIN32 xscreensaver.cpp xscreensaver_res.rc)
target_link_libraries(xscreensaver PRIVATE comctl32)
