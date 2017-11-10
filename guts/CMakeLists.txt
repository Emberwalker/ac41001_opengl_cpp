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
        guts/matrix_ops.h
        guts/objs/extrusion.h
        guts/uniform.h guts/internal/containers.h
        guts/objs/tube.h
        guts/tex/texture_loader.h
        guts/tex/png_loader.h guts/gl_texture.h)
set(GUTS_CPP
        guts/init.cc
        guts/glfw_window.cc
        guts/gl_program_builder.cc
        guts/gl_object.cc
        guts/objs/cube.cc
        guts/objs/sphere.cc
        guts/objs/extrusion.cc
        guts/debug_tools.cc
        guts/uniform.cc
        guts/objs/tube.cc
        guts/tex/texture_loader.cc guts/tex/png_loader.cc guts/gl_texture.cc)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_library(guts
    ${GUTS_H}
    ${GUTS_CPP})

install(TARGETS guts DESTINATION bin/guts)
install(FILES ${GUTS_H} DESTINATION include/guts)
