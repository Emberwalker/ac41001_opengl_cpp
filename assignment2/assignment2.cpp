/*
 * Assignment 1
 *
 * Sections copied from or based on Dr Iain Martin's materials.
 * Other sources noted where applicable.
 * This disclaimer also covers the entire GUTS static library.
 *
 * @author Robert T. <arkan@drakon.io>
 */

#include <guts/guts.h>
#include <guts/objs/tube.h>
#include <guts/objs/cube.h>
#include <guts/objs/sphere.h>
#include <guts/objs/ltree.h>

#include <memory>
#include <stack>
#include <cmath>
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Uniform aliases. Without these, the lines making/editing them get silly long.
template<typename T>
using UniformPtr = std::shared_ptr<guts::GLUniform<T>>;

template<typename T>
inline UniformPtr<T> GetNewUniform(GLuint program, const std::string &name) {
  return std::make_shared<guts::GLUniform<T>>(program, name);
}

// Useful constants
glm::vec3 AXIS_X = glm::vec3(1, 0, 0);
glm::vec3 AXIS_Y = glm::vec3(0, 1, 0);
glm::vec3 AXIS_Z = glm::vec3(0, 0, 1);
const float VIEW_INC = 0.05f;

// based on https://stackoverflow.com/a/35651717
template<typename T>
T const PI = std::acos(-T(1));

// GL Globals
GLuint program_std, program_show_normals, vao;
GLuint window_width = 640, window_height = 480;
GLuint colourmode = 1, lightmode = 0;
GLfloat aspect_ratio = 640.f / 480.f;
GLfloat view_x = 0.f, view_y = 0.f, view_z = 0.f;
GLfloat light_x = 0, light_y = 0, light_z = -3;
guts::GLRenderMode render_mode = guts::RENDER_NORMAL;
float time_period = 5.0;

// GL Uniforms
UniformPtr<glm::mat4> model_uniform, view_uniform,
    projection_uniform;
UniformPtr<glm::mat3> normal_matrix_uniform;
UniformPtr<GLuint> colour_mode_uniform, emit_mode_uniform, light_mode_uniform;
UniformPtr<glm::vec4> light_pos_uniform;

// Scene objects
std::unique_ptr<guts::objs::LTree> sample_tree;
std::unique_ptr<guts::objs::Sphere> light_sphere;


// Initialiser function.
static void Init(__unused guts::GlfwWindow *window) {
  // Generate abd bind our VAO
  gl::GenVertexArrays(1, &vao);
  gl::BindVertexArray(vao);

  // Build the program
  guts::GLProgramBuilder builder;
  builder.AddShader(gl::VERTEX_SHADER, "standard.vert");
  builder.AddShader(gl::FRAGMENT_SHADER, "standard.frag");
  program_std = builder.BuildProgram();

  // Generate uniform objects
  model_uniform = GetNewUniform<glm::mat4>(program_std, "model");
  view_uniform = GetNewUniform<glm::mat4>(program_std, "view");
  projection_uniform = GetNewUniform<glm::mat4>(program_std, "projection");
  normal_matrix_uniform = GetNewUniform<glm::mat3>(program_std, "normalmatrix");
  colour_mode_uniform = GetNewUniform<GLuint>(program_std, "colourmode");
  emit_mode_uniform = GetNewUniform<GLuint>(program_std, "emitmode");
  light_mode_uniform = GetNewUniform<GLuint>(program_std, "lightmode");
  light_pos_uniform = GetNewUniform<glm::vec4>(program_std, "lightpos");

  // Generate scene objects
  sample_tree = std::make_unique<guts::objs::LTree>("F[+F][-F+F]F[+F]");
  light_sphere = std::make_unique<guts::objs::Sphere>(100, 100);
}

// Updates all uniforms and then renders a given GLObject.
static void UpdateAndRender(const UniformPtr<glm::mat3> &u_normal_matrix,
                            const UniformPtr<glm::mat4> &u_model,
                            const UniformPtr<GLuint> &u_emitmode,
                            glm::mat4 &view, glm::mat4 &model,
                            guts::objs::GLObject &obj,
                            bool emitmode = false) {
  u_model->Set(model);
  glm::mat3 normal_matrix = guts::InverseTranspose(glm::mat3(view * model));
  u_normal_matrix->Set(normal_matrix);
  GLuint mode = 0;
  if (emitmode) mode = 1;
  u_emitmode->Set(mode);
  obj.Render(render_mode);
  mode = 0;
}

// Updates all uniforms and then renders a given MultipartGLObject (e.g. LTrees)
static void UpdateAndRender(const UniformPtr<glm::mat3> &u_normal_matrix,
                            const UniformPtr<glm::mat4> &u_model,
                            const UniformPtr<GLuint> &u_emitmode,
                            glm::mat4 &view, glm::mat4 &model,
                            guts::objs::MultipartGLObject &obj,
                            bool emitmode = false) {
  GLuint mode = 0;
  if (emitmode) mode = 1;
  u_emitmode->Set(mode);
  obj.Render(render_mode, *model_uniform, model, *normal_matrix_uniform, view);
  mode = 0;
}

// Limits a numeric type to between min and max, much like the GLSL clamp()
template <typename T>
static T Clamp(T val, T min, T max) {
  return std::max(min, std::min(max, val));
}

static void Draw(const UniformPtr<glm::mat4> &u_model,
                 const UniformPtr<glm::mat4> &u_view,
                 glm::mat4 initial_view,
                 const UniformPtr<glm::mat3> &u_normal_matrix,
                 const UniformPtr<GLuint> &u_emitmode,
                 bool render_light_src, glm::vec4 light_pos) {
  // The GL transform stack.
  std::stack<glm::mat4> model;
  model.push(glm::mat4(1.0));

  glm::vec3 light_pos3 = glm::vec3(light_pos) / light_pos.z;

  // Set constant uniforms
  u_view->Set(initial_view);

  // Light source
  if (render_light_src) {
    model.push(model.top());
    {
      model.top() = glm::translate(model.top(), light_pos3);
      model.top() = glm::scale(model.top(), glm::vec3(0.05f, 0.05f, 0.05f));
      UpdateAndRender(u_normal_matrix, u_model, u_emitmode, initial_view,
                      model.top(), *light_sphere, true);
    }
    model.pop();
  }

  // Tree
  model.push(model.top());
  {
    model.top() = glm::scale(model.top(), glm::vec3(0.1f));
    UpdateAndRender(u_normal_matrix, u_model, u_emitmode, initial_view,
                    model.top(), *sample_tree, false);
  }
  model.pop();
}

// Rendering handler for GlfwWindow.
static void Display(guts::GlfwWindow *window) {
  gl::ClearColor(0, 0, 0, 1);
  gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);
  gl::Enable(gl::DEPTH_TEST);
  gl::UseProgram(program_std);

  // Fetch the time from GLFW.
  auto time = static_cast<float>(std::fmod(window->GlfwTimer(), time_period));
  time /= time_period;

  // For sin/cos/etc based transforms
  float time_pi = time * 2 * PI<float>;

  // The GL transform stack.
  std::stack<glm::mat4> model;
  model.push(glm::mat4(1.0));

  glm::mat4 projection = glm::perspective(glm::radians(30.f), aspect_ratio, 0.1f, 100.f);
  glm::mat4 view = glm::lookAt(
      glm::vec3(0,9,9),
      glm::vec3(0,0,0),
      glm::vec3(0,1,0)
  );

  // Rotate the view with user controls on each axis.
  view = glm::rotate(view, view_x, AXIS_X);
  view = glm::rotate(view, view_y, AXIS_Y);
  view = glm::rotate(view, view_z, AXIS_Z);

  glm::vec4 light_pos = view * glm::vec4(light_x, light_y, light_z, 1.0);
  glm::vec3 light_pos3 = glm::vec3(light_x, light_y, light_z);

  // Set constant uniforms
  //view_uniform->Set(view);
  projection_uniform->Set(projection);
  light_pos_uniform->Set(light_pos);
  colour_mode_uniform->Set(colourmode);
  light_mode_uniform->Set(lightmode);

  Draw(model_uniform, view_uniform, view, normal_matrix_uniform,
       emit_mode_uniform, true, light_pos);

  gl::UseProgram(0);
}

// GlfwWindow reshape handler.
static void Reshape(GLFWwindow *window, int w, int h) {
  gl::Viewport(0, 0, (GLsizei) w, (GLsizei) h);
  aspect_ratio = ((float) w / 640.f * 4.f) / ((float) h / 480.f * 3.f);
}

// GlfwWindow handler for key presses.
static void KeyCallback(GLFWwindow *window,
                        int key,
                        __unused int s,
                        int action,
                        __unused int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, gl::TRUE_);

  if (key == GLFW_KEY_Q) view_x += VIEW_INC;
  if (key == GLFW_KEY_A) view_x -= VIEW_INC;
  if (key == GLFW_KEY_W) view_y += VIEW_INC;
  if (key == GLFW_KEY_S) view_y -= VIEW_INC;
  if (key == GLFW_KEY_E) view_z += VIEW_INC;
  if (key == GLFW_KEY_D) view_z -= VIEW_INC;
  if (key == '1') light_x -= VIEW_INC;
  if (key == '2') light_x += VIEW_INC;
  if (key == '3') light_y -= VIEW_INC;
  if (key == '4') light_y += VIEW_INC;
  if (key == '5') light_z -= VIEW_INC;
  if (key == '6') light_z += VIEW_INC;
  if (key == '=') time_period -= 1.f;
  if (key == '-') time_period += 1.f;

  if (time_period <= 0.f) time_period = 0.5f;

  if (key == 'M' && action != GLFW_PRESS) {
    colourmode++;
    if (colourmode > 2) colourmode = 0;
  }

  if (key == ',' && action != GLFW_PRESS) {
    render_mode = guts::NextRenderMode(render_mode);
  }

  if (key == '.' && action != GLFW_PRESS) {
    lightmode++;
    if (lightmode > 1) lightmode = 0;
  }
}

int main(int argc, char *argv[]) {
  guts::GlfwWindow *glw = new guts::GlfwWindow(1024, 768, "Assignment 2");

  glw->SetRenderer(Display);
  glw->SetKeyCallback(KeyCallback);
  glw->SetReshapeCallback(Reshape);

  Init(glw);

  glw->EventLoop();

  delete(glw);
  return EXIT_SUCCESS;
}