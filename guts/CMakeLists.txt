set(GUTS_H
        guts/wrapper_glfw.h guts/guts.h guts/init.h)
set(GUTS_CPP
        guts/wrapper_glfw.cpp guts/init.cc)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_library(guts
    ${GUTS_H}
    ${GUTS_CPP})

install(TARGETS guts DESTINATION bin/guts)
install(FILES ${GUTS_H} DESTINATION include/guts)
