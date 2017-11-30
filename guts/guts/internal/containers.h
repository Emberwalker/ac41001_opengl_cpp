#ifndef GUTS_INTERNAL_CONTAINERS_H
#define GUTS_INTERNAL_CONTAINERS_H

#include <vector>
#include <cctype>
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

inline std::string StringToLower(const std::string &str) {
  std::string out = "";
  out.reserve(str.length());
  auto start = str.begin();
  auto end = str.end();
  for (auto iter = start; iter != end; iter++) {
    out.push_back(static_cast<char>(std::tolower(*iter)));
  }
  return out;
}

inline std::string StringToUpper(const std::string &str) {
  std::string out = "";
  out.reserve(str.length());
  auto start = str.begin();
  auto end = str.end();
  for (auto iter = start; iter != end; iter++) {
    out.push_back(static_cast<char>(std::toupper(*iter)));
  }
  return out;
}

inline unsigned int StringCountCharacter(const char ch,
                                         const std::string &str) {
  unsigned int count = 0;
  auto start = str.begin();
  auto end = str.end();
  for (auto iter = start; iter != end; iter++) {
    if (ch == *iter) count += 1;
  }
  return count;
}

} // namespace internal
} // namespace guts

#endif //GUTS_INTERNAL_CONTAINERS_H
