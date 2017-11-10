#ifndef GUTS_TEX_TEXTURE_LOADER_H
#define GUTS_TEX_TEXTURE_LOADER_H

#include <string>
#include <vector>

namespace guts {
namespace tex {

// C++ lacks a byte type - but an unsigned char is good enough.
typedef unsigned char ubyte;

enum TextureType {
  TEXTURE_RGB,
  TEXTURE_RGBA,
  TEXTURE_HEIGHTMAP
};

// TextureLoader represents a loader for a certain file format e.g. a PNG
// texture loader.
class TextureLoader {

 public:
  virtual ~TextureLoader() = default;

  // Gets the texture type this loader instance contains.
  virtual TextureType GetTextureType() = 0;

  virtual unsigned int GetImageWidth() = 0;
  virtual unsigned int GetImageHeight() = 0;

  // Gets the raw components of the image.
  // GetComponents().size() must be divisible by 3 for RGB sources, or 4 for
  // RGBA sources. Height maps have no extra size restrictions, as each point is
  // 1 item. The size must also match up with the width * height of the image.
  // It is suggested that this value be created when this loader is created, and
  // then cached.
  virtual std::vector<ubyte> GetComponents() = 0;

};

} // namespace tex
} // namespace guts

#endif //GUTS_TEX_TEXTURE_LOADER_H
