#ifndef GUTS_OBJS_SPHERE_H
#define GUTS_OBJS_SPHERE_H

#include "guts/gl_object.h"
#include <vector>
#include <glload/gl_4_1.hpp>

namespace guts {
namespace objs {

// An OpenGL sphere. Essentially identical to Iain Martin's sphere class, but
// adjusted to fit into the GUTS hierarchy.
class Sphere : public GLObject {

 public:
  explicit Sphere(unsigned int lats, unsigned int longs,
                  GLuint attr_vertices = 0, GLuint attr_colours = 1,
                  GLuint attr_normals = 2);
  ~Sphere();
  void Render(GLRenderMode mode) override;

 protected:
  unsigned int lats, longs, vertices_count;
  GLuint vbo, cbo, nbo, element_array;
  GLuint attr_vertices, attr_colours, attr_normals;
  std::vector<GLfloat> vertices, colours;
  std::vector<GLuint> indices;

};

} // namespace objs
} // namespace guts

#endif //GUTS_OBJS_SPHERE_H
