#include "uniform.h"

namespace guts {

template<>
void GLUniform<GLuint>::Set(GLuint &val) {
  gl::Uniform1ui(this->uniform_id, val);
}

template<>
void GLUniform<GLint>::Set(GLint &val) {
  gl::Uniform1i(this->uniform_id, val);
}

template<>
void GLUniform<GLfloat>::Set(GLfloat &val) {
  gl::Uniform1f(this->uniform_id, val);
}

template<>
void GLUniform<glm::tvec3<GLfloat>>::Set(glm::tvec3<GLfloat> &val) {
  gl::Uniform3fv(this->uniform_id, 1, &val[0]);
}

template<>
void GLUniform<glm::tvec4<GLfloat>>::Set(glm::tvec4<GLfloat> &val) {
  gl::Uniform4fv(this->uniform_id, 1, &val[0]);
}

template<>
void GLUniform<glm::mat3>::Set(glm::mat3 &val) {
  gl::UniformMatrix3fv(this->uniform_id, 1, gl::FALSE_, &val[0][0]);
}

template<>
void GLUniform<glm::mat4>::Set(glm::mat4 &val) {
  gl::UniformMatrix4fv(this->uniform_id, 1, gl::FALSE_, &val[0][0]);
}

} // namespace guts