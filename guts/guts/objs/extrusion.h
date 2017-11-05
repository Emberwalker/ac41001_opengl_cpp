#ifndef GUTS_OBJS_EXTRUSION_H
#define GUTS_OBJS_EXTRUSION_H

#include "guts/gl_object.h"
#include <vector>
#include <glload/gl_4_1.hpp>
#include <glm/glm.hpp>

namespace guts {
namespace objs {

// The GUTS secret sauce. Takes a vector of edges, and another of face triangles
// (both on the y = 0 plane) and extrudes it into a shape with y length of 1.
class ExtrudedObject : public GLObject {

 public:
  explicit ExtrudedObject(std::vector<GLfloat> &face_triangles,
                          std::vector<GLfloat> &face_edges,
                          glm::vec4 colour = glm::vec4(0.75, 0.75, 0.75, 1.0));
  ~ExtrudedObject();
  // Allows overriding the default attribute layout for passing into shaders.
  // Defaults to vertices = 0, colours = 1, normals = 2.
  void OverrideAttributeLayout(GLuint attr_vertices, GLuint attr_colours,
                               GLuint attr_normals);
  void Render(GLRenderMode mode) override;

 protected:
  std::vector<GLfloat> vertices, normals, colours;
  GLuint vbo, cbo, nbo;
  GLuint attr_vertices = 0, attr_colours = 1, attr_normals = 2;

};

} // namespace objs
} // namespace guts

#endif //GUTS_OBJS_EXTRUSION_H
