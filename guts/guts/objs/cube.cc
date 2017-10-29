#include "cube.h"

#include "guts/internal/render.h"
#include "guts/internal/buffers.h"
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
} // namespace

Cube::Cube(GLuint attr_vertices, GLuint attr_colours, GLuint attr_normals) {
  this->attr_vertices = attr_vertices;
  this->attr_colours = attr_colours;
  this->attr_normals = attr_normals;

  std::copy(TMPL_VERTICES.begin(), TMPL_VERTICES.end(),
            std::begin(this->vertices));
  std::copy(TMPL_COLOURS.begin(), TMPL_COLOURS.end(),
            std::begin(this->colours));
  std::copy(TMPL_NORMALS.begin(), TMPL_NORMALS.end(),
            std::begin(this->normals));

  this->vbo = guts::internal::GenBuffer(this->vertices, sizeof(this->vertices));
  this->cbo = guts::internal::GenBuffer(this->colours, sizeof(this->colours));
  this->nbo = guts::internal::GenBuffer(this->normals, sizeof(this->normals));
}

Cube::~Cube() {
  gl::DeleteBuffers(1, &this->vbo);
  gl::DeleteBuffers(1, &this->cbo);
  gl::DeleteBuffers(1, &this->nbo);
}

void Cube::Render(GLRenderMode mode) {
  guts::internal::RenderBasicObject(mode, 36, this->vbo, this->attr_vertices,
                                    this->cbo, this->attr_colours,
                                    this->nbo, this->attr_normals);
}

} // namespace objs
} // namespace guts