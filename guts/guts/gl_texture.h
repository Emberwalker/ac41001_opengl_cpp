#ifndef GUTS_GL_TEXTURE_H
#define GUTS_GL_TEXTURE_H

#include "guts/tex/texture_loader.h"
#include "guts/debug_tools.h"

#include <string>
#include <vector>
#include <glload/gl_4_1.hpp>

namespace guts {

class GLTexture {

 public:
  explicit GLTexture(GLuint program, const std::string &uniform_name,
                     const std::string &file_name, bool is_heightmap = false);

  // Binds this texture in OpenGL.
  void BindTexture();

  // Sets sane defaults for OpenGL's texture handling.
  static void SetDefaultParameters();

 protected:
  std::vector<GLubyte> texture_data;
  __unused GLint uniform_id;
  GLuint texture_id;

};

} // namespace guts

#endif //GUTS_GL_TEXTURE_H
