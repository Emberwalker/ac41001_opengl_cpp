set(GUTS_H
        guts/wrapper_glfw.h)
set(GUTS_CPP
        guts/wrapper_glfw.cpp)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_library(guts
    ${GUTS_H}
    ${GUTS_CPP})

install(TARGETS guts DESTINATION bin/guts)
install(FILES guts/wrapper_glfw.h DESTINATION include/guts)
