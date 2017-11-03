#ifndef GUTS_OBJS_EXTRUSION_H
#define GUTS_OBJS_EXTRUSION_H

#include "guts/gl_object.h"
#include <vector>
#include <glload/gl_4_1.hpp>
#include <glm/glm.hpp>

namespace guts {
namespace objs {

class ExtrudedObject : public GLObject {

 public:
  explicit ExtrudedObject(std::vector<GLfloat> face_triangles,
                          std::vector<GLfloat> face_edges,
                          glm::vec4 colour = glm::vec4(0.75, 0.75, 0.75, 1.0));
  ~ExtrudedObject();
  void OverrideAttributeLayout(GLuint attr_vertices, GLuint attr_colours,
                               GLuint attr_normals);
  void Render(GLRenderMode mode) override;

 protected:
  // Subclasses can toggle this to false for e.g. textured drawing.
  bool should_use_colours = true;
  std::vector<GLfloat> vertices, normals, colours;
  GLuint vbo, cbo, nbo;
  GLuint attr_vertices = 0, attr_colours = 1, attr_normals = 2;

};

} // namespace objs
} // namespace guts

#endif //GUTS_OBJS_EXTRUSION_H
