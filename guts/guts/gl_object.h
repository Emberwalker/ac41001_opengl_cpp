#ifndef GUTS_GL_OBJECT_H
#define GUTS_GL_OBJECT_H

namespace guts {

enum GLRenderMode {
  RENDER_NORMAL = 0,
  RENDER_WIREFRAME = 1,
  RENDER_POINTS = 2
};

GLRenderMode NextRenderMode(GLRenderMode mode);

namespace objs {

class GLObject {

 public:
  virtual void Render(GLRenderMode mode) = 0;

};

} // namespace objs
} // namespace guts

#endif //GUTS_GL_OBJECT_H
