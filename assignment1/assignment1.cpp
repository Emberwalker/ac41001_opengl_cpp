#include <guts/guts.h>

#include <glm/glm.hpp>

// GL Globals
GLuint program, vao;

// GL Uniforms
GLint model_id, view_id, projection_id, light_pos_id, normal_matrix_id,
    colour_mode_id, emit_mode_id;

void Init(guts::GlfwWindow *window) {
  // TODO(arkan): Implement.

  gl::GenVertexArrays(1, &vao);
  gl::BindVertexArray(vao);

  guts::GLProgramBuilder builder;
  builder.AddShader(gl::VERTEX_SHADER, "assignment1.vert");
  builder.AddShader(gl::FRAGMENT_SHADER, "assignment1.frag");
  program = builder.BuildProgram();
}

void Display(guts::GlfwWindow *window) {
  // TODO(arkan): Implement.
}
