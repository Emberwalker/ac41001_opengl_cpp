# AC41001 Graphics Lab Work

## Building
Use CMake to generate appropriate build files for your platform. The generator definitely works on Windows/MinGW and
macOS High Sierra. Elsewhere your mileage may vary.

## Code Style
This project aims to follow the [Google C++ Style Guide](http://google.github.io/styleguide/cppguide.html) as far as
practical. If using CLion, you can set this in the Editor Code Style preferences using the 'Set from...' option.

Notable exceptions:
- We use exceptions. We don't have legacy code.
- No restrictions on Boost usage.
- C++14 usage.