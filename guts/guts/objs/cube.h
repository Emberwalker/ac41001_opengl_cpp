#ifndef GUTS_OBJS_CUBE_H
#define GUTS_OBJS_CUBE_H

#include "guts/gl_object.h"
#include <vector>
#include <glm/glm.hpp>
#include <glload/gl_4_1.hpp>

namespace guts {
namespace objs {

class Cube : public GLObject {

 public:
  explicit Cube(bool textured = false, glm::vec4 *override_colour = nullptr,
                GLuint attr_vertices = 0, GLuint attr_colours_or_tex = 1,
                GLuint attr_normals = 2);
  ~Cube();
  void Render(GLRenderMode mode) override;

 private:
  GLuint vbo, c_t_bo, nbo;
  GLuint attr_vertices, attr_colours_or_tex, attr_normals;
  std::vector<GLfloat> vertices, colours_or_tex, normals;

};

} // namespace objs
} // namespace guts

#endif // GUTS_OBJS_CUBE_H
