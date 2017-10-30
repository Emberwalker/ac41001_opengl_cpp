#include "glfw_window.h"

#include <iostream>

#include "init.h"
#include "debug_tools.h"

namespace guts {

GlfwWindow::GlfwWindow(int window_width, int window_height,
                       std::string initial_title) {
  this->width = window_width;
  this->height = window_height;

  init::InitGLFW();

  glfwWindowHint(GLFW_SAMPLES, 4);
  // OpenGL 4.1 is maximum due to macOS.
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  // macOS also requires OpenGL 3.1+ contexts are Forward Compatible, else NSGL
  // throws a tantrum and GLFW can't open a window.
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef DEBUG
  glfwOpenWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif // DEBUG

  this->window = glfwCreateWindow(this->width, this->height,
                                  initial_title.c_str(), nullptr, nullptr);
  if (!this->window) {
    std::cout << "Could not open GLFW window." << std::endl;
  }

  glfwMakeContextCurrent(this->window);
  init::InitOpenGL();

  // Enable sticky keys in GLFW. See http://www.glfw.org/docs/latest/group__input.html#gaa92336e173da9c8834558b54ee80563b
  // for what GLFW_STICKY_KEYS does.
  glfwSetInputMode(this->window, GLFW_STICKY_KEYS, true);
}

GlfwWindow::~GlfwWindow() {
  glfwTerminate();
}

void GlfwWindow::SetGLFWErrorCallback(GLFWerrorfun callback) {
  glfwSetErrorCallback(callback);
}

void GlfwWindow::SetReshapeCallback(GLFWframebuffersizefun callback) {
  glfwSetFramebufferSizeCallback(this->window, callback);
}

void GlfwWindow::SetKeyCallback(GLFWkeyfun callback) {
  glfwSetKeyCallback(this->window, callback);
}

void GlfwWindow::SetRenderer(GlfwWindowRenderFunc fn) {
  this->render_func = fn;
}

void GlfwWindow::EventLoop() {
  while (!glfwWindowShouldClose(this->window)) {
    this->render_func(this);
    guts::PrintOpenGLErrors();

    glfwSwapBuffers(this->window);
    glfwPollEvents();
  }
}

void GlfwWindow::SetWindowShouldClose(bool window_should_close) {
  glfwSetWindowShouldClose(this->window, window_should_close);
}

GLFWwindow *GlfwWindow::GetRawWindow() {
  return this->window;
}

} // namespace guts