// Copyright @2018 Pony AI Inc. All rights reserved.

#include "common/utils/gl/texture.h"

#include <glog/logging.h>

#include "common/utils/image/codec/codec.h"
#include "common/utils/image/codec/jpeg.h"
#include "common/utils/image/codec/netpbm.h"
#include "common/utils/image/codec/png.h"
#include "common/utils/image/image.h"

namespace gl {
namespace {
image::ImageBuffer<uint8_t, 4> RGBImage2RGBA(const image::Image<uint8_t, 3>& rawimage) {
  image::ImageBuffer<uint8_t, 4> image(rawimage.width(), rawimage.height());
  for (int y = 0; y < rawimage.height(); ++y) {
    for (int x = 0; x < rawimage.width(); ++x) {
      image(x, y)[0] = rawimage(x, y)[0];
      image(x, y)[1] = rawimage(x, y)[1];
      image(x, y)[2] = rawimage(x, y)[2];
      image(x, y)[3] = 255;
    }
  }
  return image;
}
}  // namespace

Texture::Texture(const Context* context) : Object(context) {
  f().GenTextures(1, &texture_id_);
  CHECK_NE(texture_id_, 0);
}

Texture::~Texture() {
  if (texture_id_ != 0) {
    f().DeleteTextures(1, &texture_id_);
    texture_id_ = 0;
  }
}

void Texture::CreateFromData(int width, int height, Texture::Format format, const void* data) {
  f().BindTexture(GL_TEXTURE_2D, texture_id_);

  if (format == Texture::Format::kUint8RGBA) {
    f().TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  } else {
    LOG(FATAL) << "unsupport Texture Format: " << format;
  }

  // use linear minification by default because mipmaps are not generated
  f().TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  f().TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  width_ = width;
  height_ = height;
  format_ = format;
}

void Texture::Create(int width, int height, Texture::Format format) {
  CreateFromData(width, height, format, nullptr);
}

void Texture::CreateFromImage(const image::Image<uint8_t, 4>& image) {
  CreateFromData(image.width(), image.height(), Format::kUint8RGBA, image.data());
}

void Texture::Bind() { f().BindTexture(GL_TEXTURE_2D, texture_id_); }

void Texture::Unbind() { f().BindTexture(GL_TEXTURE_2D, 0); }

void Texture::GenerateMipmap() {
  CHECK_NE(texture_id_, 0);
  f().BindTexture(GL_TEXTURE_2D, texture_id_);
  f().GenerateMipmap(GL_TEXTURE_2D);
  f().TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  f().TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  f().BindTexture(GL_TEXTURE_2D, 0);
}

std::unique_ptr<Texture> Texture::CreateFromFile(const std::string& path, Context* context) {
  image::ImageBuffer<uint8_t, 4> image;
  if (strings::EndWith(path, ".png")) {
    image::PngDecoder<4> decoder;
    CHECK_OK(decoder.Load(path, &image)) << "Failed to load Image file: " << path;
  } else if (strings::EndWith(path, ".ppm") || strings::EndWith(path, ".ppm.gz")) {
    image::ImageBuffer<uint8_t, 3> temp_image;
    image::PPMDecoder decoder;
    CHECK_OK(decoder.Load(path, &temp_image)) << "Failed to load Image file: " << path;
    image = RGBImage2RGBA(temp_image);
  } else if (strings::EndWith(path, ".jpeg") || strings::EndWith(path, ".jpg")) {
    image::ImageBuffer<uint8_t, 3> temp_image;
    image::JpegDecoder<3> decoder;
    CHECK_OK(decoder.Load(path, &temp_image)) << "Failed to load Image file: " << path;
    image = RGBImage2RGBA(temp_image);
  } else {
    LOG(FATAL) << "Unsupport file type: " << path << ", only ppm, png supported";
  }

  std::unique_ptr<Texture> texture = std::make_unique<Texture>(context);
  texture->CreateFromImage(image);
  return texture;
}
}  // namespace gl

std::ostream& operator<<(std::ostream& os, const gl::Texture::Format format) {
  switch (format) {
    case gl::Texture::Format::kUint8RGBA:
      os << "kUint8RGBAA";
      return os;
    case gl::Texture::Format::kUnknown:
    default:
      os << "Unknown Texture Format";
      return os;
  }
}
