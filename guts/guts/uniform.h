#ifndef GUTS_UNIFORM_H
#define GUTS_UNIFORM_H

#include "guts/debug_tools.h"
#include <string>
#include <iostream>
#include <glload/gl_4_1.hpp>
#include <glm/glm.hpp>

namespace guts {

// Represents an OpenGL uniform. This saves storing raw IDs in programs, and
// allows a more idiomatic API for C++.
template<typename T>
class GLUniform {

 public:
  GLUniform(GLuint program, const std::string &uniform_name) {
    this->uniform_id = gl::GetUniformLocation(program, uniform_name.c_str());
  }

#pragma clang diagnostic push
#pragma clang diagnostic ignored "NotImplementedFunctions"
  virtual // Sets the value of this uniform. This is only defined on supported types;
  // see uniform.cc for a full list.
  void Set(T &val);
#pragma clang diagnostic pop

 protected:
  // Blank ctor. Used for subclasses to do custom resolution (or dummying)
  GLUniform() {
    this->uniform_id = -1;
  }

  GLint uniform_id;

};

// A fake uniform for if a certain uniform isn't in use e.g. for passing to draw
// calls.
template<typename T>
class DummyUniform : GLUniform {
  DummyUniform(): GLUniform() {};
  void Set(T &val) override;
};

}

#endif //GUTS_UNIFORM_H
