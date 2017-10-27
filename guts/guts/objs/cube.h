#ifndef GUTS_OBJS_CUBE_H
#define GUTS_OBJS_CUBE_H

#include "guts/gl_object.h"
#include <array>

namespace guts {
namespace objs {

class Cube : public GLObject {

 public:
  explicit Cube(GLuint attr_vertices = 0, GLuint attr_colours = 1, GLuint attr_normals = 2);
  ~Cube();
  void Render(GLRenderMode mode) override;

 private:
  GLuint GenBuffer(GLfloat *contents, GLsizei size);

  GLuint vbo, cbo, nbo;
  GLuint attr_vertices, attr_colours, attr_normals;
  GLfloat vertices[108], colours[144], normals[108];

};

} // namespace objs
} // namespace guts

#endif // GUTS_OBJS_CUBE_H
