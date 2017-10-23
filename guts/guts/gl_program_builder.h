//
// Represents a (possible) OpenGL program. Handles shader compilation and
// other misc. jobs involved in creating and linking a program.
//

#ifndef GUTS_GL_PROGRAM_H
#define GUTS_GL_PROGRAM_H

#include <string>
#include <vector>

#include <glload/gl_4_1.hpp>

namespace guts {

// Represents a possible OpenGL program and assosciated shaders. Can be safely
// deleted after BuildProgram() to clean up the OpenGL shader sources.
class GLProgramBuilder {

 public:
  ~GLProgramBuilder();

  // Adds a shader to the final programs pipeline.
  void AddShader(GLenum shader_type, const std::string &shader_file_name);
  // Creates a program, links all the added shaders to it, then returns the
  // newly linked program.
  GLuint BuildProgram();

 private:
  std::vector<GLuint> shaders;
  bool already_built = false;

};

} // namespace guts

#endif //GUTS_GL_PROGRAM_H
