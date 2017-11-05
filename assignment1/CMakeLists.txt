set(AS1_SRCS
        assignment1.cpp
        assignment1_pumpjack_head.inl
        assignment1_a_frame.inl
        assignment1_counterweight.inl)
set(AS1_SHADERS
        assignment1.frag
        assignment1.vert)

# Move working dir for CLion.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_executable(assignment1 ${AS1_SRCS})
target_link_libraries(assignment1 ${ADDITIONAL_LIBRARIES})
foreach(CURR_SHADER ${AS1_SHADERS})
    configure_file(${CURR_SHADER} ${CURR_SHADER} COPYONLY)
endforeach()
