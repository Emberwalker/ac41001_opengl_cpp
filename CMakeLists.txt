# Based on the following:
#  https://gitlab.kitware.com/cmake/cmake/tree/master/Tests/Tutorial
#  https://github.com/progschj/OpenGL-Examples/blob/master/CMakeLists.txt
#  ... And others.
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(AC41001_OPENGL)

# opengl32/libGL/OpenGL Framework
find_package(OpenGL REQUIRED)
set(ADDITIONAL_LIBRARIES ${OPENGL_LIBRARIES})
include_directories(${OPENGL_INCLUDE_DIRS})

# Require C++14
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_STANDARD 17)

# User settings
set(CLANG_GCC_ADDITIONAL_FLAGS "-Wall -pedantic" CACHE STRING "Additional flags when compiling with Clang or GCC.")
set(MSVC_ADDITIONAL_FLAGS "" CACHE STRING "Additional flags when compiling with the Microsoft Visual C++ Compiler (Visual Studio/msbuild)")
set(FORCE_32BIT FALSE CACHE BOOL "Force building 32-bit even on a 64-bit host.")

# Compiler identification
if (CMAKE_COMPILER_IS_GNUCXX AND NOT WIN32)
    message(WARNING "GCC isn't what this project was tested on! There's no guarantee this will work correctly. Switch to Clang if possible.")
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${CLANG_GCC_ADDITIONAL_FLAGS})
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${CLANG_GCC_ADDITIONAL_FLAGS})
elseif (MSVC)
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${MSVC_ADDITIONAL_FLAGS})
endif ()

# Platform
set(LIB_PATH "")
set(OTHER_LIBS glload glfw3 boost_exception boost_filesystem boost_system)
foreach (_LIB ${OTHER_LIBS})
    set(REL_DBG_LIBS ${REL_DBG_LIBS} debug "${_LIB}D" optimized ${_LIB})
endforeach ()
set(NO_ADD_LIB_PATH FALSE)

if (APPLE)
    # macOS/iOS/watchOS - but we only really care about macOS
    set(LIB_PATH darwin)
    # Add GLFW3 required frameworks. See http://www.glfw.org/docs/latest/build_guide.html#build_link_osx
    set(MACOS_FRAMEWORK_NAMES Cocoa IOKit CoreVideo)
    foreach (_FRAMEWORK ${MACOS_FRAMEWORK_NAMES})
        set(MACOS_FRAMEWORKS ${MACOS_FRAMEWORKS} "-framework ${_FRAMEWORK}")
    endforeach ()
    set(ADDITIONAL_LIBRARIES ${MACOS_FRAMEWORKS} ${OTHER_LIBS} ${ADDITIONAL_LIBRARIES})
    if (NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        message(FATAL_ERROR "Use Clang on macOS devices!")
    endif ()
elseif (WIN32)
    set(ADDITIONAL_LIBRARIES ${REL_DBG_LIBS} ${ADDITIONAL_LIBRARIES})
    if (MSYS OR MINGW)
        # UNIX-y in MSYS/MinGW
        set(LIB_PATH mingw)
    else ()
        # Windows-style in MSVC
        set(LIB_PATH win)
    endif ()
elseif (UNIX)
    # Probably Linux? Assume it is. We're not mad enough to try and support Sun et al.
    #set(LIB_PATH linux)
    #set(ADDITIONAL_LIBRARIES ${OTHER_LIBS} ${ADDITIONAL_LIBRARIES})
    # TODO: Compile linux32/64 libs. Until then, use system libs.
    SET(NO_ADD_LIB_PATH TRUE)
else ()
    message(WARNING "OS detection failed. This may not work well...")
    set(ADDITIONAL_LIBRARIES ${OTHER_LIBS} ${ADDITIONAL_LIBRARIES})
    set(NO_ADD_LIB_PATH TRUE)
endif ()

# Arch. On Windows, AMD64 == 64-bit, while on UNIXes x86_64 == 64-bit. Why CMake doesn't sanitise this I don't know.
set(BUILD_64BIT FALSE)
if (CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "AMD64" OR CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "x86_64")
    if (FORCE_32BIT)
        message(STATUS "32-bit build forced on a 64-bit host.")
    else ()
        message(STATUS "Building 64-bit executables.")
        set(BUILD_64BIT TRUE)
    endif ()
else ()
    message(STATUS "Non-64-bit CPU detected; assuming 32-bit executables.")
    message(WARNING "32-bit building isn't very well tested. Here be dragons.")
endif ()

# Libraries
if (NOT NO_ADD_LIB_PATH)
    if (BUILD_64BIT)
        set(LIB_PATH ${LIB_PATH}64)
    else ()
        set(LIB_PATH ${LIB_PATH}32)
    endif ()
    link_directories(lib/${LIB_PATH})
    # Add the include dir here, since no lib path means the system might have its own appropriate headers for its libs.
    include_directories(include)
endif ()


# Gl UTilitieS
include_directories("${PROJECT_SOURCE_DIR}/guts")
add_subdirectory(guts)
set(ADDITIONAL_LIBRARIES guts ${ADDITIONAL_LIBRARIES})

# Programs!
add_subdirectory(lab1)
add_subdirectory(lab3)
add_subdirectory(lab4)
add_subdirectory(texture_checker)
add_subdirectory(assignment1)