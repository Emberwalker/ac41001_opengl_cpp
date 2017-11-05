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

#include <memory>
#include <stack>
#include <cmath>
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "assignment1_pumpjack_head.inl"
#include "assignment1_a_frame.inl"
#include "assignment1_counterweight.inl"

// Uniform aliases. Without these, the lines making/editing them get silly long.
template<typename T>
using UniformPtr = std::unique_ptr<guts::GLUniform<T>>;

template<typename T>
inline UniformPtr<T> GetNewUniform(GLuint program, const std::string &name) {
  return std::make_unique<guts::GLUniform<T>>(program, name);
}

// Useful constants
glm::vec3 AXIS_X = glm::vec3(1, 0, 0);
glm::vec3 AXIS_Y = glm::vec3(0, 1, 0);
glm::vec3 AXIS_Z = glm::vec3(0, 0, 1);
const float VIEW_INC = 0.05f;
glm::vec4 PART_COLOUR = glm::vec4(1, 1, 1, 1);

// based on https://stackoverflow.com/a/35651717
template<typename T>
T const PI = std::acos(-T(1));

// GL Globals
GLuint program, vao;
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
std::unique_ptr<guts::objs::ExtrudedObject> pumpjack_head, pumpjack_frame_part,
    pumpjack_counterweight_part;
std::unique_ptr<guts::objs::Tube> pumpjack_beam_part;
std::unique_ptr<guts::objs::Cube> pumpjack_base_part;
std::unique_ptr<guts::objs::Sphere> light_sphere;


// Initialiser function.
static void Init(__unused guts::GlfwWindow *window) {
  // Generate abd bind our VAO
  gl::GenVertexArrays(1, &vao);
  gl::BindVertexArray(vao);

  // Build the program
  guts::GLProgramBuilder builder;
  builder.AddShader(gl::VERTEX_SHADER, "assignment1.vert");
  builder.AddShader(gl::FRAGMENT_SHADER, "assignment1.frag");
  program = builder.BuildProgram();

  // Generate uniform objects
  model_uniform = GetNewUniform<glm::mat4>(program, "model");
  view_uniform = GetNewUniform<glm::mat4>(program, "view");
  projection_uniform = GetNewUniform<glm::mat4>(program, "projection");
  normal_matrix_uniform = GetNewUniform<glm::mat3>(program, "normalmatrix");
  colour_mode_uniform = GetNewUniform<GLuint>(program, "colourmode");
  emit_mode_uniform = GetNewUniform<GLuint>(program, "emitmode");
  light_mode_uniform = GetNewUniform<GLuint>(program, "lightmode");
  light_pos_uniform = GetNewUniform<glm::vec4>(program, "lightpos");

  // Generate scene objects
  pumpjack_head =
      std::make_unique<guts::objs::ExtrudedObject>(PUMPJACK_HEAD_TRIANGLES,
                                                   PUMPJACK_HEAD_EDGES,
                                                   PART_COLOUR);
  pumpjack_frame_part =
      std::make_unique<guts::objs::ExtrudedObject>(A_FRAME_TRIANGLES,
                                                   A_FRAME_EDGES,
                                                   PART_COLOUR);
  pumpjack_counterweight_part =
      std::make_unique<guts::objs::ExtrudedObject>(COUNTERWEIGHT_TRIANGLES,
                                                   COUNTERWEIGHT_EDGES,
                                                   PART_COLOUR);

  pumpjack_beam_part = std::make_unique<guts::objs::Tube>(500);
  pumpjack_base_part = std::make_unique<guts::objs::Cube>(false, &PART_COLOUR);
  light_sphere = std::make_unique<guts::objs::Sphere>(100, 100);
}

// Updates all uniforms and then renders a given GLObject.
static void UpdateAndRender(glm::mat4 &view, glm::mat4 &model,
                            guts::objs::GLObject &obj,
                            bool emitmode = false) {
  model_uniform->Set(model);
  glm::mat3 normal_matrix = guts::InverseTranspose(glm::mat3(view * model));
  normal_matrix_uniform->Set(normal_matrix);
  GLuint mode = 0;
  if (emitmode) mode = 1;
  emit_mode_uniform->Set(mode);
  obj.Render(render_mode);
  mode = 0;
}

// Limits a numeric type to between min and max, much like the GLSL clamp()
template <typename T>
static T Clamp(T val, T min, T max) {
  return std::max(min, std::min(max, val));
}

// Rendering handler for GlfwWindow.
static void Display(guts::GlfwWindow *window) {
  gl::ClearColor(0, 0, 0, 1);
  gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);
  gl::Enable(gl::DEPTH_TEST);
  gl::UseProgram(program);

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
  view_uniform->Set(view);
  projection_uniform->Set(projection);
  light_pos_uniform->Set(light_pos);
  colour_mode_uniform->Set(colourmode);
  light_mode_uniform->Set(lightmode);

  // Light source
  model.push(model.top());
  {
    model.top() = glm::translate(model.top(), light_pos3);
    model.top() = glm::scale(model.top(), glm::vec3(0.05f, 0.05f, 0.05f));
    model_uniform->Set(model.top());
    UpdateAndRender(view, model.top(), *light_sphere, true);
  }
  model.pop();

  // Pumpjack base
  model.push(model.top());
  {
    // Whole model transforms
    model.top() = glm::scale(model.top(), glm::vec3(2, 2, 2));
    //odel.top() = glm::translate(model.top(), glm::vec3(0, -8, 0));

    // Base
    model.push(model.top());
    {
      model.top() = glm::scale(model.top(), glm::vec3(10, 5, 0.2));
      //UpdateAndRender(view, model.top(), *pumpjack_base_part);
    }
    model.pop();
    // Base END

    model.top() = glm::translate(model.top(), glm::vec3(0, 0, -1));

    // Pumpjack beam
    model.push(model.top());
    {
      model.top() = glm::scale(model.top(), glm::vec3(0.01, 0.5, 0.01));
      model.top() = glm::translate(model.top(), glm::vec3(0, -0.55, 0));
      UpdateAndRender(view, model.top(), *pumpjack_beam_part);
    }
    model.pop();
    // Pumpjack beam END

    model.top() = glm::rotate(model.top(), PI<GLfloat>, AXIS_X);

    // A Frame Left
    model.push(model.top());
    {
      model.top() = glm::scale(model.top(), glm::vec3(1, 0.05, 1));
      model.top() = glm::translate(model.top(), glm::vec3(0, 4, 0));
      UpdateAndRender(view, model.top(), *pumpjack_frame_part);
    }
    model.pop();
    // A Frame Left END

    // A Frame Right
    model.push(model.top());
    {
      model.top() = glm::scale(model.top(), glm::vec3(1, 0.05, 1));
      model.top() = glm::translate(model.top(), glm::vec3(0, -4, 0));
      UpdateAndRender(view, model.top(), *pumpjack_frame_part);
    }
    model.pop();
    // A Frame Right END

    // Pumpjack head
    model.push(model.top());
    {
      model.top() = glm::rotate(model.top(), std::sin(time_pi) * 0.31f, AXIS_Y);

      // Pumpjack rear connector rod
      model.push(model.top());
      {
        model.top() = glm::translate(model.top(), glm::vec3(1, -0.23, 0));

        // Pitman arm left
        model.push(model.top());
        {
          // This bit is odd. I ran into some issues with the rod falling out
          // of sync with the counterweight rotation due to the head end moving
          // in and out. The clamped sine doesn't fully fix it, but I lack the
          // maths background to refine it more.
          model.top() = glm::rotate(model.top(),
                                    (std::sin(time_pi - PI<float> / 2.f) * 0.49f) -
                                        (Clamp(std::sin(time_pi), 0.f, 1.f)) * 0.1f,
                                    AXIS_Y);
          // This cancels out the parents rotation.
          model.top() = glm::rotate(model.top(),
                                    std::sin(time_pi) * -0.25f + PI<float>,
                                    AXIS_Y);
          model.top() = glm::rotate(model.top(), PI<float> / 2, AXIS_X);
          model.top() = glm::scale(model.top(), glm::vec3(0.01, 0.61, 0.01));
          UpdateAndRender(view, model.top(), *pumpjack_beam_part);
        }
        model.pop();
        // Pitman arm left END

        // Pitman arm right
        model.push(model.top());
        {
          // See the left Pitman arm for remarks.
          model.top() = glm::translate(model.top(), glm::vec3(0, 0.5, 0));
          model.top() = glm::rotate(model.top(),
                                    (std::sin(time_pi - PI<float> / 2.f) * 0.49f) -
                                        (Clamp(std::sin(time_pi), 0.f, 1.f)) * 0.1f,
                                    AXIS_Y);
          model.top() = glm::rotate(model.top(),
                                    std::sin(time_pi) * -0.25f + PI<float>,
                                    AXIS_Y);
          model.top() = glm::rotate(model.top(), PI<float> / 2, AXIS_X);
          model.top() = glm::scale(model.top(), glm::vec3(0.01, 0.61, 0.01));
          UpdateAndRender(view, model.top(), *pumpjack_beam_part);
        }
        model.pop();
        // Pitman arm right END

        model.top() = glm::scale(model.top(), glm::vec3(0.05, 0.5, 0.05));
        UpdateAndRender(view, model.top(), *pumpjack_beam_part);
      }
      model.pop();
      // Pumpjack rear connector rod END

      model.top() = glm::scale(model.top(), glm::vec3(2, 0.2, 2));
      model.top() = glm::translate(model.top(), glm::vec3(0, -0.4, 0));
      UpdateAndRender(view, model.top(), *pumpjack_head);
    }
    model.pop();
    // Pumpjack head END

    // Rear section (counterweights etc)
    model.push(model.top());
    {
      model.top() = glm::translate(model.top(), glm::vec3(1, 0.02, -0.70));
      model.top() = glm::scale(model.top(), glm::vec3(0.5, 0.5, 0.5));

      // Counterweight base placeholder
      model.push(model.top());
      {
        model.top() = glm::scale(model.top(), glm::vec3(1, 0.8, 2));
        UpdateAndRender(view, model.top(), *pumpjack_base_part);
      }
      model.pop();
      // Counterweight base placeholder END

      // Counterweight mid-rod
      model.push(model.top());
      {
        model.top() = glm::translate(model.top(), glm::vec3(0, -0.4, 0.25));
        model.top() = glm::scale(model.top(), glm::vec3(0.05, 0.8, 0.05));
        UpdateAndRender(view, model.top(), *pumpjack_beam_part);
      }
      model.pop();
      // Counterweight mid-rod END

      // Counterweight left
      model.push(model.top());
      {
        model.top() = glm::translate(model.top(), glm::vec3(0, 0.3, 0.25));
        model.top() = glm::scale(model.top(), glm::vec3(1, 0.1, 1));
        model.top() = glm::rotate(model.top(), time_pi + PI<float>,
                                  AXIS_Y);

        // Rod
        model.push(model.top());
        {
          model.top() = glm::translate(model.top(), glm::vec3(-0.6, 0, 0));
          model.top() = glm::scale(model.top(), glm::vec3(0.05, 3, 0.05));
          UpdateAndRender(view, model.top(), *pumpjack_beam_part);
        }
        model.pop();
        // Rod END

        UpdateAndRender(view, model.top(), *pumpjack_counterweight_part);
      }
      model.pop();
      // Counterweight left END

      // Counterweight right
      model.push(model.top());
      {
        model.top() = glm::rotate(model.top(), PI<float>, AXIS_X);
        model.top() = glm::translate(model.top(), glm::vec3(0, 0.3, -0.25));
        model.top() = glm::scale(model.top(), glm::vec3(1, 0.1, 1));
        model.top() = glm::rotate(model.top(), -(time_pi + PI<float>),
                                  AXIS_Y);

        // Rod
        model.push(model.top());
        {
          model.top() = glm::translate(model.top(), glm::vec3(-0.6, 0, 0));
          model.top() = glm::scale(model.top(), glm::vec3(0.05, 3, 0.05));
          UpdateAndRender(view, model.top(), *pumpjack_beam_part);
        }
        model.pop();
        // Rod END

        UpdateAndRender(view, model.top(), *pumpjack_counterweight_part);
      }
      model.pop();
      // Counterweight right END
    }
    model.pop();
    // Rear section END
  }
  model.pop();
  // Pumpjack base END

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
  guts::GlfwWindow *glw = new guts::GlfwWindow(1024, 768, "Assignment 1");

  glw->SetRenderer(Display);
  glw->SetKeyCallback(KeyCallback);
  glw->SetReshapeCallback(Reshape);

  Init(glw);

  glw->EventLoop();

  delete(glw);
  return EXIT_SUCCESS;
}