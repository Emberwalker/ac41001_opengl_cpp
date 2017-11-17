#include "gl_particle.h"

#include "internal/buffers.h"

namespace guts {

template<typename T>
GLParticle<T>::GLParticle(unsigned int particle_count,
                          ParticleInitFn<T> initFn,
                          ParticleUpdateFn<T> updateFn,
                          unsigned int attr_vertices,
                          unsigned int attr_colours) {
  this->particle_count = particle_count;
  this->updateFn = updateFn;
  this->attr_vertices = attr_vertices;
  this->attr_colours = attr_colours;

  points.reserve(particle_count * 3);
  colours.reserve(particle_count * 4);
  metadata_ptrs.reserve(particle_count);

  for (unsigned int i = 0; i < particle_count; i++) {
    GLParticlePoint<T> pt = initFn(i);

    points.push_back(pt.location.x);
    points.push_back(pt.location.y);
    points.push_back(pt.location.z);

    colours.push_back(pt.colour.r);
    colours.push_back(pt.colour.g);
    colours.push_back(pt.colour.b);
    colours.push_back(pt.colour.a);

    metadata_ptrs.push_back(std::move(pt.user_metadata));
  }

  vbo = internal::GenBuffer(points);
  cbo = internal::GenBuffer(colours);
}

template<typename T>
void GLParticle<T>::SetUpdateFn(ParticleUpdateFn<T> newFn) {
  this->updateFn = newFn;
}

template<typename T>
void GLParticle<T>::UpdateAndDraw() {
  // Update...
  for (unsigned int i = 0; i < particle_count; i++) {
    glm::vec3 old_loc = glm::vec3(points[i], points[i+1], points[i+2]);
    glm::vec4 old_colour = glm::vec4(colours[i], colours[i+1], colours[i+2],
                                     colours[i+3]);
    GLParticlePoint<T> pt = updateFn(old_loc, old_colour,
                                     std::move(metadata_ptrs[i]));
    auto loc = pt.location;
    auto col = pt.colour;
    points[i] = loc.x; points[i+1] = loc.y; points[i+2] = loc.z;
    colours[i] = col.r; colours[i+1] = col.b;
    colours[i+2] = col.b; colours[i+3] = col.a;
    metadata_ptrs[i] = std::move(pt.user_metadata);
  }

  gl::BindBuffer(gl::ARRAY_BUFFER, vbo);
  gl::BufferData(gl::ARRAY_BUFFER, particle_count * 3 * sizeof(float),
                 points.data(), gl::DYNAMIC_DRAW);
  gl::EnableVertexAttribArray(attr_vertices);
  gl::VertexAttribPointer(0, 3, gl::FLOAT, gl::FALSE_, 0, 0);

  gl::BindBuffer(gl::ARRAY_BUFFER, cbo);
  gl::BufferData(gl::ARRAY_BUFFER, particle_count * 4 * sizeof(float),
                 colours.data(), gl::DYNAMIC_DRAW);
  gl::EnableVertexAttribArray(attr_colours);
  gl::VertexAttribPointer(0, 4, gl::FLOAT, gl::FALSE_, 0, 0);

  // ...and draw.
  gl::DrawArrays(gl::POINTS, 0, static_cast<GLsizei>(particle_count));
}

} // namespace guts