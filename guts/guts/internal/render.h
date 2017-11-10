#ifndef GUTS_INTERNAL_RENDER_H
#define GUTS_INTERNAL_RENDER_H

#include "guts/gl_object.h"
#include "guts/debug_tools.h"

namespace guts {
namespace internal {

namespace {

// Internal helper to bind an array buffer.
inline void BindArrayBuffer(GLuint id, GLuint attr, GLuint len) {
  gl::BindBuffer(gl::ARRAY_BUFFER, id);
  gl::EnableVertexAttribArray(attr);
  gl::VertexAttribPointer(attr, len, gl::FLOAT, gl::FALSE_, 0, nullptr);
}

} // namespace

// Renders a basic object consisting of only triangles with no texture edges.
inline void RenderBasicObject(GLRenderMode mode, int render_length,
                              GLuint vbo, GLuint attr_vertices, GLuint v_size,
                              GLuint cbo, GLuint attr_colours, GLuint c_size,
                              GLuint nbo, GLuint attr_normals, GLuint n_size) {
  PrintOpenGLErrors();
  BindArrayBuffer(vbo, attr_vertices, v_size);
  BindArrayBuffer(cbo, attr_colours, c_size);
  BindArrayBuffer(nbo, attr_normals, n_size);

  gl::PointSize(3.f);

  if (mode == RENDER_WIREFRAME) {
    gl::PolygonMode(gl::FRONT_AND_BACK, gl::LINE);
  } else {
    gl::PolygonMode(gl::FRONT_AND_BACK, gl::FILL);
  }

  PrintOpenGLErrors();

  if (mode == RENDER_POINTS) {
    gl::DrawArrays(gl::POINTS, 0, render_length);
  } else {
    gl::DrawArrays(gl::TRIANGLES, 0, render_length);
  }
  PrintOpenGLErrors();
}

inline void RenderBasicObject(GLRenderMode mode, int render_length,
                              GLuint vbo, GLuint attr_vertices,
                              GLuint cbo, GLuint attr_colours,
                              GLuint nbo, GLuint attr_normals) {
  RenderBasicObject(mode, render_length,
                    vbo, attr_vertices, 3,
                    cbo, attr_colours, 4,
                    nbo, attr_normals, 3);
}

} // namespace internal
} // namespace guts

#endif //GUTS_INTERNAL_RENDER_H
