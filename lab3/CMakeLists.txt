set(LAB3_SRCS
        lab3.cpp)
set(LAB3_SHADERS
        lab3.frag
        lab3.vert)

# Move working dir for CLion.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_executable(lab3 ${LAB3_SRCS})
target_link_libraries(lab3 ${ADDITIONAL_LIBRARIES})
foreach(CURR_SHADER ${LAB3_SHADERS})
    configure_file(${CURR_SHADER} ${CURR_SHADER} COPYONLY)
endforeach()
