#ifndef GUTS_OBJS_TERRAIN_H
#define GUTS_OBJS_TERRAIN_H

#include "guts/gl_object.h"
#include <vector>
#include <memory>

// We forward declare these because noiseutils pollutes the global namespace
// with `using namespace noise;`
namespace noise {
namespace utils {
class NoiseMapBuilderPlane;
class NoiseMap;
} // namespace utils
} // namespace noise

namespace guts {
namespace objs {

enum TerrainZone {
  ZONE_SEA = 0,
  ZONE_SAND = 1,
  ZONE_GRASS = 2,
  ZONE_STONE = 3,
  ZONE_SNOW = 4,
};

// Represents a simple terrain mesh. This is backed by libnoise and its
// accompanying utility library, noiseutils.
// Based heavily on the libnoise tutorial.
class Terrain : public GLObject {
 public:
  // Builds a new terrain with default Perlin parameters.
  Terrain(unsigned int x_length, unsigned int z_length);

  // Builds a new terrain using the given plane builder. The builder must have
  // a source module set at minimum.
  Terrain(unsigned int x_length, unsigned int z_length,
          noise::utils::NoiseMapBuilderPlane &builder);

  ~Terrain();

  // Override the shader parameter layout.
  void OverrideAttributeLayout(GLuint attr_vertices,
                               GLuint attr_colours,
                               GLuint attr_normals);

  // Dumps the height map with pretty colours to file_name.bmp in CWD.
  void DumpHeightMapImage(const std::string &file_name);

  // Renders the terrain.
  void Render(GLRenderMode mode) override;

  float HeightAtPoint(unsigned int x, unsigned int z);
  TerrainZone ZoneAtPoint(unsigned int x, unsigned int z);
  std::vector<glm::vec3> GetPointsInZone(TerrainZone zone);

 private:
  void Init(unsigned int x_length, unsigned int z_length,
            noise::utils::NoiseMapBuilderPlane &builder);
  unsigned int x_length, z_length;
  unsigned int vbo, cbo, nbo, element_array;
  unsigned int attr_vertices = 0, attr_colours = 1, attr_normals = 2;
  unsigned int vertex_count;
  float scaling_factor;
  std::vector<float> vertices, colours, normals;
  std::vector<unsigned int> indices;
  std::vector<glm::vec3> points_by_zone[5];
  std::unique_ptr<noise::utils::NoiseMap> heightMap;
};

} // namespace objs
} // namespace guts

#endif //GUTS_OBJS_TERRAIN_H
