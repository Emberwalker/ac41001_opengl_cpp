#ifndef GUTS_GL_OBJECT_H
#define GUTS_GL_OBJECT_H

#include <vector>

#include <glload/gl_4_1.hpp>
#include <glm/glm.hpp>

namespace guts {
namespace objs {

class GLObject {

 public:
  virtual ~GLObject() = 0;
  void Render();
  virtual std::vector<GLfloat> GetVertices() = 0;
  virtual std::vector<GLfloat> GetVertexColours() = 0;
  virtual std::vector<GLfloat> GetNormals() = 0;

};

}
}

#endif //GUTS_GL_OBJECT_H
