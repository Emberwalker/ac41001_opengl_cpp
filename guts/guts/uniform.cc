#include "uniform.h"
#include <glm/gtc/type_ptr.hpp>

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
void GLUniform<glm::vec3>::Set(glm::vec3 &val) {
  gl::Uniform3fv(this->uniform_id, 1, glm::value_ptr(val));
}

template<>
void GLUniform<glm::vec4>::Set(glm::vec4 &val) {
  gl::Uniform4fv(this->uniform_id, 1, glm::value_ptr(val));
}

template<>
void GLUniform<glm::mat3>::Set(glm::mat3 &val) {
  gl::UniformMatrix3fv(this->uniform_id, 1, gl::FALSE_, &val[0][0]);
}

template<>
void GLUniform<glm::mat4>::Set(glm::mat4 &val) {
  gl::UniformMatrix4fv(this->uniform_id, 1, gl::FALSE_, &val[0][0]);
}

template<typename T>
void DummyUniform<T>::Set(T &val) {
  return;
}

} // namespace guts