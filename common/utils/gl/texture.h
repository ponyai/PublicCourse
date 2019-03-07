// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include "common/utils/common/defines.h"

#include <iostream>

#include "common/utils/gl/object.h"
#include "common/utils/image/image.h"

namespace gl {

class Context;

class Texture : public Object {
 public:
  enum class Format {
    kUnknown,
    kUint8RGBA,  // RGBA(255)
  };

  static std::unique_ptr<Texture> CreateFromFile(const std::string& path, Context* context);

  explicit Texture(const Context* context);
  virtual ~Texture();

  void CreateFromImage(const image::Image<uint8_t, 4>& image);
  void Create(int width, int height, Format format);

  void Bind();
  void Unbind();

  void GenerateMipmap();

  int width() const { return width_; }
  int height() const { return height_; }
  GLuint texture_id() const { return texture_id_; }
  Format format() const { return format_; }

 private:
  void CreateFromData(int width, int height, Texture::Format format, const void* data);

  int width_ = 0;
  int height_ = 0;
  GLuint texture_id_ = 0;
  Format format_ = Format::kUnknown;

  DISALLOW_COPY_AND_ASSIGN(Texture);
};
}  // namespace gl

std::ostream& operator<<(std::ostream& os, const gl::Texture::Format format);
