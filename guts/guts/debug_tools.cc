#include "debug_tools.h"

#include <iostream>
#include <glload/gl_4_1.hpp>

namespace guts {

bool PrintOpenGLErrors() {
  GLenum err;
  bool errored = false;
  while ((err = gl::GetError()) != gl::NO_ERROR_) {
    std::cerr << "OpenGL error " << std::hex << err << std::endl;
    errored = true;
  }
  return errored;
}

} // namespace guts