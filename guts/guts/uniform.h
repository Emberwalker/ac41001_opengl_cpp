#ifndef GUTS_UNIFORM_H
#define GUTS_UNIFORM_H

#include "guts/debug_tools.h"
#include <string>
#include <iostream>
#include <glload/gl_4_1.hpp>
#include <glm/glm.hpp>

namespace guts {

template<class T> class GLUniform {

 public:
  GLUniform(GLuint program, const std::string &uniform_name) {
    this->uniform_id = gl::GetUniformLocation(program, uniform_name.c_str());
  }

  template void Set<GLuint>(T val) {
    gl::Uniform1ui(this->uniform_id, val);
  }

 protected:
  GLint uniform_id;

};

template<template<typename V> class T> class GLUniform<T> {

 public:
  GLUniform(GLuint program, const std::string &uniform_name);

  template void Set<glm::vec3, GLfloat>(T<V> val) {
    gl::Uniform3fv(this->uniform_id, 1, val[0]);
  }

  template void Set<glm::vec4, GLfloat>(T<V> val) {
    gl::Uniform4fv(this->uniform_id, 1, val[0]);
  }

 private:
  GLint uniform_id;

};

}

#endif //GUTS_UNIFORM_H
