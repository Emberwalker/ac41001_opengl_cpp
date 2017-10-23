//
// Init functions for OpenGL/GLFW/etc.
//

#ifndef GUTS_INIT_H
#define GUTS_INIT_H

#include <glload/gl_load.hpp>
#include <glload/gl_4_1.hpp>

namespace guts {
namespace init {

void InitOpenGL();
void InitGLFW();

} // namespace init
} // namespace guts

#endif //GUTS_INIT_H
