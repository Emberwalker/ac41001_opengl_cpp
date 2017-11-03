#include "gl_object.h"

#include "guts/debug_tools.h"

namespace guts {

GLRenderMode NextRenderMode(GLRenderMode mode) {
  switch(mode) {
    case RENDER_NORMAL:
      return RENDER_WIREFRAME;
    case RENDER_WIREFRAME:
      return RENDER_POINTS;
    case RENDER_POINTS:
      return RENDER_NORMAL;
    default:
      guts_assert(false, "Invalid enum value for GLRenderMode!");
  }
}

}
