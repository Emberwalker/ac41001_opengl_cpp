# Library Rebuilding
These notes are roughly how to generate new library files for the `lib` directories.

# Win32/64
## GLLoad
1. Install Premake4 somewhere
2. Open a PowerShell/CMD session in the checkout of glsdk (the downloads on the site will do)
3. `premake4 vs2010`
4. Open the VS project, build GLLoad for Win32 Release and Debug
5. Open the Configuration Manager, open the Active Solution Platform dropdown and select New...
6. Set to copy from Win32, select x64 from the architecture dropdown, tick 'Create new project platforms' and click OK
7. Build GLLoad in X64 Release and Debug
8. Find the libraries:
    - 32-bit: `glload/lib`
    - 64-bit: `x64` Release and Debug subdirectories
    
## GLFW 3
1. Check out the appropriate Git revision you're after.
2. Run CMake with the Visual Studio generator with target directory `build` in the repo
    - Optionally: disable GLFW_BUILD_{DOCS,EXAMPLES,TESTS} and enable GLFW_USE_HYBRID_HPG to force dedicated GPU use on
        mixed GPU systems (e.g. Laptops with Intel and AMD/Nvidia GPUs)
3. Build Release and Debug with VS.
4. Copy libs out of build directory: `build/src`
5. Reset/Clean the repo
6. Repeat above but with the VS 64-bit generator.

## Boost
1. Open a Visual Studio 'Developer Command Prompt' from Start to use with the next step.
2. Follow [the official guidance](http://www.boost.org/doc/libs/1_65_1/more/getting_started/windows.html#prepare-to-use-a-boost-library-binary) 
but replace the `.\b2` command with `.\b2 --build-type=complete variant=debug,release threading=multi link=static
--with-exception --with-filesystem` and optionally a `-jX` flag to use `X` parallel jobs (recommended!).
3. Fetch libs from `$BOOST_ROOT/stage/lib` to `win32`
    - Note that Boost uses a weird naming convention for libraries!
    - Take the ones ending `-mt-{VERSION}` for non-debug and `-mt-gd-{VERSION}` for debug
    - Rename these files to the usual convention e.g. `boost_exceptionD.lib` for debug
4. `.\b2 clean`
4. Repeat the above, but add `address-model=64` to the `b2` commandline to generate x64 libs.

# MinGW64
_Note: MinGW 32-bit is untested as it would require a second install of MinGW to try out. I think?_
## GLLoad
1. Install Premake4 somewhere
2. Open a MinGW64 session in the checkout of glsdk (the downloads on the site will do)
3. `premake4 gmake`
4. `mingw32-make glload`
5. `mingw32-make glload config=release`
6. Retrieve libs from `glload/lib`

## GLFW3
1. Check out the appropriate Git revision you're after.
2. Run CMake with the MinGW Makefiles generator with target directory `build` in the repo with CMAKE_BUILD_TYPE=Release
    - Optionally: disable GLFW_BUILD_{DOCS,EXAMPLES,TESTS} and enable GLFW_USE_HYBRID_HPG to force dedicated GPU use on
    mixed GPU systems (e.g. Laptops with Intel and AMD/Nvidia GPUs)
3. In the `build` directory, `mingw32-make`
4. Retrieve the built lib from `build/src`
5. Reset/Clean the repo
6. Repeat above but substitute CMAKE_BUILD_TYPE=Debug

## Boost
1. Follow the steps to [install Boost.Build (b2)](http://www.boost.org/doc/libs/1_65_1/more/getting_started/windows.html#or-build-binaries-from-source)
2. Open a PowerShell/CMD session in `$BOOST_ROOT`
3. `b2 toolset=gcc --build-type=complete variant=debug,release threading=multi link=static --with-exception --with-filesystem`
    - Optionally: add a `-jX` flag to use `X` parallel jobs.
4. Retrieve libs from `$BOOST_ROOT/stage/lib`
    - Note that Boost uses a weird naming convention for libraries!
    - Take the ones ending `-mt-{VERSION}` for non-debug and `-mt-d-{VERSION}` for debug
    - Rename these files to the usual convention e.g. `libboost_exceptionD.a` for debug