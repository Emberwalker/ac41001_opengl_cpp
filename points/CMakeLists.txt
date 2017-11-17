set(POINTS_SRCS
        points.cpp)
set(POINTS_SHADERS
        points.frag
        points_analytic.frag
        points.vert)

# Move working dir for CLion.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_executable(points ${POINTS_SRCS})
target_link_libraries(points ${ADDITIONAL_LIBRARIES})
foreach(CURR_SHADER ${POINTS_SHADERS})
    configure_file(${CURR_SHADER} ${CURR_SHADER} COPYONLY)
endforeach()
