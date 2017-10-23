#include "init.h"

#include <iostream>

#include <GLFW/glfw3.h>

namespace guts {
namespace init {

namespace {

bool opengl_inited = false;
bool glfw_inited = false;

} // namespace

void InitOpenGL() {
  if (opengl_inited) return;
  if (!glload::LoadFunctions()) {
    std::cerr << "LoadFunctions() failed. Exiting." << std::endl;
    glfwTerminate();
    return;
  }
  opengl_inited = true;
}

void InitGLFW() {
  if (glfw_inited) return;
  if (glfwInit() != GLFW_TRUE) {
    std::cout << "Failed to initialize GLFW." << std::endl;
    exit(EXIT_FAILURE);
  }

  glfwSetErrorCallback([](int errCode, const char *description) {
    std::cout << "GLFW error " << errCode << ": " << description << std::endl;
  });
  glfw_inited = true;
}

} // namespace init
} // namespace guts