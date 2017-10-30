#ifndef GUTS_MATRIX_OPS_H
#define GUTS_MATRIX_OPS_H

#include <glm/glm.hpp>

namespace guts {

inline glm::mat3 InverseTranspose(glm::mat3 matrix) {
  return glm::transpose(glm::inverse(matrix));
}

} // namespace guts

#endif //GUTS_MATRIX_OPS_H
