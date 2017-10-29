#ifndef GUTS_INTERNAL_RENDER_H
#define GUTS_INTERNAL_RENDER_H

#include "guts/gl_object.h"

namespace guts {
namespace internal {

inline void RenderBasicObject(GLRenderMode mode, int render_length,
                              GLuint vbo, GLuint attr_vertices,
                              GLuint cbo, GLuint attr_colours,
                              GLuint nbo, GLuint attr_normals) {
  gl::BindBuffer(gl::ARRAY_BUFFER, vbo);
  gl::EnableVertexAttribArray(attr_vertices);
  gl::VertexAttribPointer(attr_vertices, 3, gl::FLOAT, gl::FALSE_, 0, nullptr);

  gl::BindBuffer(gl::ARRAY_BUFFER, cbo);
  gl::EnableVertexAttribArray(attr_colours);
  gl::VertexAttribPointer(attr_colours, 4, gl::FLOAT, gl::FALSE_, 0, nullptr);

  gl::BindBuffer(gl::ARRAY_BUFFER, nbo);
  gl::EnableVertexAttribArray(attr_normals);
  gl::VertexAttribPointer(attr_normals, 3, gl::FLOAT, gl::FALSE_, 0, nullptr);

  gl::PointSize(3.f);

  if (mode == RENDER_WIREFRAME) {
    gl::PolygonMode(gl::FRONT_AND_BACK, gl::LINE);
  } else {
    gl::PolygonMode(gl::FRONT_AND_BACK, gl::FILL);
  }

  if (mode == RENDER_POINTS) {
    gl::DrawArrays(gl::POINTS, 0, render_length);
  } else {
    gl::DrawArrays(gl::TRIANGLES, 0, render_length);
  }
}

} // namespace internal
} // namespace guts

#endif //GUTS_INTERNAL_RENDER_H
