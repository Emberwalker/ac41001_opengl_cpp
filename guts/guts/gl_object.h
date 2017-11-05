#ifndef GUTS_GL_OBJECT_H
#define GUTS_GL_OBJECT_H

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

} // namespace objs
} // namespace guts

#endif //GUTS_GL_OBJECT_H
