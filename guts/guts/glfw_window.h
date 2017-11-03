//
// Represents a GLFW window.
//

#ifndef GUTS_GLFW_WINDOW_H
#define GUTS_GLFW_WINDOW_H

#include <string>

#include <glload/gl_4_1.hpp>
#include <GLFW/glfw3.h>

namespace guts {

class GlfwWindow;

// A function which GlfwWindow can use as a renderer in the event loop.
typedef void(*GlfwWindowRenderFunc)(GlfwWindow *window);

// Represents a GLFW window by wrapping part of GLFW's C API.
class GlfwWindow {

 public:
  GlfwWindow(int window_width, int window_height, std::string initial_title);
  ~GlfwWindow();

  // Sets GLFWs error callback *globally* - this affects all of GLFW, even
  // sections that don't touch the window! A sane default is set by
  // guts::init::InitGLFW().
  static void SetGLFWErrorCallback(GLFWerrorfun callback);

  void SetReshapeCallback(GLFWframebuffersizefun callback);
  void SetKeyCallback(GLFWkeyfun callback);
  void SetRenderer(GlfwWindowRenderFunc fn);
  void EventLoop();
  void SetWindowShouldClose(bool window_should_close);

  // Gets the raw window that this object represents. DO NOT USE UNLESS
  // ABSOLUTELY NEEDED! Prefer to use other methods on this object.
  GLFWwindow *GetRawWindow();

 private:
  GLFWwindow *window;
  int width;
  int height;
  GlfwWindowRenderFunc render_func = [](GlfwWindow *window) {};

};

} // namespace guts

#endif //GUTS_GLFW_WINDOW_H
