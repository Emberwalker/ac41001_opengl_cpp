set(TEX_CHECKER_SRCS
        texture_checker.cpp)
set(TEX_CHECKER_SHADERS
        texture_checker.frag
        texture_checker.vert)

# Move working dir for CLion.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_executable(texture_checker ${TEX_CHECKER_SRCS})
target_link_libraries(texture_checker ${ADDITIONAL_LIBRARIES})
foreach(CURR_SHADER ${TEX_CHECKER_SHADERS})
    configure_file(${CURR_SHADER} ${CURR_SHADER} COPYONLY)
endforeach()
