set(LAB1_SRCS
        basic.cpp)
set(LAB1_SHADERS
        basic.frag
        basic.vert)

# Move working dir for CLion.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_executable(lab1 ${LAB1_SRCS})
target_link_libraries(lab1 ${ADDITIONAL_LIBRARIES})
foreach(CURR_SHADER ${LAB1_SHADERS})
    configure_file(${CURR_SHADER} ${CURR_SHADER} COPYONLY)
endforeach()
