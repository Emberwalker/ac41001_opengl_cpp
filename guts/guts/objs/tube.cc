#include "tube.h"

#include "guts/debug_tools.h"
#include "guts/internal/buffers.h"
#include "guts/internal/render.h"
#include "guts/internal/containers.h"

#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace guts {
namespace objs {

namespace {

// based on https://stackoverflow.com/a/35651717
template<typename T>
T const PI = std::acos(-T(1));

} // namespace

Tube::Tube(unsigned int points, glm::vec4 colour) {
  guts_assert(points > 2, "point count must be > 2");

  float TWO_PI = 2.f * PI<float>;
  float angle_inc = TWO_PI / points;
  auto start = glm::vec4(1, 0, 0, 1);

  std::vector<glm::vec3> edge_vertices;
  edge_vertices.reserve(points);
  auto ident4 = glm::mat4();
  auto axis = glm::vec3(0, 1, 0);

  for (unsigned int i = 0; i < points; i++) {
    auto rot_matrix = glm::rotate(ident4, i * angle_inc, axis);
    auto point4 = start * rot_matrix;
    auto point = glm::vec3(point4) / point4.w;
    edge_vertices.push_back(point);
  }

  std::vector<glm::vec3> triangle_vertices;
  triangle_vertices.reserve(edge_vertices.size() * 3);
  auto origin = glm::vec3(0.f);

  for (unsigned int i = 0; i < points; i++) {
    auto curr = edge_vertices[i];
    auto next = guts::internal::GetFromVec(edge_vertices, i + 1);
    triangle_vertices.push_back(next);
    triangle_vertices.push_back(curr);
    triangle_vertices.push_back(origin);
  }

  auto edge_vertices_floats =
      guts::internal::ToComponents(edge_vertices);
  auto triangle_vertices_floats =
      guts::internal::ToComponents(triangle_vertices);

  extrusion = std::make_unique<ExtrudedObject>(triangle_vertices_floats,
                                               edge_vertices_floats, colour);
}

void Tube::OverrideAttributeLayout(GLuint attr_vertices,
                                   GLuint attr_colours,
                                   GLuint attr_normals) {
  extrusion->OverrideAttributeLayout(attr_vertices, attr_colours, attr_normals);
}

void Tube::Render(GLRenderMode mode) {
  extrusion->Render(mode);
}

} // namespace objs
} // namespace guts