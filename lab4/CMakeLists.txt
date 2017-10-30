set(LAB4_SRCS
        lab4.cpp)
set(LAB4_SHADERS
        lab4.frag
        lab4.vert)

# Move working dir for CLion.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_executable(lab4 ${LAB4_SRCS})
target_link_libraries(lab4 ${ADDITIONAL_LIBRARIES})
foreach(CURR_SHADER ${LAB4_SHADERS})
    configure_file(${CURR_SHADER} ${CURR_SHADER} COPYONLY)
endforeach()
