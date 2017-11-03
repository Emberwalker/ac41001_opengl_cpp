#ifndef GUTS_INTERNAL_RENDER_H
#define GUTS_INTERNAL_RENDER_H

#include "guts/gl_object.h"
#include "guts/debug_tools.h"

namespace guts {
namespace internal {

namespace {

inline void BindArrayBuffer(GLuint id, GLuint attr, GLuint len) {
  gl::BindBuffer(gl::ARRAY_BUFFER, id);
  gl::EnableVertexAttribArray(attr);
  gl::VertexAttribPointer(attr, len, gl::FLOAT, gl::FALSE_, 0, nullptr);
}

} // namespace

inline void RenderBasicObject(GLRenderMode mode, int render_length,
                              GLuint vbo, GLuint attr_vertices,
                              GLuint cbo, GLuint attr_colours,
                              GLuint nbo, GLuint attr_normals) {
  PrintOpenGLErrors();
  BindArrayBuffer(vbo, attr_vertices, 3);
  BindArrayBuffer(cbo, attr_colours, 4);
  BindArrayBuffer(nbo, attr_normals, 3);

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
                              GLuint nbo, GLuint attr_normals,
                              GLuint tbo, GLuint attr_texs) {
  BindArrayBuffer(tbo, attr_texs, 2);
  RenderBasicObject(mode, render_length, vbo, attr_vertices, cbo, attr_colours,
                    nbo, attr_normals);
}

} // namespace internal
} // namespace guts

#endif //GUTS_INTERNAL_RENDER_H
