// Copyright @2018 Pony AI Inc. All rights reserved.

#include "common/utils/image/codec/png.h"

#include "glog/logging.h"

namespace image {

namespace {

static void WriteDataCallback(png_structp png_ptr, png_bytep data, png_size_t length) {
  std::stringstream* s = reinterpret_cast<std::stringstream*>(png_get_io_ptr(png_ptr));
  s->write(reinterpret_cast<const char*>(data), length);
}

static void ReadDataCallback(png_structp png_ptr, png_bytep data, png_size_t length) {
  std::stringstream* s = reinterpret_cast<std::stringstream*>(png_get_io_ptr(png_ptr));
  s->read(reinterpret_cast<char*>(data), length);
}

void DecodePNGHeader(const std::string& encoded, png_uint_32* width, png_uint_32* height) {
  if (!IsPngEncoded(encoded)) {
    LOG(ERROR) << "Failed to decode PNG image.";
    return;
  }
  std::stringstream source(encoded);

  png_structp png_p = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  png_infop info_p = png_create_info_struct(png_p);
  if (setjmp(png_jmpbuf(png_p))) {
    // An error occured, so clean up what we have allocated so far...
    png_destroy_read_struct(&png_p, &info_p, nullptr);
    // Make sure you return here. libPNG will jump to here if something
    // goes wrong, and if you continue with your normal code, you might
    // End up with an infinite loop.
    return;
  }
  png_set_read_fn(png_p, &source, ReadDataCallback);

  // Now call png_read_info with our png_p as image handle, and info_p to
  // receive the file info.
  png_read_info(png_p, info_p);

  *width = png_get_image_width(png_p, info_p);
  *height = png_get_image_height(png_p, info_p);

  png_destroy_read_struct(&png_p, &info_p, nullptr);
}

}  // namespace

template <int C>
PngEncoder<C>::PngEncoder() {}

template <int C>
utils::Status PngEncoder<C>::Encode(const Image<uint8_t, C>& image, std::string* encoded) {
  CHECK(encoded != nullptr);
  png_structp png_p = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  png_infop info_p = png_create_info_struct(png_p);
  if (setjmp(png_jmpbuf(png_p))) {
    // An error occured, so clean up what we have allocated so far...
    png_destroy_write_struct(&png_p, &info_p);
    // Make sure you return here. libPNG will jump to here if something
    // goes wrong, and if you continue with your normal code, you might
    // End up with an infinite loop.
    return utils::Status(utils::error::INTERNAL_ERROR, "Failed to create libpng context.");
  }

  std::stringstream ss;
  png_set_write_fn(png_p, &ss, WriteDataCallback, nullptr);

  int color_type = -1;
  switch (C) {
    case 1:
      color_type = PNG_COLOR_TYPE_GRAY;
      break;
    case 3:
      color_type = PNG_COLOR_TYPE_RGB;
      break;
    case 4:
      color_type = PNG_COLOR_TYPE_RGBA;
      break;
  }
  if (color_type == -1) {
    return utils::Status(utils::error::INTERNAL_ERROR, "Unsupported channel number");
  }
  png_set_IHDR(png_p, info_p, image.width(), image.height(), 8, color_type, PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

  std::vector<png_bytep> rows(image.height());
  for (int y = 0; y < image.height(); ++y) {
    rows[y] = const_cast<png_bytep>(image.Row(y));
  }
  png_write_info(png_p, info_p);
  png_set_rows(png_p, info_p, &rows[0]);
  png_write_png(png_p, info_p, PNG_TRANSFORM_IDENTITY, nullptr);
  png_write_end(png_p, info_p);

  *encoded = ss.str();

  png_destroy_write_struct(&png_p, &info_p);

  return utils::Status::OK;
}

template <int C>
utils::Status PngDecoder<C>::Decode(const std::string& encoded, ImageBuffer<uint8_t, C>* image) {
  CHECK(image != nullptr);

  png_uint_32 width = 0;
  png_uint_32 height = 0;
  DecodePNGHeader(encoded, &width, &height);
  image->Create(width, height);

  return DecodeToPreallocatedImage(encoded, image, false);
}

template <int C>
utils::Status PngDecoder<C>::DecodeToPreallocatedImage(const std::string& encoded,
                                                       Image<uint8_t, C>* image, bool check_size) {
  if (!IsPngEncoded(encoded)) {
    LOG(ERROR) << "Couldn't decode as a PNG image.";
    return utils::Status(utils::error::INTERNAL_ERROR, "Couldn't decode as a PNG image");
  }

  png_structp png_p = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  png_infop info_p = png_create_info_struct(png_p);
  if (setjmp(png_jmpbuf(png_p))) {
    // An error occured, so clean up what we have allocated so far...
    png_destroy_read_struct(&png_p, &info_p, nullptr);
    // Make sure you return here. libPNG will jump to here if something
    // goes wrong, and if you continue with your normal code, you might
    // End up with an infinite loop.
    return utils::Status(utils::error::INTERNAL_ERROR, "Failed to create libpng context.");
  }

  std::stringstream source(encoded);
  png_set_read_fn(png_p, &source, ReadDataCallback);

  // Now call png_read_info with our png_p as image handle, and info_p to
  // receive the file info.
  png_read_info(png_p, info_p);

  png_uint_32 width = png_get_image_width(png_p, info_p);
  png_uint_32 height = png_get_image_height(png_p, info_p);
  if (check_size) {
    CHECK_EQ(image->width(), width);
    CHECK_EQ(image->height(), height);
  }

  // bits per CHANNEL! note: not per pixel!
  png_uint_32 bitdepth = png_get_bit_depth(png_p, info_p);
  // Number of channels
  png_uint_32 channels = png_get_channels(png_p, info_p);
  // Color type. (RGB, RGBA, Luminance, luminance alpha... palette... etc)
  png_uint_32 color_type = png_get_color_type(png_p, info_p);

  switch (color_type) {
    case PNG_COLOR_TYPE_PALETTE:
      png_set_palette_to_rgb(png_p);
      // It's used later to know how big a buffer we need for the image
      channels = 3;
      break;
    case PNG_COLOR_TYPE_GRAY:
      if (bitdepth < 8) {
        png_set_expand_gray_1_2_4_to_8(png_p);
      }
      // And the bitdepth info
      bitdepth = 8;
      break;
  }

  // We don't support 16 bit precision.. so if the image Has 16 bits per channel
  // precision... round it down to 8.
  if (bitdepth == 16) {
    png_set_strip_16(png_p);
  }
  // We should let png update the information structs with the transformations
  // we requested:
  png_read_update_info(png_p, info_p);

  std::vector<png_bytep> rows(height);
  for (int y = 0; y < height; ++y) {
    rows[y] = reinterpret_cast<png_bytep>(image->data()) + y * width * channels;
  }

  png_read_image(png_p, &rows[0]);
  png_destroy_read_struct(&png_p, &info_p, nullptr);

  return utils::Status::OK;
}

// Explicit template instantiation.
template class PngEncoder<1>;
template class PngEncoder<3>;
template class PngEncoder<4>;
template class PngDecoder<1>;
template class PngDecoder<3>;
template class PngDecoder<4>;

}  // namespace image
