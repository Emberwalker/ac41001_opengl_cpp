#ifndef GUTS_UNIFORM_H
#define GUTS_UNIFORM_H

#include "guts/debug_tools.h"
#include <string>
#include <iostream>
#include <glload/gl_4_1.hpp>
#include <glm/glm.hpp>

namespace guts {

template<typename T>
class GLUniform {

 public:
  GLUniform(GLuint program, const std::string &uniform_name) {
    this->uniform_id = gl::GetUniformLocation(program, uniform_name.c_str());
  }

#pragma clang diagnostic push
#pragma clang diagnostic ignored "NotImplementedFunctions"
  void Set(T &val);
#pragma clang diagnostic pop

 private:
  GLint uniform_id;

};

}

#endif //GUTS_UNIFORM_H
