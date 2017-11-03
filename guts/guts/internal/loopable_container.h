#ifndef GUTS_INTERNAL_LOOPABLE_CONTAINER_H
#define GUTS_INTERNAL_LOOPABLE_CONTAINER_H

#include <vector>

namespace guts {
namespace internal {

template<typename T>
T GetFromVec(std::vector<T> &vec, int i) {
  if (i < 0) {
    return vec[vec.size() + i];
  }
  return vec[i % vec.size()];
}

} // namespace internal
} // namespace guts

#endif //GUTS_INTERNAL_LOOPABLE_CONTAINER_H
