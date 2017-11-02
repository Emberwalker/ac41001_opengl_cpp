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
  void Render(GLRenderMode mode) override;

 protected:
  std::vector<GLfloat> vertices, normals;

};

} // namespace objs
} // namespace guts

#endif //GUTS_OBJS_EXTRUSION_H
