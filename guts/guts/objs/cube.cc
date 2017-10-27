#include "cube.h"

#include <glload/gl_4_1.hpp>
#include <glm/glm.hpp>

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

  std::copy(TMPL_VERTICES.begin(), TMPL_VERTICES.end(), std::begin(this->vertices));
  std::copy(TMPL_COLOURS.begin(), TMPL_COLOURS.end(), std::begin(this->colours));
  std::copy(TMPL_NORMALS.begin(), TMPL_NORMALS.end(), std::begin(this->normals));

  this->vbo = GenBuffer(this->vertices, sizeof(this->vertices));
  this->cbo = GenBuffer(this->colours, sizeof(this->colours));
  this->nbo = GenBuffer(this->normals, sizeof(this->normals));
}

GLuint Cube::GenBuffer(GLfloat *contents, GLsizei size) {
  GLuint buffer;
  gl::GenBuffers(1, &buffer);
  gl::BindBuffer(gl::ARRAY_BUFFER, buffer);
  gl::BufferData(gl::ARRAY_BUFFER, size, contents, gl::STATIC_DRAW);
  gl::BindBuffer(gl::ARRAY_BUFFER, 0);
  return buffer;
}

Cube::~Cube() {
  gl::DeleteBuffers(1, &this->vbo);
  gl::DeleteBuffers(1, &this->cbo);
  gl::DeleteBuffers(1, &this->nbo);
}

void Cube::Render(GLRenderMode mode) {
  gl::BindBuffer(gl::ARRAY_BUFFER, this->vbo);
  gl::EnableVertexAttribArray(this->attr_vertices);
  gl::VertexAttribPointer(this->attr_vertices, 3, gl::FLOAT, gl::FALSE_, 0, nullptr);

  gl::BindBuffer(gl::ARRAY_BUFFER, this->cbo);
  gl::EnableVertexAttribArray(this->attr_colours);
  gl::VertexAttribPointer(this->attr_colours, 4, gl::FLOAT, gl::FALSE_, 0, nullptr);

  gl::BindBuffer(gl::ARRAY_BUFFER, this->nbo);
  gl::EnableVertexAttribArray(this->attr_normals);
  gl::VertexAttribPointer(this->attr_normals, 3, gl::FLOAT, gl::FALSE_, 0, nullptr);

  gl::PointSize(3.f);

  if (mode == RENDER_WIREFRAME) {
    gl::PolygonMode(gl::FRONT_AND_BACK, gl::LINE);
  } else {
    gl::PolygonMode(gl::FRONT_AND_BACK, gl::FILL);
  }

  if (mode == RENDER_POINTS) {
    gl::DrawArrays(gl::POINTS, 0, 108);
  } else {
    gl::DrawArrays(gl::TRIANGLES, 0, 108);
  }
}

} // namespace objs
} // namespace guts