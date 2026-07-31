add_library(xlockmore STATIC xlockmore.c xlockmore_res.rc)
target_link_libraries(xlockmore comctl32 common)
