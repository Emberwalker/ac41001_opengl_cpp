#ifndef GUTS_INTERNAL_BUFFERS_H
#define GUTS_INTERNAL_BUFFERS_H

#include <glload/gl_4_1.hpp>

namespace guts {
namespace internal {

inline GLuint GenBuffer(GLfloat *contents, GLsizei size) {
  GLuint buffer;
  gl::GenBuffers(1, &buffer);
  gl::BindBuffer(gl::ARRAY_BUFFER, buffer);
  gl::BufferData(gl::ARRAY_BUFFER, size * sizeof(GLfloat), contents,
                 gl::STATIC_DRAW);
  gl::BindBuffer(gl::ARRAY_BUFFER, 0);
  return buffer;
}

inline GLuint GenElementBuffer(GLuint *contents, GLsizei size) {
  GLuint buffer;
  gl::GenBuffers(1, &buffer);
  gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, buffer);
  gl::BufferData(gl::ELEMENT_ARRAY_BUFFER, size * sizeof(GLuint), contents,
                 gl::STATIC_DRAW);
  gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, 0);
  return buffer;
}

} // namespace internal
} // namespace guts

#endif //GUTS_INTERNAL_BUFFERS_H
