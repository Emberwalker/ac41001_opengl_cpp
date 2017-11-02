#include "cube.h"

#include "guts/internal/render.h"
#include "guts/internal/buffers.h"
#include "guts/debug_tools.h"
#include <array>

namespace guts {
namespace objs {

namespace {
constexpr std::array<GLfloat, 108> TMPL_VERTICES = {
    -0.25f, 0.25f, -0.25f,
    -0.25f, -0.25f, -0.25f,
    0.25f, -0.25f, -0.25f,

    0.25f, -0.25f, -0.25f,
    0.25f, 0.25f, -0.25f,
    -0.25f, 0.25f, -0.25f,

    0.25f, -0.25f, -0.25f,
    0.25f, -0.25f, 0.25f,
    0.25f, 0.25f, -0.25f,

    0.25f, -0.25f, 0.25f,
    0.25f, 0.25f, 0.25f,
    0.25f, 0.25f, -0.25f,

    0.25f, -0.25f, 0.25f,
    -0.25f, -0.25f, 0.25f,
    0.25f, 0.25f, 0.25f,

    -0.25f, -0.25f, 0.25f,
    -0.25f, 0.25f, 0.25f,
    0.25f, 0.25f, 0.25f,

    -0.25f, -0.25f, 0.25f,
    -0.25f, -0.25f, -0.25f,
    -0.25f, 0.25f, 0.25f,

    -0.25f, -0.25f, -0.25f,
    -0.25f, 0.25f, -0.25f,
    -0.25f, 0.25f, 0.25f,

    -0.25f, -0.25f, 0.25f,
    0.25f, -0.25f, 0.25f,
    0.25f, -0.25f, -0.25f,

    0.25f, -0.25f, -0.25f,
    -0.25f, -0.25f, -0.25f,
    -0.25f, -0.25f, 0.25f,

    -0.25f, 0.25f, -0.25f,
    0.25f, 0.25f, -0.25f,
    0.25f, 0.25f, 0.25f,

    0.25f, 0.25f, 0.25f,
    -0.25f, 0.25f, 0.25f,
    -0.25f, 0.25f, -0.25f,
};

constexpr std::array<GLfloat, 144> TMPL_COLOURS = {
    0.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f,

    0.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f,

    1.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 0.0f, 1.0f,

    1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 1.0f,

    1.0f, 0.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 1.0f,

    0.0f, 1.0f, 1.0f, 1.0f,
    0.0f, 1.0f, 1.0f, 1.0f,
    0.0f, 1.0f, 1.0f, 1.0f,
    0.0f, 1.0f, 1.0f, 1.0f,
    0.0f, 1.0f, 1.0f, 1.0f,
    0.0f, 1.0f, 1.0f, 1.0f,
};

constexpr std::array<GLfloat, 108> TMPL_NORMALS = {
    0, 0, -1.f, 0, 0, -1.f, 0, 0, -1.f,
    0, 0, -1.f, 0, 0, -1.f, 0, 0, -1.f,
    1.f, 0, 0, 1.f, 0, 0, 1.f, 0, 0,
    1.f, 0, 0, 1.f, 0, 0, 1.f, 0, 0,
    0, 0, 1.f, 0, 0, 1.f, 0, 0, 1.f,
    0, 0, 1.f, 0, 0, 1.f, 0, 0, 1.f,
    -1.f, 0, 0, -1.f, 0, 0, -1.f, 0, 0,
    -1.f, 0, 0, -1.f, 0, 0, -1.f, 0, 0,
    0, -1.f, 0, 0, -1.f, 0, 0, -1.f, 0,
    0, -1.f, 0, 0, -1.f, 0, 0, -1.f, 0,
    0, 1.f, 0, 0, 1.f, 0, 0, 1.f, 0,
    0, 1.f, 0, 0, 1.f, 0, 0, 1.f, 0,
};

constexpr std::array<GLfloat, 78> TMPL_TEX = {
    0, 1.f, 0, 0, 1.f, 0,
    1.f, 0, 1.f, 1.f, 0, 1.f,
    1.f, 0, 1.f, 0, 1.f, 1.f,
    0, 0, 0, 1.f, 1.f, 0,
    0, 1.f, 1.f, 1.f, 1.f, 0,
    1.f, 0, 0, 0, 1.f, 1.f,
    0, 0, 0, 1.f, 1.f, 1.f,
    0, 1.f, 0, 0, 1.f, 1.f,
    0, 0, 1.f, 0, 1.f, 1.f,
    0, 1.f, 1.f, 1.f, 1.f, 0,
    1.f, 0, 0, 0, 0, 1.f,
    0, 0, 1.f, 0, 1.f, 1.f,
    1.f, 1.f, 0, 1.f, 0, 0
};
} // namespace

Cube::Cube(bool textured, GLuint attr_vertices, GLuint attr_colours_or_tex,
           GLuint attr_normals) {
  guts::PrintOpenGLErrors();
  this->attr_vertices = attr_vertices;
  this->attr_colours_or_tex = attr_colours_or_tex;
  this->attr_normals = attr_normals;

  this->vertices = std::vector<GLfloat>(TMPL_VERTICES.begin(),
                                        TMPL_VERTICES.end());
  this->normals = std::vector<GLfloat>(TMPL_NORMALS.begin(),
                                       TMPL_NORMALS.end());

  if (textured) {
    this->colours_or_tex = std::vector<GLfloat>(TMPL_TEX.begin(),
                                                TMPL_TEX.end());
  } else {
    this->colours_or_tex = std::vector<GLfloat>(TMPL_COLOURS.begin(),
                                                TMPL_COLOURS.end());
  }

  this->vbo = guts::internal::GenBuffer(this->vertices);
  this->c_t_bo = guts::internal::GenBuffer(this->colours_or_tex);
  this->nbo = guts::internal::GenBuffer(this->normals);
  guts::PrintOpenGLErrors();
}

Cube::~Cube() {
  gl::DeleteBuffers(1, &this->vbo);
  gl::DeleteBuffers(1, &this->c_t_bo);
  gl::DeleteBuffers(1, &this->nbo);
}

void Cube::Render(GLRenderMode mode) {
  guts::PrintOpenGLErrors();
  guts::internal::RenderBasicObject(mode, 36, this->vbo, this->attr_vertices,
                                    this->c_t_bo, this->attr_colours_or_tex,
                                    this->nbo, this->attr_normals);
  guts::PrintOpenGLErrors();
}

} // namespace objs
} // namespace guts