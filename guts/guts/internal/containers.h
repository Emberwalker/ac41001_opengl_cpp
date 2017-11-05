#ifndef GUTS_INTERNAL_CONTAINERS_H
#define GUTS_INTERNAL_CONTAINERS_H

#include <vector>
#include <glm/glm.hpp>

namespace guts {
namespace internal {

// Gets a value from a vector, while handing negative indices properly and
// looping after the end of the vector. Great for grabbing the next vec3 in a
// GLfloat vector.
template<typename T>
T GetFromVec(std::vector<T> &vec, int i) {
  if (i < 0) {
    return vec[vec.size() + i];
  }
  return vec[i % vec.size()];
}

// Breaks down a vector of GLM vec2s into a GLfloat raw vector.
inline std::vector<float> ToComponents(std::vector<glm::vec2> &src) {
  std::vector<float> out;
  out.reserve(src.size() * 2);
  for (auto &vec : src) {
    out.push_back(vec.x);
    out.push_back(vec.y);
  }
  return out;
};

// Breaks down a vector of GLM vec3s into a GLfloat raw vector.
inline std::vector<float> ToComponents(std::vector<glm::vec3> &src) {
  std::vector<float> out;
  out.reserve(src.size() * 3);
  for (auto &vec : src) {
    out.push_back(vec.x);
    out.push_back(vec.y);
    out.push_back(vec.z);
  }
  return out;
};

// Breaks down a vector of GLM vec4s into a GLfloat raw vector.
inline std::vector<float> ToComponents(std::vector<glm::vec4> &src) {
  std::vector<float> out;
  out.reserve(src.size() * 4);
  for (auto &vec : src) {
    out.push_back(vec.x);
    out.push_back(vec.y);
    out.push_back(vec.z);
    out.push_back(vec.w);
  }
  return out;
};

} // namespace internal
} // namespace guts

#endif //GUTS_INTERNAL_CONTAINERS_H
