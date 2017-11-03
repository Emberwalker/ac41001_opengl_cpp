#include "extrusion.h"

#include "guts/debug_tools.h"
#include "guts/internal/buffers.h"
#include "guts/internal/render.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>

namespace guts {
namespace objs {

namespace {

glm::vec3 GetNormalBetween(glm::vec3 anchor, glm::vec3 prev, glm::vec3 next) {
  auto to_prev = glm::normalize(prev - anchor);
  auto to_next = glm::normalize(anchor - next);
  // Ignore IncompatibleTypes because CLion is a derp.
/*
#pragma clang diagnostic push
#pragma ide diagnostic ignored "IncompatibleTypes"
  float dotted = glm::dot(to_prev, to_next);
#pragma clang diagnostic pop
  float angle = acos(dotted);
  glm::vec4 norm = glm::rotate(glm::mat4(), angle, glm::vec3(0, 1, 0))
      * glm::vec4(to_prev, 1);
  //norm = glm::normalize(norm);
  auto norm3 = glm::vec3(norm);
  norm3 /= norm.z;
  return norm3;*/
#pragma clang diagnostic push
#pragma ide diagnostic ignored "IncompatibleTypes"
  float angle = glm::angle(to_prev, to_next);
#pragma clang diagnostic pop
  glm::vec4 norm = glm::rotate(glm::mat4(), angle / 2.f, glm::vec3(0,1,0))
      * glm::vec4(to_prev, 1);
  norm = glm::normalize(norm);
  auto norm3 = glm::vec3(norm);
  norm3 *= (1.f/norm.z);
  return norm3;
}

inline void PushVec3OnVector(std::vector<GLfloat> &std_vec,
                             glm::vec3 &vec3) {
  std_vec.push_back(vec3.x);
  std_vec.push_back(vec3.y);
  std_vec.push_back(vec3.z);
}

} // namespace

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

  // Store face triangles and their normals
  vertices.insert(vertices.end(), face_triangles.begin(), face_triangles.end());
  vertices.insert(vertices.end(), top_face_triangles.begin(),
                  top_face_triangles.end());
  auto triangle_face_count = top_face_triangles.size();
  normals.reserve(triangle_face_count * 2);
  for (int i = 0; i < triangle_face_count; i++) {
    normals.push_back(0);
    normals.push_back(-1);
    normals.push_back(0);
  }
  for (int i = 0; i < triangle_face_count; i++) {
    normals.push_back(0);
    normals.push_back(1);
    normals.push_back(0);
  }

  // Generate edge vertices and their normals
  auto edge_vertex_count = face_edges.size() / 3;
  vertices.reserve(vertices.size() + edge_vertex_count * 3);
  normals.reserve(vertices.size() + edge_vertex_count * 3);
  for (int i = 0; i < edge_vertex_count - 1; i += 3) {
    glm::vec3 prev_edge_bottom;
    if ((i - 3) < 0) {
      auto base = edge_vertex_count * 3;
      prev_edge_bottom = glm::vec3(face_edges[base-3], 0,
                                   face_edges[base-1]);
    } else {
      prev_edge_bottom = glm::vec3(face_edges[i-3], 0, face_edges[i-1]);
    }
    auto curr_edge_bottom = glm::vec3(face_edges[i], 0, face_edges[i+2]);
    auto next_edge_bottom = glm::vec3(face_edges[i+3], 0, face_edges[i+5]);
    glm::vec3 next_next_edge_bottom;
    // Wrap around if necessary
    if (i + 6 >= edge_vertex_count) {
      auto remainder = (i + 6) % edge_vertex_count;
      next_next_edge_bottom = glm::vec3(face_edges[remainder], 0, face_edges[remainder + 2]);
    } else {
      next_next_edge_bottom = glm::vec3(face_edges[i+6], 0, face_edges[i+8]);
    }

    auto curr_edge_top = curr_edge_bottom;
    curr_edge_top.y = 1.f;
    auto next_edge_top = next_edge_bottom;
    next_edge_bottom.y = 1.f;

    glm::vec3 vecs[] = {curr_edge_top, curr_edge_bottom, next_edge_bottom,
                        next_edge_bottom, next_edge_top, curr_edge_top};
    for (auto vec : vecs) {
      PushVec3OnVector(vertices, vec);
    }

    // Normals (curr)
    glm::vec3 norm_currs = GetNormalBetween(curr_edge_bottom, prev_edge_bottom,
                                            next_edge_bottom);

    // Normals (next)
    glm::vec3 norm_nexts = GetNormalBetween(next_edge_bottom, curr_edge_bottom,
                                            next_next_edge_bottom);

    PushVec3OnVector(vertices, norm_currs);
    PushVec3OnVector(vertices, norm_currs);
    PushVec3OnVector(vertices, norm_nexts);
    PushVec3OnVector(vertices, norm_nexts);
    PushVec3OnVector(vertices, norm_nexts);
    PushVec3OnVector(vertices, norm_currs);
  }

  // Colours
  // TODO(arkan): Finish this bit.

  vbo = guts::internal::GenBuffer(vertices);
  cbo = guts::internal::GenBuffer(colours);
  nbo = guts::internal::GenBuffer(normals);
}

ExtrudedObject::~ExtrudedObject() {
  gl::DeleteBuffers(1, &this->vbo);
  gl::DeleteBuffers(1, &this->cbo);
  gl::DeleteBuffers(1, &this->nbo);
}

void ExtrudedObject::OverrideAttributeLayout(GLuint attr_vertices,
                                             GLuint attr_colours,
                                             GLuint attr_normals) {
  this->attr_vertices = attr_vertices;
  this->attr_colours = attr_colours;
  this->attr_normals = attr_normals;
}

void ExtrudedObject::Render(GLRenderMode mode) {
  // TODO(arkan): Obey should_use_colours
  guts::internal::RenderBasicObject(mode,
                                    static_cast<int>(this->vertices.size() / 3),
                                    this->vbo, this->attr_vertices,
                                    this->cbo, this->attr_colours,
                                    this->nbo, this->attr_normals);
}

} // namespace objs
} // namespace guts