set(AS2_SRCS
        assignment2.cpp)
set(AS2_SHADERS
        standard.frag
        standard.vert
        show_normals.frag
        show_normals.geom
        show_normals.vert)

# Move working dir for CLion.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_executable(assignment2 ${AS2_SRCS})
target_link_libraries(assignment2 ${ADDITIONAL_LIBRARIES})
foreach(CURR_SHADER ${AS2_SHADERS})
    configure_file(${CURR_SHADER} ${CURR_SHADER} COPYONLY)
endforeach()
