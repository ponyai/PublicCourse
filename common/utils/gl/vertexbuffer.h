// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <glog/logging.h>

#include "common/utils/common/defines.h"
#include "common/utils/containers/array_view.h"
#include "common/utils/display/color.h"
#include "common/utils/gl/binding_interface.h"
#include "common/utils/gl/context.h"
#include "common/utils/gl/geometry_utils.h"
#include "common/utils/gl/object.h"

namespace gl {

class VertexBuffer : public Object, public BindingInterface {
 public:
  enum class DrawMode : unsigned { kStaticDraw = GL_STATIC_DRAW, kStreamDraw = GL_STREAM_DRAW };

  enum class Type : unsigned {
    kArrayBuffer = GL_ARRAY_BUFFER,
    kElementArrayBuffer = GL_ELEMENT_ARRAY_BUFFER
  };

  explicit VertexBuffer(const Context* context, DrawMode mode = DrawMode::kStaticDraw,
                        Type type = Type::kArrayBuffer);
  ~VertexBuffer();

  GLenum GLFloatType() { return float_type_; }

  int Dimension() const { return float_dim_; }

  bool IsSet() const { return is_set_; }

  template <typename DataType>
  MUST_USE_RESULT bool SetData(const utils::ConstArrayView<DataType>& data);

  void Bind() override;
  void Unbind() override;

 private:
  GLuint vbo_ = 0;
  GLuint mode_ = 0;
  GLuint type_ = 0;
  bool is_set_ = false;
  bool is_bound_ = false;
  int float_dim_ = 3;
  GLenum float_type_ = GL_FLOAT;
};

template <class DataType>
bool VertexBuffer::SetData(const utils::ConstArrayView<DataType>& data) {
  if (vbo_ == 0) {
    return false;
  }
  Bind();
  float_dim_ = geometry_utils::GetDimension<DataType>();
  float_type_ = geometry_utils::GetGLFloatType<DataType>();
  f().BufferData(type_, sizeof(DataType) * data.size(), data.begin(), mode_);
  Unbind();
  is_set_ = true;
  return true;
}

}  // namespace gl
