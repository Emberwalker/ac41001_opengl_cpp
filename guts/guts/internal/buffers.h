#ifndef GUTS_INTERNAL_BUFFERS_H
#define GUTS_INTERNAL_BUFFERS_H

#include <glload/gl_4_1.hpp>
#include <vector>

namespace guts {
namespace internal {

//inline GLuint GenBuffer(GLfloat *contents, GLsizei size) {
inline GLuint GenBuffer(const std::vector<GLfloat> &contents) {
  GLuint buffer;
  auto size = static_cast<GLsizei>(contents.size());
  gl::GenBuffers(1, &buffer);
  gl::BindBuffer(gl::ARRAY_BUFFER, buffer);
  gl::BufferData(gl::ARRAY_BUFFER, size * sizeof(GLfloat), contents.data(),
                 gl::STATIC_DRAW);
  gl::BindBuffer(gl::ARRAY_BUFFER, 0);
  return buffer;
}

inline GLuint GenElementBuffer(const std::vector<GLuint> &contents) {
  GLuint buffer;
  auto size = static_cast<GLsizei>(contents.size());
  gl::GenBuffers(1, &buffer);
  gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, buffer);
  gl::BufferData(gl::ELEMENT_ARRAY_BUFFER, size * sizeof(GLuint),
                 contents.data(), gl::STATIC_DRAW);
  gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, 0);
  return buffer;
}

} // namespace internal
} // namespace guts

#endif //GUTS_INTERNAL_BUFFERS_H
