# Library Rebuilding
These notes are roughly how to generate new library files for the `lib` directories.

## Win32/64

### GLLoad

1. Install Premake4 somewhere
2. Open a PowerShell/CMD session in the checkout of glsdk (the downloads on the site will do)
3. `premake4 vs2010`
4. Open the VS project, build GLLoad for Win32 Release
5. Open the Configuration Manager, open the Active Solution Platform dropdown and select New...
6. Set to copy from Win32, select x64 from the architecture dropdown, tick 'Create new project platforms' and click OK
7. Build GLLoad in X64 Release
8. Find the libraries:
    - 32-bit: `glload/lib`
    - 64-bit: `x64` Release subdirectory
    
### GLFW 3

1. Check out the appropriate Git revision you're after.
2. Run CMake with the Visual Studio generator with target directory `build` in the repo
    - Optionally: disable `GLFW_BUILD_{DOCS,EXAMPLES,TESTS}` and enable `GLFW_USE_HYBRID_HPG` to force dedicated GPU use on mixed GPU systems (e.g. Laptops with Intel and AMD/Nvidia GPUs)
3. Build Release with VS.
4. Copy libs out of build directory: `build/src`
5. Reset/Clean the repo
6. Repeat above but with the VS 64-bit generator.

### libpng

1. Download a ZIP distribution from [the libpng home page](www.libpng.org)
2. Open `projects/vstudio/zlib.props` in a competant code editor (e.g. VSCode, Atom)
3. Get the zlib version referenced in `ZLibSrcDir`, unpack it and update `ZLibSrcDir` to its path.
4. Open `projects/vstudio/vstudio.sln` in Visual Studio
5. Change to the "Release Library" configuration and build libpng
    - You may need to "Set Start-up Project" on libpng for this.
6. Open the Configuration Manager and add an X64 configuration in "Active solution platform", copying
from Win32.
7. Fetch Win32 libs from `projects\vstudio\Release Library` (zlib.lib _and_ libpng16.lib)
8. Fetch x64 libs from `projects\vstudio\x64\Release Library` (zlib.lib _and_ libpng16.lib)
9. For both Win32 and x64, rename `zlib.lib` to `z.lib` so CMake picks it up properly.
9. Grab `png.h` from the libpng archive root for include.

## MinGW64

_Note: MinGW 32-bit is untested as it would require a second install of MinGW to try out. I think?_

### GLLoad

1. Install Premake4 somewhere
2. Open a MinGW64 session in the checkout of glsdk (the downloads on the site will do)
3. `premake4 gmake`
4. `mingw32-make glload config=release`
5. Retrieve lib from `glload/lib`

### GLFW3

1. Check out the appropriate Git revision you're after.
2. Run CMake with the MinGW Makefiles generator with target directory `build` in the repo with CMAKE_BUILD_TYPE=Release
    - Optionally: disable `GLFW_BUILD_{DOCS,EXAMPLES,TESTS}` and enable `GLFW_USE_HYBRID_HPG` to force dedicated GPU use on mixed GPU systems (e.g. Laptops with Intel and AMD/Nvidia GPUs)
3. In the `build` directory, `mingw32-make`
4. Retrieve the built lib from `build/src`
5. Reset/Clean the repo
6. Repeat above but substitute CMAKE_BUILD_TYPE=Debug

### libpng

libpng needs zlib to work. MinGW seems to provide it, but for safety we'll build zlib too.

1. Download the latest zlib .tar.gz release from [the zlib home page](https://zlib.net/)
2. `mingw32-make.exe -f win32/Makefile.gcc`
3. Copy `libz.a` to appropriate place
1. Download the appropriate libpng release from [the libpng home page](http://www.libpng.org/pub/png/libpng.html)
2. `./configure --enable-static`
3. `mingw32-make` (optionally with `-j X` flag to use X jobs - good for multicore systems)
4. Fetch `.libs/libpng16.a` and `png.h` for appropriate folders.

## Darwin64 (macOS)

Before continuing, install the following Homebrew `brew` packages:

- `premake`
- `ninja`

### GLLoad

1. Open a shell in a checkout of glsdk (the downloads on the site will do)
2. `premake4 gmake`
3. `make glload config=release`
4. Retrieve `glload/lib/libglload.a`

### GLFW3

1. Check out the appropriate Git revision you're after.
2. Run CMake with the Ninja generator with target directory 'build' in the repo with `CMAKE_BUILD_TYPE=Release`
3. In the `build` directory, `ninja`
4. Retrieve `build/src/libglfw3.a`