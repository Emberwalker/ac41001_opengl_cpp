#include "extrusion.h"

#include "guts/debug_tools.h"

namespace guts {
namespace objs {

ExtrudedObject::ExtrudedObject(std::vector<GLfloat> face_triangles,
                               std::vector<GLfloat> face_edges,
                               glm::vec4 colour) {
  guts_assert(face_triangles.size() % 3 == 0, "face_triangles must be "
      "divisible by 3 for vertices.");
  guts_assert(face_edges.size() % 3 == 0, "face_edges must be divisible by 3 "
      "for vertices.");

  std::vector<GLfloat> top_face_triangles;
  top_face_triangles.reserve(face_triangles.size());
  for (int i = 0; i < face_triangles.size(); i += 3) {
    top_face_triangles.push_back(face_triangles[i]);
    top_face_triangles.push_back(1.f);
    top_face_triangles.push_back(face_triangles[i+2]);
  }

  std::vector<GLfloat> top_face_edges;
  top_face_edges.reserve(face_edges.size());
  for (int i = 0; i < face_edges.size(); i+=3) {
    top_face_edges.push_back(face_edges[i]);
    top_face_edges.push_back(1.f);
    top_face_edges.push_back(face_edges[i+2]);
  }

  // Store face triangles
  vertices.insert(vertices.end(), face_triangles.begin(), face_triangles.end());
  vertices.insert(vertices.end(), top_face_triangles.begin(),
                  top_face_triangles.end());
}

ExtrudedObject::~ExtrudedObject() {
  // TODO(arkan)
}

void ExtrudedObject::Render(GLRenderMode mode) {
  // TODO(arkan)
}

} // namespace objs
} // namespace guts