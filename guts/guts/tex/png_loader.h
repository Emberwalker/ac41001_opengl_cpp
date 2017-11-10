#ifndef GUTS_TEX_PNG_LOADER_H
#define GUTS_TEX_PNG_LOADER_H

#include "texture_loader.h"

namespace guts {
namespace tex {

class PNGLoader : public TextureLoader {

 public:
  explicit PNGLoader(const std::string &file_name, bool is_heightmap = false);
  TextureType GetTextureType() override;
  unsigned int GetImageWidth() override;
  unsigned int GetImageHeight() override;
  std::vector<ubyte> GetComponents() override;

 private:
  std::vector<ubyte> components;
  unsigned int width, height;
  bool is_heightmap;
  TextureType type;

};

} // namespace tex
} // namespace guts

#endif //GUTS_TEX_PNG_LOADER_H
