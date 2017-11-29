#ifndef GUTS_GL_OBJECT_H
#define GUTS_GL_OBJECT_H

#include "uniform.h"
#include <stack>

namespace guts {

enum GLRenderMode {
  RENDER_NORMAL = 0,
  RENDER_WIREFRAME = 1,
  RENDER_POINTS = 2
};

// Gets the next render mode after this one.
GLRenderMode NextRenderMode(GLRenderMode mode);

namespace objs {

// Represents an abstract OpenGL renderable object. Base class for all the
// classes in guts::objs (guts/objs/*.h)
class GLObject {

 public:
  virtual void Render(GLRenderMode mode) = 0;

};

class MultipartGLObject {

 public:
  virtual void Render(GLRenderMode mode,
                      GLUniform<glm::mat4> &model_uniform,
                      glm::mat4 &model_matrix,
                      GLUniform<glm::mat3> &normal_uniform,
                      glm::mat4 &view_matrix) = 0;

};

} // namespace objs
} // namespace guts

#endif //GUTS_GL_OBJECT_H
