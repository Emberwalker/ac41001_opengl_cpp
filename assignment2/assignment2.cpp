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
#include <guts/objs/terrain.h>
#include <guts/objs/particle_system.h>

#include <memory>
#include <stack>
#include <cmath>
#include <algorithm>
#include <random>
#include <array>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Uniform aliases. Without these, the lines making/editing them get silly long.
template<typename T>
using UniformPtr = std::shared_ptr<guts::GLUniform<T>>;

template<typename T>
inline UniformPtr<T> GetNewUniform(GLuint program, const std::string &name) {
  return std::make_shared<guts::GLUniform<T>>(program, name);
}

// ==== CONFIG ====
#define CONF_WORLD_TREE
//#define CONF_PARTICLE_SYSTEM
const unsigned int HOW_MANY_TREES = 10;
const float WORLD_TREE_SCALE = 7.f;
const unsigned int HOW_MANY_PARTICLES_PER_TREE = 25;

// Config END

// Useful constants
const float VIEW_INC = 0.1f;
const float LIGHT_INC = 1.f;
const float LIGHT_SCALE = 0.1f;
const float TERRAIN_SCALE = 0.1f;
const float TREE_SCALE = 1.f;
const unsigned int TERRAIN_DIM = 1024;
const float TERRAIN_OFFSET = -static_cast<float>(TERRAIN_DIM) / 2.f;
const int TERRAIN_OFFSET_INT = -TERRAIN_DIM / 2;
const std::array<std::string, 2> PRODUCTION_PATTERNS = {
    "F[+F][-F+F]F[+F]",
    "F[+F[-F]F]F[+F]",
};
const unsigned int MAX_LTREE_ITER = 3;
const unsigned int MIN_LTREE_ITER = 2;
const unsigned int TREE_TUBE_RES = 4;
const unsigned int WORLD_TREE_TUBE_RES = 8;
const unsigned int HOW_MANY_TREE_TEMPLATES = 5;
const unsigned int PROGRAM_COUNT = 5;

glm::vec3 AXIS_X = glm::vec3(1, 0, 0); // NOLINT
glm::vec3 AXIS_Y = glm::vec3(0, 1, 0); // NOLINT
glm::vec3 AXIS_Z = glm::vec3(0, 0, 1); // NOLINT
glm::vec4 FOG_COLOUR = glm::vec4(0.4f, 0.4f, 0.4f, 1.f); // NOLINT
glm::vec4 PARTICLE_COLOUR = glm::vec4(0.f, 0.8f, 0.1f, 1.f); // NOLINT

struct TreePosition {
  glm::vec3 pos;
  std::shared_ptr<guts::objs::LTree> tree;
};

// based on https://stackoverflow.com/a/35651717
template<typename T>
T const PI = std::acos(-T(1));

// GL Globals
GLuint program_std, program_particle, vao;
GLuint window_width = 640, window_height = 480;
GLuint colourmode = 1, lightmode = 0, fogmode = 1;
GLfloat aspect_ratio = 640.f / 480.f;
GLfloat view_x = 0.f, view_y = 0.f, view_z = 0.f;
GLfloat target_x = 0.f, target_y = 0.f, target_z = 0.f;
GLfloat light_x = 0, light_y = 20, light_z = 0;
guts::GLRenderMode render_mode = guts::RENDER_NORMAL;
float time_period = 5.0, camera_zoom = 10.f;
glm::vec3 world_tree_pos = glm::vec3(0.f); // NOLINT

// GL Uniforms
UniformPtr<glm::mat4> model_uniform[PROGRAM_COUNT], view_uniform[PROGRAM_COUNT],
    projection_uniform[PROGRAM_COUNT];
UniformPtr<glm::mat3> normal_matrix_uniform[PROGRAM_COUNT];
UniformPtr<GLuint> colour_mode_uniform[PROGRAM_COUNT],
    emit_mode_uniform[PROGRAM_COUNT], light_mode_uniform[PROGRAM_COUNT],
    fog_mode_uniform[PROGRAM_COUNT];
UniformPtr<glm::vec4> light_pos_uniform[PROGRAM_COUNT],
    fog_colour_uniform[PROGRAM_COUNT];
UniformPtr<glm::vec3> view_pos_uniform[PROGRAM_COUNT];

// Scene objects
std::unique_ptr<guts::objs::Terrain> terrain;
std::unique_ptr<guts::objs::Sphere> light_sphere;
std::unique_ptr<guts::objs::LTree> world_tree;
std::unique_ptr<guts::objs::ParticleSystem> particles;
std::vector<std::shared_ptr<guts::objs::LTree>> tree_templates;
std::vector<TreePosition> trees;


// Initialiser function.
static void Init(__unused guts::GlfwWindow *window) {
  // Generate abd bind our VAO
  gl::GenVertexArrays(1, &vao);
  gl::BindVertexArray(vao);

  // Build the programs
  guts::GLProgramBuilder builder_std;
  builder_std.AddShader(gl::VERTEX_SHADER, "standard.vert");
  builder_std.AddShader(gl::FRAGMENT_SHADER, "standard.frag");
  program_std = builder_std.BuildProgram();

  guts::GLProgramBuilder builder_particle;
  builder_particle.AddShader(gl::VERTEX_SHADER, "particle.vert");
  builder_particle.AddShader(gl::FRAGMENT_SHADER, "particle.frag");
  program_particle = builder_particle.BuildProgram();

  // Generate uniform objects
  for (int i = 0; i < PROGRAM_COUNT; i++) {
    model_uniform[i] = GetNewUniform<glm::mat4>(program_std, "model");
    view_uniform[i] = GetNewUniform<glm::mat4>(program_std, "view");
    projection_uniform[i] = GetNewUniform<glm::mat4>(program_std, "projection");
    normal_matrix_uniform[i] = GetNewUniform<glm::mat3>(program_std, "normalmatrix");
    colour_mode_uniform[i] = GetNewUniform<GLuint>(program_std, "colourmode");
    emit_mode_uniform[i] = GetNewUniform<GLuint>(program_std, "emitmode");
    light_mode_uniform[i] = GetNewUniform<GLuint>(program_std, "lightmode");
    fog_mode_uniform[i] = GetNewUniform<GLuint>(program_std, "fogmode");
    light_pos_uniform[i] = GetNewUniform<glm::vec4>(program_std, "lightpos");
    fog_colour_uniform[i] = GetNewUniform<glm::vec4>(program_std, "fogcolour");
    view_pos_uniform[i] = GetNewUniform<glm::vec3>(program_std, "viewpos");
  }

  // Generate scene objects
  terrain = std::make_unique<guts::objs::Terrain>(TERRAIN_DIM, TERRAIN_DIM);
  light_sphere = std::make_unique<guts::objs::Sphere>(100, 100);
#ifdef CONF_PARTICLE_SYSTEM
  particles = std::make_unique<guts::objs::ParticleSystem>(
      HOW_MANY_PARTICLES_PER_TREE, 2, time_period, PARTICLE_COLOUR);
#endif

  target_y = terrain->HeightAtPoint(
      static_cast<unsigned int>(-TERRAIN_OFFSET_INT),
      static_cast<unsigned int>(-TERRAIN_OFFSET_INT));
  light_y += target_y;

  std::mt19937 twister = guts::GetMT_RNG();
#ifdef CONF_WORLD_TREE
  std::uniform_int_distribution<int> pattern(0, static_cast<int>(
      PRODUCTION_PATTERNS.size() - 1));

  world_tree = std::make_unique<guts::objs::LTree>(
      PRODUCTION_PATTERNS[pattern(twister)], MAX_LTREE_ITER,
      WORLD_TREE_TUBE_RES);

  std::vector<glm::vec3> candidate_locs =
      terrain->GetPointsInZone(guts::objs::ZONE_GRASS);
  std::uniform_int_distribution<int> point_picker(0, static_cast<int>(
      candidate_locs.size()) - 1);
  world_tree_pos = candidate_locs[point_picker(twister)];
  world_tree_pos += glm::vec3(TERRAIN_OFFSET, 0.f, TERRAIN_OFFSET);
  world_tree_pos *= glm::vec3(TERRAIN_SCALE, 0.f, TERRAIN_SCALE);
#else
  // Build tree templates
  std::uniform_int_distribution<int> iterations(MIN_LTREE_ITER, MAX_LTREE_ITER);
  std::uniform_int_distribution<int> pattern(0, static_cast<int>(
      PRODUCTION_PATTERNS.size() - 1));
  for (int i = 0; i < HOW_MANY_TREE_TEMPLATES; i++) {
    std::string pattern_str = PRODUCTION_PATTERNS[pattern(twister)];
    int iters = iterations(twister);
    tree_templates.push_back(
        std::make_shared<guts::objs::LTree>(pattern_str, iters, TREE_TUBE_RES));
  }

  // Plant trees
  std::vector<glm::vec3> candidate_locs =
      terrain->GetPointsInZone(guts::objs::ZONE_GRASS);
  std::uniform_int_distribution<int> point_picker(0, static_cast<int>(
      candidate_locs.size()) - 1);
  std::uniform_int_distribution<int> tree_picker(0, static_cast<int>(
      tree_templates.size()) - 1);
  for (int i = 0; i < HOW_MANY_TREES; i++) {
    glm::vec3 position = candidate_locs[point_picker(twister)];
    position += glm::vec3(TERRAIN_OFFSET, 0.f, TERRAIN_OFFSET);
    position *= glm::vec3(TERRAIN_SCALE, 0.f, TERRAIN_SCALE);
    TreePosition pos = {
        position,
        tree_templates[tree_picker(twister)],
    };
    trees.push_back(pos);
  }
#endif
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
static void UpdateAndRenderMultipart(const UniformPtr<glm::mat3> &u_normal_matrix,
									 const UniformPtr<glm::mat4> &u_model,
									 const UniformPtr<GLuint> &u_emitmode,
									 glm::mat4 &view, glm::mat4 &model,
									 guts::objs::MultipartGLObject &obj,
									 bool emitmode = false) {
  GLuint mode = 0;
  if (emitmode) mode = 1;
  u_emitmode->Set(mode);
  obj.Render(render_mode, *u_model, model, *u_normal_matrix, view);
  mode = 0;
}

// Limits a numeric type to between min and max, much like the GLSL clamp()
template <typename T>
static T Clamp(T val, T min, T max) {
  return std::max(min, std::min(max, val));
}

template <typename T>
static void UpdateUniforms(UniformPtr<T> uniform[PROGRAM_COUNT], T &value) {
  for (int i = 0; i < PROGRAM_COUNT; i++) {
    uniform[i]->Set(value);
  }
}

// Rendering handler for GlfwWindow.
static void Display(guts::GlfwWindow *window) {
  gl::ClearColor(FOG_COLOUR.r, FOG_COLOUR.g, FOG_COLOUR.b, 1.f);
  gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);
  gl::Enable(gl::DEPTH_TEST);
  gl::UseProgram(program_std);

  // The GL transform stack.
  std::stack<glm::mat4> model;
  model.push(glm::mat4(1.0));

  glm::vec3 target = glm::vec3(target_x, target_y, target_z);
  glm::vec3 camera = glm::vec3(camera_zoom);
  camera += target;

  glm::mat4 projection = glm::perspective(glm::radians(40.f), aspect_ratio, 0.1f, 500.f);
  glm::mat4 view = glm::lookAt(
      camera,
      target,
      glm::vec3(0,1,0)
  );

  // Rotate the view with user controls on each axis.
  view = glm::rotate(view, view_x, AXIS_X);
  view = glm::rotate(view, view_y, AXIS_Y);
  view = glm::rotate(view, view_z, AXIS_Z);

  glm::vec4 light_pos = glm::vec4(light_x, light_y, light_z, 1.0);

  // Set constant uniforms
  UpdateUniforms(view_pos_uniform, camera);
  UpdateUniforms(fog_colour_uniform, FOG_COLOUR);
  UpdateUniforms(projection_uniform, projection);
  UpdateUniforms(light_pos_uniform, light_pos);
  UpdateUniforms(colour_mode_uniform, colourmode);
  UpdateUniforms(light_mode_uniform, lightmode);
  UpdateUniforms(fog_mode_uniform, fogmode);

  double time = window->GlfwTimer();// The GL transform stack.

  glm::vec3 light_pos3 = glm::vec3(light_pos) / light_pos.w;

  // Set constant uniforms
  UpdateUniforms(view_uniform, view);
  unsigned int program = 0;

  // Light source
  if (true) {
    model.push(model.top());
    {
      model.top() = scale(model.top(), glm::vec3(LIGHT_SCALE));
      model.top() = translate(model.top(), light_pos3);
      UpdateAndRender(normal_matrix_uniform[0],
                      model_uniform[0],
                      emit_mode_uniform[0], view,
                      model.top(), *light_sphere, true);
    }
    model.pop();
  }

  // Terrain
  model.push(model.top());
  {
    model.top() = translate(model.top(),
                                 glm::vec3(TERRAIN_OFFSET * TERRAIN_SCALE, 0.f,
                                           TERRAIN_OFFSET * TERRAIN_SCALE));
    model.top() = scale(model.top(), glm::vec3(TERRAIN_SCALE));
    UpdateAndRender(normal_matrix_uniform[0],
                    model_uniform[0],
                    emit_mode_uniform[0], view,
                    model.top(), *terrain, false);
  }
  model.pop();

#ifdef CONF_PARTICLE_SYSTEM
  particles->SetPeriod(time_period);
  particles->SetTime(time);
#endif

  // Trees
#ifdef CONF_WORLD_TREE
  model.push(model.top());
  {
    model.top() = glm::translate(model.top(), world_tree_pos);
    model.top() = glm::scale(model.top(), glm::vec3(TERRAIN_SCALE));
    model.top() = glm::scale(model.top(), glm::vec3(WORLD_TREE_SCALE));
    UpdateAndRenderMultipart(normal_matrix_uniform[0], model_uniform[0],
                             emit_mode_uniform[0], view,
                             model.top(), *world_tree, false);
#ifdef CONF_PARTICLE_SYSTEM
    gl::UseProgram(program_particle);
    UpdateAndRender(normal_matrix_uniform[1], model_uniform[1],
                    emit_mode_uniform[1], view,
                    model.top(), *particles, false);
#endif
  }
  model.pop();
#else
  for (TreePosition pos : trees) {
    model.push(model.top());
    {
      model.top() = translate(model.top(), pos.pos);
      model.top() = scale(model.top(), glm::vec3(TREE_SCALE));
      model.top() = scale(model.top(), glm::vec3(TERRAIN_SCALE));
      UpdateAndRenderMultipart(normal_matrix_uniform[0],
                               model_uniform[0],
                               emit_mode_uniform[0], view,
                               model.top(), *pos.tree, false);
#ifdef CONF_PARTICLE_SYSTEM
      gl::UseProgram(program_particle);
      UpdateAndRenderMultipart(normal_matrix_uniform[1],
                               model_uniform[1],
                               emit_mode_uniform[1], view,
                               model.top(), *particles, false);
#endif
    }
    model.pop();
  }
#endif

  if (model.size() != 1) {
    //guts_error("Imbalanced stack.");
  }

  gl::UseProgram(0);
}

// GlfwWindow reshape handler.
static void Reshape(__unused GLFWwindow *window, int w, int h) {
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

  if (key == GLFW_KEY_R) camera_zoom += LIGHT_INC;
  if (key == GLFW_KEY_F) camera_zoom -= LIGHT_INC;

  if (key == GLFW_KEY_Z) target_x += LIGHT_INC;
  if (key == GLFW_KEY_X) target_x -= LIGHT_INC;
  if (key == GLFW_KEY_C) target_y += LIGHT_INC;
  if (key == GLFW_KEY_V) target_y -= LIGHT_INC;
  if (key == GLFW_KEY_B) target_z += LIGHT_INC;
  if (key == GLFW_KEY_N) target_z -= LIGHT_INC;

  if (key == '1') light_x -= LIGHT_INC;
  if (key == '2') light_x += LIGHT_INC;
  if (key == '3') light_y -= LIGHT_INC;
  if (key == '4') light_y += LIGHT_INC;
  if (key == '5') light_z -= LIGHT_INC;
  if (key == '6') light_z += LIGHT_INC;

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

  if (key == '/' && action != GLFW_PRESS) {
    fogmode++;
    if (fogmode > 1) fogmode = 0;
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