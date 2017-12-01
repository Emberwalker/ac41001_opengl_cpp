#ifndef GUTS_OBJS_PARTICLE_SYSTEM_H
#define GUTS_OBJS_PARTICLE_SYSTEM_H

#include "guts/gl_object.h"

#include <vector>

namespace guts {
namespace objs {

// Internal struct to hold system state for each instance
struct ParticleInstance {
  glm::vec3 pos;
  double offset;
};

class ParticleSystem: public GLObject {

 public:
  ParticleSystem(unsigned int particle_count, unsigned int spread_radius,
                 double period,
                 glm::vec4 colour);
  void SetTime(double time);
  void SetPeriod(double period);
  void SetParticleSize(float size);
  void Render(GLRenderMode mode) override;

 private:
  std::vector<ParticleInstance> particles;
  std::vector<float> points, colours;
  double period;
  double current_time = 0;
  double last_time = 0;
  float particle_size = 10.f;
  unsigned int vbo, cbo, particle_count;

};

} // namespace objs
} // naespace guts

#endif //GUTS_OBJS_PARTICLE_SYSTEM_H
