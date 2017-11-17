#ifndef GUTS_GL_PARTICLE_H
#define GUTS_GL_PARTICLE_H

#include <memory>
#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace guts {

template <typename T>
struct GLParticlePoint {
  glm::vec3 location;
  glm::vec4 colour;
  std::unique_ptr<T> user_metadata;
};

// C++11 allows 'using' to effectively do template typedefs. Yay!

// Function called for each particle in the system to establish initial location
// and colour data, as well as initialise the user-defined metadata.
template <typename T>
using ParticleInitFn = GLParticlePoint<T> (*)(unsigned int index);

// Function called on every UpdateAndDraw() call. Metadata should always be
// moved with std::move!
template <typename T>
using ParticleUpdateFn = GLParticlePoint<T> (*)(const glm::vec3 &old_location,
                                                const glm::vec3 &old_colour,
                                                std::unique_ptr<T> metadata);

// Dummy object implementations which don't need user-defined metadata can
// return instead of a real meta object.
struct ParticleMetaDummy {};

template <typename T>
class GLParticle {

 public:
  GLParticle(unsigned int particle_count, ParticleInitFn<T> initFn,
             ParticleUpdateFn<T> updateFn, unsigned int attr_vertices = 0,
             unsigned int attr_colours = 1);
  void SetUpdateFn(ParticleUpdateFn<T> newFn);
  void UpdateAndDraw();

 protected:
  unsigned int particle_count;
  std::vector<float> points, colours;
  std::vector<std::unique_ptr<T>> metadata_ptrs;
  unsigned int vbo, cbo;
  unsigned int attr_vertices, attr_colours;
  ParticleUpdateFn<T> updateFn;

};

} // namespace guts

#endif //GUTS_GL_PARTICLE_H
