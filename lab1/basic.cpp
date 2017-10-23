/*
 Basic example to demonstrate using a GLFW wrapper
 class with an OpenGL 4.x examples
*/

/* Include the header to the GLFW wrapper class which
   also includes the OpenGL extension initialisation*/
#include <iostream>

#include <guts/guts.h>

GLuint positionBufferObject;
GLuint program;
GLuint vao;

using namespace std;


//Called to update the display.
//You should call glfwSwapBuffers() after all of your rendering to display what you rendered.
void display(guts::GlfwWindow *window) {
  gl::ClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  gl::Clear(gl::COLOR_BUFFER_BIT);

  gl::UseProgram(program);

  gl::BindBuffer(gl::ARRAY_BUFFER, positionBufferObject);
  gl::EnableVertexAttribArray(0);
  gl::VertexAttribPointer(0, 4, gl::FLOAT, gl::FALSE_, 0, 0);

  gl::DrawArrays(gl::TRIANGLES, 0, 3);

  gl::DisableVertexAttribArray(0);
  gl::UseProgram(0);
}

/* Called whenever the window is resized. The new window size is given, in pixels. */
static void reshape(GLFWwindow *window, int w, int h) {
  gl::Viewport(0, 0, (GLsizei) w, (GLsizei) h);
}

/* change view angle, exit upon ESC */
static void keyCallback(GLFWwindow *window,
                        int k,
                        int s,
                        int action,
                        int mods) {
  if (action != GLFW_PRESS) return;

  cout << "KEY: " << (char) k << endl;

  if (k == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, gl::TRUE_);
}

/* Entry point of program */
int main(int argc, char *argv[]) {
  auto *window = new guts::GlfwWindow(1024, 768, "Hello Graphics World");

  window->SetRenderer(display);
  window->SetKeyCallback(keyCallback);
  window->SetReshapeCallback(reshape);

  gl::GenVertexArrays(1, &vao);
  gl::BindVertexArray(vao);

  float vertexPositions[] = {
      0.75f, 0.75f, 0.0f, 1.0f,
      0.75f, -0.75f, 0.0f, 1.0f,
      -0.75f, -0.75f, 0.0f, 1.0f,
  };

  gl::GenBuffers(1, &positionBufferObject);
  gl::BindBuffer(gl::ARRAY_BUFFER, positionBufferObject);
  gl::BufferData(gl::ARRAY_BUFFER,
                 sizeof(vertexPositions),
                 vertexPositions,
                 gl::STATIC_DRAW);
  gl::BindBuffer(gl::ARRAY_BUFFER, 0);

  try {
    auto builder = new guts::GLProgramBuilder();
    builder->AddShader(gl::VERTEX_SHADER, "basic.vert");
    builder->AddShader(gl::FRAGMENT_SHADER, "basic.frag");
    program = builder->BuildProgram();
    delete(builder);
  }
  catch (exception &e) {
    cout << "Caught exception: " << e.what() << endl;
    cin.ignore();
    delete (window);
    exit(0);
  }

  window->EventLoop();

  return 0;
}



