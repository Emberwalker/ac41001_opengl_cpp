#include <bits/unique_ptr.h>
#include "guts/gl_texture.h"
#include "guts/debug_tools.h"
#include "guts/tex/png_loader.h"

namespace guts {

GLTexture::GLTexture(GLuint program, const std::string &uniform_name,
                     const std::string &file_name, bool is_heightmap) {
  auto ext_pos = file_name.rfind('.');
  guts_assert(ext_pos != 0, "file name must include an extension");
  guts_assert(ext_pos + 1 < file_name.size(), "file name must include an"
      "extension");
  std::string ext = file_name.substr(ext_pos + 1);

  std::unique_ptr<tex::TextureLoader> loader;
  if (ext == "png") {
    loader = std::make_unique<tex::PNGLoader>(file_name, is_heightmap);
  } else {
    guts_error("unknown file extension: " + ext);
  }

  auto height = loader->GetImageHeight();
  auto width = loader->GetImageWidth();
  tex::TextureType type = loader->GetTextureType();
  this->texture_data = loader->GetComponents();

  // TODO(arkan)
  guts_assert(type == tex::TEXTURE_RGBA,
              "types other than RGBA not implemented yet");

  gl::GenTextures(1, &this->texture_id);
  gl::BindTexture(gl::TEXTURE_2D, this->texture_id);
  gl::TexImage2D(gl::TEXTURE_2D, 0, gl::RGBA8, width, height, 0, gl::RGBA,
                 gl::UNSIGNED_BYTE, this->texture_data.data());
  this->uniform_id = gl::GetUniformLocation(program, uniform_name.c_str());
  PrintOpenGLErrors();
}

void GLTexture::BindTexture() {
  // TODO(arkan): Make this work for multitexturing
  // ^ (glActiveTexture GL_TEXTURE0 + x)
  // from https://www.khronos.org/opengl/wiki/Texture#Texture_image_units
  gl::BindTexture(gl::TEXTURE_2D, this->texture_id);
}

void GLTexture::SetDefaultParameters() {
  gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MIN_FILTER, gl::NEAREST);
  gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MAG_FILTER, gl::NEAREST);
  gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_WRAP_S, gl::REPEAT);
  gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_WRAP_S, gl::REPEAT);
}

} // namespace guts