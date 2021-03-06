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

constexpr std::array<GLfloat, 72> TMPL_TEX = {
    0, 1.f, 0, 0, 1.f, 0,
    1.f, 0, 1.f, 1.f, 0, 1.f,

    0, 0, 1.f, 0, 0, 1.f,
    1.f, 0, 1.f, 1.f, 0, 1.f,

    0, 0, 1.f, 0, 0, 1.f,
    1.f, 0, 1.f, 1.f, 0, 1.f,

    0, 0, 1.f, 0, 0, 1.f,
    1.f, 0, 1.f, 1.f, 0, 1.f,

    0, 1.f, 0, 0, 1.f, 0,
    1.f, 0, 1.f, 1.f, 0, 1.f,

    0, 1.f, 0, 0, 1.f, 0,
    1.f, 0, 1.f, 1.f, 0, 1.f,
};
} // namespace

Cube::Cube(bool textured, glm::vec4 *override_colour, GLuint attr_vertices, GLuint attr_colours_or_tex,
           GLuint attr_normals) {
  guts::PrintOpenGLErrors();
  this->attr_vertices = attr_vertices;
  this->attr_colours_or_tex = attr_colours_or_tex;
  this->attr_normals = attr_normals;
  this->textured = textured;

  this->vertices = std::vector<GLfloat>(TMPL_VERTICES.begin(),
                                        TMPL_VERTICES.end());
  this->normals = std::vector<GLfloat>(TMPL_NORMALS.begin(),
                                       TMPL_NORMALS.end());

  if (textured) {
    this->colours_or_tex = std::vector<GLfloat>(TMPL_TEX.begin(),
                                                TMPL_TEX.end());
  } else if (override_colour) {
    std::vector<GLfloat> colours;
    auto vertex_count = vertices.size() / 3;
    colours.reserve(vertex_count * 4);
    for (int i = 0; i < vertex_count; i++) {
      colours.push_back(override_colour->r);
      colours.push_back(override_colour->g);
      colours.push_back(override_colour->b);
      colours.push_back(override_colour->a);
    }
    this->colours_or_tex = colours;
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
  GLuint c_tex_size = textured ? 2 : 4;
  guts::internal::RenderBasicObject(mode, 36, this->vbo, this->attr_vertices, 3,
                                    this->c_t_bo, this->attr_colours_or_tex, c_tex_size,
                                    this->nbo, this->attr_normals, 3);
  guts::PrintOpenGLErrors();
}

} // namespace objs
} // namespace guts