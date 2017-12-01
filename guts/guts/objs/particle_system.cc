#include "particle_system.h"

#include <cmath>
#include "guts/random.h"
#include "guts/internal/containers.h"
#include "guts/internal/buffers.h"
#include "guts/internal/render.h"
#include "guts/debug_tools.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace guts {
namespace objs {

namespace {

// based on https://stackoverflow.com/a/35651717
template<typename T>
T const PI = std::acos(-T(1));

glm::vec3 AXIS_Y = glm::vec3(0.f, 1.f, 0.f); // NOLINT

} // namespace

ParticleSystem::ParticleSystem(unsigned int particle_count,
                               unsigned int spread_radius,
                               double period,
                               glm::vec4 colour) {
  this->period = period;
  this->particle_count = particle_count;
  this->particles.reserve(particle_count);
  this->points.reserve(particle_count * 3);
  this->colours.reserve(particle_count * 4);

  // Generate points
  std::mt19937 twister = GetMT_RNG();
  std::uniform_real_distribution<float> spread(0.5f, spread_radius);
  std::uniform_real_distribution<float> angle(0, 2 * PI<float>);
  std::uniform_real_distribution<double> offset(0, 50);

  std::vector<glm::vec4> vec_colours;
  vec_colours.reserve(particle_count);
  std::vector<glm::vec3> vec_points;
  vec_points.reserve(particle_count);

  glm::mat4 ident4(1.f);
  for (int i = 0; i < particle_count; i++) {
    vec_colours.emplace_back(colour);
    glm::vec4 pos4(spread(twister), 0.f, 0.f, 1.f);
    pos4 = glm::rotate(ident4, angle(twister), AXIS_Y) * pos4;
    glm::vec3 pos = glm::vec3(pos4) / pos4.w;
    double p_offset = offset(twister);
    pos.y = static_cast<float>(std::fmod(p_offset, period) / period);
    vec_points.emplace_back(pos);
    this->particles.push_back(ParticleInstance { pos, p_offset });
  }

  this->colours = internal::ToComponents(vec_colours);
  this->points = internal::ToComponents(vec_points);

  this->vbo = internal::GenBuffer(this->points);
  this->cbo = internal::GenBuffer(this->colours);
  PrintOpenGLErrors();
}

void ParticleSystem::SetTime(double time) {
  this->current_time = std::fmod(time, period);
}

void ParticleSystem::SetPeriod(double period) {
  this->period = period;
}

void ParticleSystem::SetParticleSize(float size) {
  this->particle_size = size;
}

void ParticleSystem::Render(GLRenderMode mode) {
  // Update all alive particles (if not updated this run)
  if (current_time != last_time) {
    auto time_frac = static_cast<float>(
        std::fmod(current_time, period) / period);

    for (int i = 0; i < particles.size(); i++) {
      ParticleInstance inst = particles[i];
      // Recalculate y position
      auto y = static_cast<float>(std::fmod(inst.offset, period) / period);
      y *= time_frac;
      points[i*3 + 1] = y;
    }

    // Update last-seen timestamp
    this->last_time = current_time;
  }

  PrintOpenGLErrors();
  internal::RenderBasicObject(RENDER_POINTS, particle_count,
                              vbo, 0,
                              cbo, 1,
                              0, 0,
                              false, particle_size);
  PrintOpenGLErrors();
}

} // namespace objs
} // naespace guts