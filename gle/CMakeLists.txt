find_package(OpenGL)
include_directories(glextrusion/src/GL ${OPENGL_INCLUDE_DIR})

add_definitions(-DOPENGL_10=1)

add_library(gle STATIC
      glextrusion/src/ex_alpha.c
      glextrusion/src/ex_angle.c
      glextrusion/src/ex_cut_round.c
      glextrusion/src/ex_raw.c
      glextrusion/src/extrude.c
      glextrusion/src/intersect.c
      glextrusion/src/qmesh.c
      glextrusion/src/rot_prince.c
      glextrusion/src/rotate.c
      glextrusion/src/round_cap.c
      glextrusion/src/segment.c
      glextrusion/src/texgen.c
      glextrusion/src/urotate.c
      glextrusion/src/view.c)
target_link_libraries(gle ${OPENGL_LIBRARIES})
