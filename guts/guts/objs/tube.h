#ifndef GUTS_OBJS_TUBE_H
#define GUTS_OBJS_TUBE_H

#include "guts/objs/extrusion.h"
#include <memory>
#include <glload/gl_4_1.hpp>

namespace guts {
namespace objs {

class Tube : public GLObject {

 public:
  Tube(unsigned int points, glm::vec4 colour = glm::vec4(1,1,1,1));
  void OverrideAttributeLayout(GLuint attr_vertices,
                               GLuint attr_colours,
                               GLuint attr_normals);
  void Render(GLRenderMode mode) override;

 private:
  std::unique_ptr<ExtrudedObject> extrusion;

};

} // namespace objs
} // namespace guts

#endif //GUTS_OBJS_TUBE_H
