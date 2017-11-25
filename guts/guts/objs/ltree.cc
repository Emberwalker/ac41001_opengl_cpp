#include "ltree.h"
#include "tube.h"
#include "guts/debug_tools.h"

#include <glm/gtc/matrix_transform.hpp>

namespace guts {
namespace objs {

namespace {

struct TreeSegment {
  glm::vec3 start;
  glm::vec3 end;
};

glm::mat4 TMPL_TUBE_MATRIX = glm::scale(glm::vec3(0.05, 1.0, 0.05),
                                        glm::mat4(1.f));

} // namespace

LTree::LTree(const std::string &production_pattern,
             unsigned int iterations,
             unsigned int tube_resolution,
             glm::vec4 colour,
             std::unique_ptr<GLObject> component,
             std::unique_ptr<glm::mat4> component_matrix) {

  if (component) {
    this->component = std::move(component);
    guts_assert(component_matrix != nullptr,
                "if component is defined, so too must component_matrix");
    this->component_matrix = std::move(component_matrix);
  } else {
    this->component = std::make_unique<Tube>(tube_resolution, colour);
    this->component_matrix = std::make_unique<glm::mat4>(TMPL_TUBE_MATRIX);
    this->is_using_tube = true;
  }
}

void LTree::OverrideAttributeLayout(GLuint attr_vertices,
                                    GLuint attr_colours,
                                    GLuint attr_normals) {
  if (is_using_tube) {
    // If we've set is_using_tube, we know it's a tube.
    auto tube = dynamic_cast<Tube*>(this->component.operator*());
    guts_assert(tube != nullptr, "dynamic cast to tube must succeed.");
    tube->OverrideAttributeLayout(attr_vertices, attr_colours, attr_normals);
  } else {
    guts_error("Attempt to override attrs when a custom object is in use!");
  }
}

void LTree::Render(GLRenderMode mode) {

}

} // namespace objs
} // namespace guts