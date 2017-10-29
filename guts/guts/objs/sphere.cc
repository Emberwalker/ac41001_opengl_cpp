#include "sphere.h"
#include "guts/internal/buffers.h"

#include <glm/glm.hpp>

namespace guts {
namespace objs {

namespace {

} // namespace

Sphere::Sphere(int lats, int longs, GLuint attr_vertices, GLuint attr_colours,
               GLuint attr_normals) {
  this->lats = lats;
  this->longs = longs;
  this->vertices_count = 2 + ((this->lats - 1) * this->longs);

  this->attr_vertices = attr_vertices;
  this->attr_colours = attr_colours;
  this->attr_normals = attr_normals;

  this->vertices.resize(this->vertices_count * 3);
  this->colours.resize(this->vertices_count * 4);

  // makeUnitSphere() START
  GLfloat DEG_TO_RADIANS = 3.141592f / 180.f;
  GLuint vnum = 0;
  GLfloat x, y, z, lat_radians, lon_radians;

  /* Define north pole */
  this->vertices[0] = 0; this->vertices[1] = 0; this->vertices[2] = 1.f;
  vnum++;

  GLfloat latstep = 180.f / this->lats;
  GLfloat longstep = 360.f / this->longs;

  /* Define vertices along latitude lines */
  for (GLfloat lat = 90.f - latstep; lat > -90.f; lat -= latstep)
  {
    lat_radians = lat * DEG_TO_RADIANS;
    for (GLfloat lon = -180.f; lon < 180.f; lon += longstep)
    {
      lon_radians = lon * DEG_TO_RADIANS;

      x = cos(lat_radians) * cos(lon_radians);
      y = cos(lat_radians) * sin(lon_radians);
      z = sin(lat_radians);

      /* Define the vertex */
      this->vertices[vnum * 3] = x; this->vertices[vnum * 3 + 1] = y; this->vertices[vnum * 3 + 2] = z;
      vnum++;
    }
  }
  /* Define south pole */
  this->vertices[vnum * 3] = 0; this->vertices[vnum * 3 + 1] = 0; this->vertices[vnum * 3 + 2] = -1.f;
  // makeUnitSphere() END

  /* Define colours as the x,y,z components of the sphere vertices */
  for (int i = 0; i < this->vertices_count; i++) {
    this->colours[i * 4] = this->vertices[i * 3];
    this->colours[i * 4 + 1] = this->vertices[i * 3 + 1];
    this->colours[i * 4 + 2] = this->vertices[i * 3 + 2];
    this->colours[i * 4 + 3] = 1.f;
  }

  int indices_count =
      ((this->longs * 2) + 2) * (this->lats - 1) + ((this->longs + 2) * 2);
  this->indices.resize(indices_count);

  GLuint idx = 0;

  // Define indices for the first triangle fan for one pole
  for (int i = 0; i < this->longs + 1; i++) {
    this->indices[idx++] = i;
  }
  this->indices[idx++] = 1;	// Join last triangle in the triangle fan

  GLuint start = 1;		// Start index for each latitude row
  for (int j = 0; j < this->lats - 2; j++) {
    for (int i = 0; i < this->longs; i++) {
      this->indices[idx++] = start + i;
      this->indices[idx++] = start + i + this->longs;
    }
    // close the triangle strip loop by going back to the first vertex in the
    // loop
    this->indices[idx++] = start;
    this->indices[idx++] = start + this->longs;

    start += this->longs;
  }

  // Define indices for the last triangle fan for the south pole region
  for (int i = this->vertices_count - 1;
       i > (this->vertices_count - this->longs - 2); i--) {
    this->indices[idx++] = i;
  }
  this->indices[idx] = this->vertices_count - 2; // Tie up last triangle in fan

  // Generate buffers after the vectors are fully constructed.
  this->vbo = guts::internal::GenBuffer((GLfloat *) &this->vertices.front(),
                                        this->vertices_count * 3);
  this->cbo = guts::internal::GenBuffer((GLfloat *) &this->colours.front(),
                                        this->vertices_count * 4);
  this->nbo = guts::internal::GenBuffer((GLfloat *) &this->vertices.front(),
                                        this->vertices_count * 3);
  this->element_array = guts::internal::GenElementBuffer(
      (GLuint *) this->indices.front(), this->indices.size());
}

Sphere::~Sphere() {
  gl::DeleteBuffers(1, &this->vbo);
  gl::DeleteBuffers(1, &this->cbo);
  gl::DeleteBuffers(1, &this->nbo);
  gl::DeleteBuffers(1, &this->element_array);
}

void Sphere::Render(GLRenderMode mode) {
  /* Draw the vertices as GL_POINTS */
  gl::BindBuffer(gl::ARRAY_BUFFER, this->vbo);
  gl::VertexAttribPointer(this->attr_vertices, 3, gl::FLOAT, gl::FALSE_, 0,
                          nullptr);
  gl::EnableVertexAttribArray(this->attr_vertices);

  /* Bind the sphere normals */
  gl::BindBuffer(gl::ARRAY_BUFFER, this->nbo);
  gl::VertexAttribPointer(this->attr_normals, 3, gl::FLOAT, gl::FALSE_, 0,
                          nullptr);
  gl::EnableVertexAttribArray(this->attr_normals);

  /* Bind the sphere colours */
  gl::BindBuffer(gl::ARRAY_BUFFER, this->cbo);
  gl::VertexAttribPointer(this->attr_colours, 4, gl::FLOAT, gl::FALSE_, 0,
                          nullptr);
  gl::EnableVertexAttribArray(this->attr_colours);

  gl::PointSize(3.f);

  // Enable this line to show model in wireframe
  if (mode == RENDER_WIREFRAME) {
    gl::PolygonMode(gl::FRONT_AND_BACK, gl::LINE);
  } else {
    gl::PolygonMode(gl::FRONT_AND_BACK, gl::FILL);
  }

  if (mode == RENDER_POINTS) {
    gl::DrawArrays(gl::POINTS, 0, this->vertices_count);
  } else {
    // Bind the indexed vertex buffer
    gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, this->element_array);

    // Draw the north pole regions as a triangle fan
    gl::DrawElements(gl::TRIANGLE_FAN, this->longs + 2, gl::UNSIGNED_INT,
                     nullptr);

    // Calculate offsets into the indexed array. Note that we multiply offsets
    // by 4 because it is a memory offset, the indices are type GLuint which is
    // 4-bytes
    GLuint lat_offset_jump = (this->longs * 2) + 2;
    GLuint lat_offset_start = this->longs + 2;
    GLuint lat_offset_current = lat_offset_start * 4;

    /* Draw the triangle strips of latitudes */
    for (int i = 0; i < this->lats - 2; i++)
    {
      gl::DrawElements(gl::TRIANGLE_STRIP, this->longs * 2 + 2,
                       gl::UNSIGNED_INT, (GLvoid*)(lat_offset_current));
      lat_offset_current += (lat_offset_jump * 4);
    }
    /* Draw the south pole as a triangle fan */
    gl::DrawElements(gl::TRIANGLE_FAN, this->longs + 2, gl::UNSIGNED_INT,
                     (GLvoid*)(lat_offset_current));
  }
}

} // namespace objs
} // namespace guts