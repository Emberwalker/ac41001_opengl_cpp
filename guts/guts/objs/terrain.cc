#include "terrain.h"

#include <limits>
#include <random>
#include "guts/internal/buffers.h"
#include "guts/internal/containers.h"
#include "guts/random.h"
#include "noise/noise.h"
#include "guts/external/noiseutils.h"

namespace guts {
namespace objs {

namespace {

const float BORDER_VALUE = -100.f;
const float SCALING_FACTOR = 75.f;

glm::vec4 COLOUR_SEA = glm::vec4(0.f, 0.f, 1.f, 1.f);
glm::vec4 COLOUR_SAND = glm::vec4(1.f, 0.764f, 0.f, 1.f);
glm::vec4 COLOUR_GRASS = glm::vec4(0.f, 0.75f, 0.f, 1.f);
glm::vec4 COLOUR_STONE = glm::vec4(0.3f, 0.3f, 0.3f, 1.f);
glm::vec4 COLOUR_SNOW = glm::vec4(1.f, 1.f, 1.f, 1.f);

const float BOUNDARY_SEA = 0.f;
const float BOUNDARY_SAND = 0.05f;
const float BOUNDARY_GRASS = 0.2f;
const float BOUNDARY_STONE = 0.8f;
const float BOUNDARY_SNOW = std::numeric_limits<float>::max();

} // namespace

Terrain::Terrain(unsigned int x_length,
                 unsigned int z_length,
                 utils::NoiseMapBuilderPlane &builder) {
  Init(x_length, z_length, builder);
}

Terrain::Terrain(unsigned int x_length, unsigned int z_length) {
  noise::module::Perlin perlin;
  std::mt19937 mt = GetMT_RNG();
  perlin.SetSeed(mt());
  noise::utils::NoiseMapBuilderPlane builder;
  builder.SetSourceModule(perlin);
  Init(x_length, z_length, builder);
}

Terrain::~Terrain() = default;

void Terrain::OverrideAttributeLayout(GLuint attr_vertices,
                                      GLuint attr_colours,
                                      GLuint attr_normals) {
  this->attr_vertices = attr_vertices;
  this->attr_colours = attr_colours;
  this->attr_normals = attr_normals;
}

void Terrain::Init(unsigned int x_length,
                   unsigned int z_length,
                   noise::utils::NoiseMapBuilderPlane &builder) {
  this->x_length = x_length;
  this->z_length = z_length;
  this->scaling_factor = (x_length + z_length) / SCALING_FACTOR;
  this->heightMap = std::make_unique<utils::NoiseMap>();

  // We set this so we know if we accidentally run off the end of the map
  this->heightMap->SetBorderValue(BORDER_VALUE);

  builder.SetDestNoiseMap(*this->heightMap);
  builder.SetDestSize(x_length, z_length);
  builder.SetBounds(0, 4, 0, 4);
  builder.Build();

  // Generate vertices (in vec3s) and colours (in vec4s)
  std::vector<glm::vec3> vec_vertices;
  std::vector<glm::vec4> vec_colours;
  vec_vertices.reserve(x_length * z_length);
  vec_colours.reserve(x_length * z_length);

  for (int x = 0; x < x_length; x++) {
    for (int z = 0; z < z_length; z++) {
      float y = heightMap->GetValue(x, z);
      y *= scaling_factor;
      if (y < 0) y = -0.001f; // Sea level
      auto pos = glm::vec3(x, y, z);
      vec_vertices.push_back(pos);

      if (y < BOUNDARY_SEA * scaling_factor) {
        vec_colours.emplace_back(COLOUR_SEA);
        points_by_zone[ZONE_SEA].push_back(pos);
      } else if (y < BOUNDARY_SAND * scaling_factor) {
        vec_colours.emplace_back(COLOUR_SAND);
        points_by_zone[ZONE_SAND].push_back(pos);
      } else if (y < BOUNDARY_GRASS * scaling_factor) {
        vec_colours.emplace_back(COLOUR_GRASS);
        points_by_zone[ZONE_GRASS].push_back(pos);
      } else if (y < BOUNDARY_STONE * scaling_factor) {
        vec_colours.emplace_back(COLOUR_STONE);
        points_by_zone[ZONE_STONE].push_back(pos);
      } else {
        vec_colours.emplace_back(COLOUR_SNOW);
        points_by_zone[ZONE_SNOW].push_back(pos);
      }
    }
  }

  // Convert vec3s into pure floats for OpenGL.
  this->vertices = internal::ToComponents(vec_vertices);
  this->colours = internal::ToComponents(vec_colours);
  this->vertex_count = static_cast<unsigned int>(this->vertices.size());

  // Triangle winding and normals
  std::vector<glm::vec3> vec_normals;
  vec_normals.reserve(x_length * z_length);
  unsigned int next_row = x_length;
  for (int x = 0; x < x_length - 1; x++) {
    unsigned int row_offset = x * next_row;
    for (int z = 0; z < z_length; z++) {
      unsigned int offset = row_offset + z;
      this->indices.push_back(offset);
      this->indices.push_back(offset + next_row);
      glm::vec3 here = vec_vertices[offset];

      // Generate four normals to calculate from - imagine drawing a square
      // rotated 45 degrees around the current point, split into triangles.
      glm::vec3 normal = glm::vec3(0.f);
      if (z < z_length - 1 && x > 0) {
        normal += glm::cross(vec_vertices[offset + 1] - here,
                             vec_vertices[offset - next_row] - here);
      }
      if (z > 0 && x > 0) {
        normal += glm::cross(vec_vertices[offset - next_row] - here,
                             vec_vertices[offset - 1] - here);
      }
      if (z > 0) {
        normal += glm::cross(vec_vertices[offset - 1] - here,
                             vec_vertices[offset + next_row] - here);
      }
      if (z < z_length) {
        normal += glm::cross(vec_vertices[offset + next_row] - here,
                             vec_vertices[offset + 1] - here);
      }
      normal = glm::normalize(normal);
      vec_normals.push_back(normal);
    }
  }

  // Convert normals to pure floats.
  this->normals = internal::ToComponents(vec_normals);

  this->vbo = guts::internal::GenBuffer(this->vertices);
  this->cbo = guts::internal::GenBuffer(this->colours);
  this->nbo = guts::internal::GenBuffer(this->normals);
  this->element_array = guts::internal::GenElementBuffer(this->indices);
}

void Terrain::DumpHeightMapImage(const std::string &file_name) {
  // This is directly from libnoise tutorial 3. Purely for debugging.
  noise::utils::RendererImage renderer;
  noise::utils::Image image;
  renderer.SetSourceNoiseMap(*this->heightMap);
  renderer.SetDestImage(image);
  renderer.ClearGradient();
  renderer.AddGradientPoint(-1.0000, utils::Color(  0,   0, 128, 255));
  renderer.AddGradientPoint(-0.2500, utils::Color(  0,   0, 255, 255));
  renderer.AddGradientPoint( 0.0000, utils::Color(  0, 128, 255, 255));
  renderer.AddGradientPoint( 0.0625, utils::Color(240, 240,  64, 255));
  renderer.AddGradientPoint( 0.1250, utils::Color( 32, 160,   0, 255));
  renderer.AddGradientPoint( 0.3750, utils::Color(224, 224,   0, 255));
  renderer.AddGradientPoint( 0.7500, utils::Color(128, 128, 128, 255));
  renderer.AddGradientPoint( 1.0000, utils::Color(255, 255, 255, 255));
  renderer.EnableLight();
  renderer.SetLightContrast(3.0);
  renderer.SetLightBrightness(2.0);
  renderer.Render();

  utils::WriterBMP writer;
  writer.SetSourceImage(image);
  writer.SetDestFilename(file_name + ".bmp");
  writer.WriteDestFile();
}

void Terrain::Render(GLRenderMode mode) {
  // This is mostly copied from the sphere.

  // Vertices
  gl::BindBuffer(gl::ARRAY_BUFFER, this->vbo);
  gl::VertexAttribPointer(this->attr_vertices, 3, gl::FLOAT, gl::FALSE_, 0,
                          nullptr);
  gl::EnableVertexAttribArray(this->attr_vertices);

  // Normals
  gl::BindBuffer(gl::ARRAY_BUFFER, this->nbo);
  gl::VertexAttribPointer(this->attr_normals, 3, gl::FLOAT, gl::FALSE_, 0,
                          nullptr);
  gl::EnableVertexAttribArray(this->attr_normals);

  // Colours
  gl::BindBuffer(gl::ARRAY_BUFFER, this->cbo);
  gl::VertexAttribPointer(this->attr_colours, 4, gl::FLOAT, gl::FALSE_, 0,
                          nullptr);
  gl::EnableVertexAttribArray(this->attr_colours);

  gl::PointSize(3.f);

  // RENDER_WIREFRAME handling.
  if (mode == RENDER_WIREFRAME) {
    gl::PolygonMode(gl::FRONT_AND_BACK, gl::LINE);
  } else {
    gl::PolygonMode(gl::FRONT_AND_BACK, gl::FILL);
  }

  if (mode == RENDER_POINTS) {
    gl::DrawArrays(gl::POINTS, 0, this->vertex_count);
  } else {
    // Bind the index array buffer
    gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, this->element_array);

    unsigned int row_length = sizeof(unsigned int) * x_length * 2;
    for (int x = 0; x < x_length - 1; x++) {
      gl::DrawElements(gl::TRIANGLE_STRIP, x_length * 2, gl::UNSIGNED_INT,
                       (GLvoid*)(row_length * x));
    }
  }
}

float Terrain::HeightAtPoint(unsigned int x, unsigned int z) {
  return heightMap->GetValue(x, z) * scaling_factor;
}

TerrainZone Terrain::ZoneAtPoint(unsigned int x, unsigned int z) {
  float y = HeightAtPoint(x, z);
  if (y < BOUNDARY_SEA) return ZONE_SEA;
  else if (y < BOUNDARY_SAND) return ZONE_SAND;
  else if (y < BOUNDARY_GRASS) return ZONE_GRASS;
  else if (y < BOUNDARY_STONE) return ZONE_STONE;
  else return ZONE_SNOW;
}

std::vector<glm::vec3> Terrain::GetPointsInZone(TerrainZone zone) {
  return points_by_zone[zone];
}

} // namespace objs
} // namespace guts