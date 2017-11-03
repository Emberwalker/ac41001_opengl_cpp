#include <guts/guts.h>
#include <guts/objs/extrusion.h>
#include <guts/objs/sphere.h>

#include <memory>
#include <stack>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

template<typename T>
using UniformPtr = std::unique_ptr<guts::GLUniform<T>>;

template<typename T>
inline UniformPtr<T> GetNewUniform(GLuint program, const std::string &name) {
  return std::make_unique<guts::GLUniform<T>>(program, name);
}

// Useful constants
constexpr glm::vec3 AXIS_X = glm::vec3(1, 0, 0);
constexpr glm::vec3 AXIS_Y = glm::vec3(0, 1, 0);
constexpr glm::vec3 AXIS_Z = glm::vec3(0, 0, 1);
const float VIEW_INC = 0.05f;

// GL Globals
GLuint program, vao;
GLuint window_width = 640, window_height = 480;
GLuint colourmode = 0;
GLfloat aspect_ratio = 640.f / 480.f;
GLfloat view_x = 0.f, view_y = 0.f, view_z = 0.f;
GLfloat light_x = 0, light_y = 0, light_z = 0;
GLfloat TEMP_x = 0;
guts::GLRenderMode render_mode = guts::RENDER_NORMAL;

int ticks = 0;

// GL Uniforms
UniformPtr<glm::mat4> model_uniform, view_uniform,
    projection_uniform;
UniformPtr<glm::mat3> normal_matrix_uniform;
UniformPtr<GLuint> colour_mode_uniform, emit_mode_uniform;
UniformPtr<glm::vec4> light_pos_uniform;

// Scene objects
std::unique_ptr<guts::objs::ExtrudedObject> pumpjack_head, pumpjack_frame,
    pumpjack_counterweight_left, pumpjack_counterweight_right,
    pumpjack_pitman_arm_left, pumpjack_pitman_arm_right;
std::unique_ptr<guts::objs::Sphere> light_sphere;

std::unique_ptr<guts::objs::ExtrudedObject> TEMPORARY;

static void Init(__unused guts::GlfwWindow *window) {
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
  emit_mode_uniform = GetNewUniform<GLuint>(program, "emitmode");
  light_pos_uniform = GetNewUniform<glm::vec4>(program, "lightpos");

  std::vector<GLfloat> vertices = {0,0,0, 1,0,0, 1,0,1};
  std::vector<GLfloat> edges = {0,0,0, 1,0,0, 1,0,1};
  TEMPORARY = std::make_unique<guts::objs::ExtrudedObject>(vertices, edges,
                                                           glm::vec4(1,1,1,1));

  light_sphere = std::make_unique<guts::objs::Sphere>(100, 100);
}

static void Display(__unused guts::GlfwWindow *window) {
  gl::ClearColor(0, 0, 0, 1);
  gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);
  gl::Enable(gl::DEPTH_TEST);
  gl::UseProgram(program);

  std::stack<glm::mat4> model;
  model.push(glm::mat4(1.0));

  glm::mat3 normal_matrix;
  glm::mat4 projection = glm::perspective(glm::radians(30.f), aspect_ratio, 0.1f, 100.f);
  glm::mat4 view = glm::lookAt(
      glm::vec3(0,0,8),
      glm::vec3(0,0,0),
      glm::vec3(0,1,0)
  );
  // Rotate the view with user controls on each axis.
  view = glm::rotate(view, view_x, AXIS_X);
  view = glm::rotate(view, view_y, AXIS_Y);
  view = glm::rotate(view, view_z, AXIS_Z);

  glm::vec4 light_pos = view * glm::vec4(light_x, light_y, light_z, 1.0);
  glm::vec3 light_pos3 = glm::vec3(light_pos) / light_pos.w;

  if (ticks % 1000) {
    ticks = 0;
    std::cout << "Light Pos: x"
              << light_pos3.x
              << " y" << light_pos3.y
              << " z" << light_pos3.z
              << std::endl;
  }

  view_uniform->Set(view);
  projection_uniform->Set(projection);
  light_pos_uniform->Set(light_pos);

  model.push(model.top());
  {
    model.top() = glm::translate(model.top(), light_pos3);
    model.top() = glm::scale(model.top(), glm::vec3(0.05f, 0.05f, 0.05f));
    model_uniform->Set(model.top());

    normal_matrix = guts::InverseTranspose(glm::mat3(view * model.top()));
    normal_matrix_uniform->Set(normal_matrix);

    GLuint mode = 1;
    emit_mode_uniform->Set(mode);
    light_sphere->Render(render_mode);
    mode = 0;
    emit_mode_uniform->Set(mode);
  }
  model.pop();

  // TODO(arkan) do properly.
  model.push(model.top());
  {
    model.top() = glm::translate(model.top(), glm::vec3(0, -2, 0));
    //model.top() = glm::rotate(model.top(), TEMP_x, AXIS_X);
    //model.top() = glm::rotate(model.top(), TEMP_x, AXIS_Y);
    model_uniform->Set(model.top());
    normal_matrix = guts::InverseTranspose(glm::mat3(view * model.top()));
    normal_matrix_uniform->Set(normal_matrix);
    TEMPORARY->Render(render_mode);
  }
  model.pop();

  gl::UseProgram(0);
  ticks++;
  TEMP_x += VIEW_INC;
}

// Called whenever the window is resized. The new window size is given, in pixels.
static void reshape(GLFWwindow *window, int w, int h) {
  gl::Viewport(0, 0, (GLsizei) w, (GLsizei) h);
  aspect_ratio = ((float) w / 640.f * 4.f) / ((float) h / 480.f * 3.f);
}

static void keyCallback(GLFWwindow *window,
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

  if (key == 'M' && action != GLFW_PRESS) {
    colourmode = static_cast<GLuint>(!colourmode);
  }

  if (key == ',' && action != GLFW_PRESS) {
    render_mode = guts::NextRenderMode(render_mode);
  }
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