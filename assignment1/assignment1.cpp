#include <stack>

#include <guts/guts.h>
#include <guts/objs/extrusion.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

template<typename T>
using UniformPtr = std::unique_ptr<guts::GLUniform<T>>;

template<typename T>
inline UniformPtr<T> GetNewUniform(GLuint program, const std::string &name) {
  return std::make_unique<guts::GLUniform<T>>(program, name);
}

// GL Globals
GLuint program, vao;
GLuint window_width = 640, window_height = 480;
GLfloat aspect_ratio = 640.f / 480.f;
GLfloat light_x = 0, light_y = 2, light_z = 0;

// GL Uniforms
UniformPtr<glm::mat4> model_uniform, view_uniform,
    projection_uniform;
UniformPtr<glm::mat3> normal_matrix_uniform;
UniformPtr<GLuint> colour_mode_uniform;
UniformPtr<glm::vec3> light_pos_uniform;

// Scene objects
std::unique_ptr<guts::objs::ExtrudedObject> pumpjack_head, pumpjack_frame,
    pumpjack_counterweight_left, pumpjack_counterweight_right,
    pumpjack_pitman_arm_left, pumpjack_pitman_arm_right;

std::unique_ptr<guts::objs::ExtrudedObject> TEMPORARY;

static void Init(guts::GlfwWindow *window) {
  // TODO(arkan): Implement.

  gl::GenVertexArrays(1, &vao);
  gl::BindVertexArray(vao);

  guts::GLProgramBuilder builder;
  builder.AddShader(gl::VERTEX_SHADER, "assignment1.vert");
  builder.AddShader(gl::FRAGMENT_SHADER, "assignment1.frag");
  program = builder.BuildProgram();

  model_uniform = GetNewUniform<glm::mat4>(program, "model");
  view_uniform = GetNewUniform<glm::mat4>(program, "view");
  projection_uniform = GetNewUniform<glm::mat4>(program, "projection");
  normal_matrix_uniform = GetNewUniform<glm::mat3>(program, "normalmatrix");
  colour_mode_uniform = GetNewUniform<GLuint>(program, "colourmode");
  light_pos_uniform = GetNewUniform<glm::vec3>(program, "lightpos");

  std::vector<GLfloat> vertices = {0,0,0, 1,0,0, 0,0,1};
  std::vector<GLfloat> edges = {0,0,0, 1,0,0, 0,0,1};
  TEMPORARY = std::make_unique<guts::objs::ExtrudedObject>(vertices, edges,
                                                           glm::vec4(1,1,1,1));
}

static void Display(guts::GlfwWindow *window) {
  gl::ClearColor(0, 0, 0, 1);
  gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);
  gl::Enable(gl::DEPTH_TEST);
  gl::UseProgram(program);

  std::stack<glm::mat4> model;
  model.push(glm::mat4(1.0));

  glm::mat3 normal_matrix;
  glm::mat4 projection = glm::perspective(30.f, aspect_ratio, 0.1f, 100.f);
  glm::mat4 view = glm::lookAt(
      glm::vec3(0,0,4),
      glm::vec3(0,0,0),
      glm::vec3(0,1,0)
  );

  glm::vec4 light_pos4 = view * glm::vec4(light_x, light_y, light_z, 1.0);

  view_uniform->Set(view);
  projection_uniform->Set(projection);
  glm::vec3 light_pos = glm::vec3(light_pos4) / light_pos4.w;
  light_pos_uniform->Set(light_pos);

  // TODO(arkan) do properly.
  model_uniform->Set(model.top());
  normal_matrix = guts::InverseTranspose(glm::mat3(view * model.top()));
  normal_matrix_uniform->Set(normal_matrix);
  TEMPORARY->Render(guts::RENDER_WIREFRAME);

  gl::UseProgram(0);
}

// Called whenever the window is resized. The new window size is given, in pixels.
static void reshape(GLFWwindow *window, int w, int h) {
  gl::Viewport(0, 0, (GLsizei) w, (GLsizei) h);
  aspect_ratio = ((float) w / 640.f * 4.f) / ((float) h / 480.f * 3.f);
}

static void keyCallback(GLFWwindow *window,
                        int key,
                        int s,
                        int action,
                        int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, gl::TRUE_);
}

int main(int argc, char *argv[]) {
  guts::GlfwWindow *glw = new guts::GlfwWindow(1024, 768, "Assignment 1");

  glw->SetRenderer(Display);
  glw->SetKeyCallback(keyCallback);
  glw->SetReshapeCallback(reshape);

  Init(glw);

  glw->EventLoop();

  delete (glw);
  return 0;
}