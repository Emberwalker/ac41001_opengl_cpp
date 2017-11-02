set(GUTS_H
        guts/guts.h guts/init.h
        guts/glfw_window.h
        guts/gl_program_builder.h
        guts/gl_object.h
        guts/objs/cube.h
        guts/internal/render.h
        guts/objs/sphere.h
        guts/internal/buffers.h
        guts/debug_tools.h
        guts/matrix_ops.h guts/objs/extrusion.h guts/uniform.h)
set(GUTS_CPP
        guts/init.cc
        guts/glfw_window.cc
        guts/gl_program_builder.cc
        guts/objs/cube.cc
        guts/objs/sphere.cc
        guts/objs/extrusion.cc
        guts/debug_tools.cc guts/uniform.cc)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_library(guts
    ${GUTS_H}
    ${GUTS_CPP})

install(TARGETS guts DESTINATION bin/guts)
install(FILES ${GUTS_H} DESTINATION include/guts)
