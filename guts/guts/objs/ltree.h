#ifndef GUTS_OBJS_LTREE_H
#define GUTS_OBJS_LTREE_H

#include "guts/objs/extrusion.h"
#include <string>
#include <memory>

namespace guts {
namespace objs {

class LTree: public GLObject {

 public:
  // Constructs a new L-System backed tree. Defaults to brown 'sticks'.
  // A custom component model can be provided via component, along with the
  // matrix needed to make it the correct scale (1 unit tall, <1 unit width) and
  // with the correct 'anchor' point (0, 0, 0 is used as the base of a
  // component)
  LTree(const std::string &production_pattern, unsigned int iterations = 8,
        unsigned int tube_resolution = 8,
        glm::vec4 colour = glm::vec4(0.368f, 0.153f, 0.007f, 1.f),
        std::unique_ptr<GLObject> component = nullptr,
        std::unique_ptr<glm::mat4> component_matrix = nullptr);


  // Overrides the layout for the shaders. DO NOT USE IF A CUSTOM COMPONENT IS
  // PASSED! You should handle that yourself.
  void OverrideAttributeLayout(GLuint attr_vertices,
                               GLuint attr_colours,
                               GLuint attr_normals);

  void Render(GLRenderMode mode) override;

 private:
  std::unique_ptr<GLObject> component;
  std::unique_ptr<glm::mat4> component_matrix;
  bool is_using_tube = false;

};

} // namespace objs
} // namespace guts

#endif //GUTS_OBJS_LTREE_H
