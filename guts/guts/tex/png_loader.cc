#include "png_loader.h"

#include "guts/debug_tools.h"
#include <stdexcept>
#include <iostream>
#include <cstdio>
#include <png/png.h>

namespace guts {
namespace tex {

const unsigned int SIGNATURE_BYTES_TO_CHECK = 8;

PNGLoader::PNGLoader(const std::string &file_name, bool is_heightmap) {
  this->is_heightmap = is_heightmap;

  // This mostly follows libpng's example.c, but ported to C++
  // Get C-style FILE handle.
  FILE *fp = std::fopen(file_name.c_str(), "rb");
  if (!fp) throw std::invalid_argument("unable to open file " + file_name);

  // First check we actually have a valid PNG.
  unsigned char signature_buf[SIGNATURE_BYTES_TO_CHECK];
  if (std::fread(signature_buf, 1, SIGNATURE_BYTES_TO_CHECK, fp) !=
      SIGNATURE_BYTES_TO_CHECK) {
    throw std::invalid_argument("unable to fetch signature; is this definitely "
                                    "a PNG file?");
  }

  if (!png_check_sig(const_cast<png_const_bytep>(&signature_buf[0]),
                     SIGNATURE_BYTES_TO_CHECK)) {
    throw std::invalid_argument("file signature is not a valid PNG signature.");
  }

  // Get the libpng structures we need.
  png_structp png_ptr;
  png_infop info_ptr;

  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr,
                                   nullptr);
  if (!png_ptr) {
    std::fclose(fp);
    exit(EXIT_FAILURE);
  }

  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    std::fclose(fp);
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    exit(EXIT_FAILURE);
  }

  if (setjmp(png_jmpbuf(png_ptr))) { // NOLINT - ignore setjmp vs exceptions
    // This is a jump target for libpng if something goes wrong - in proper C++
    // this would be exceptions, but libpng is a C library.
    std::fclose(fp);
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    exit(EXIT_FAILURE);
  }

  // Setup libpng I/O
  png_init_io(png_ptr, fp);
  png_set_sig_bytes(png_ptr, SIGNATURE_BYTES_TO_CHECK);

  // Use the higher-level path. No need to make this harder.
  // We also set the EXPAND transform. It'll make life easier later.
  png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, nullptr);

  this->width = png_get_image_width(png_ptr, info_ptr);
  this->height = png_get_image_height(png_ptr, info_ptr);

  png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);
  // Channels in the image (e.g. 4 for RGBA)
  unsigned int channels = png_get_channels(png_ptr, info_ptr);
  // Bits *per sample* (i.e. per pixel)
  unsigned int bit_depth = png_get_bit_depth(png_ptr, info_ptr);
  guts_assert(bit_depth == 8, "libpng bit depth is 8 bits (1 byte)");

  if (is_heightmap) {
    this->type = TEXTURE_HEIGHTMAP;
  } else if (channels == 4) {
    this->type = TEXTURE_RGBA;
  } else if (channels == 3) {
    this->type = TEXTURE_RGB;
  } else {
    std::cerr << "png: unknown channel count " << channels << std::endl;
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    std::fclose(fp);
    exit(EXIT_FAILURE);
  }

  this->components.reserve(this->width * this->height * channels);
  for (int row = 0; row < this->height; row++) {
    png_bytep row_ptr = row_pointers[row];
    for (int col = 0; col < this->width * channels; col++) {
      this->components.push_back(row_ptr[col]);
    }
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  std::fclose(fp);
}

TextureType PNGLoader::GetTextureType() {
  return this->type;
}

unsigned int PNGLoader::GetImageWidth() {
  return this->width;
}

unsigned int PNGLoader::GetImageHeight() {
  return this->height;
}

std::vector<ubyte> PNGLoader::GetComponents() {
  return this->components;
}

} // namespace tex
} // namespace guts