// Copyright @2018 Pony AI Inc. All rights reserved.

#include "common/utils/gl/vertexbuffer.h"

namespace gl {

VertexBuffer::VertexBuffer(const Context* context, DrawMode mode, Type type)
    : Object(context), mode_(static_cast<GLuint>(mode)), type_(static_cast<GLuint>(type)) {
  f().GenBuffers(1, &vbo_);
  if (vbo_ == 0) {
    LOG(ERROR) << "Failed to create vertex buffer object!";
  }
}

VertexBuffer::~VertexBuffer() {
  if (vbo_) {
    CHECK(!is_bound_) << "Trying to delete a vertex buffer object which is in use.";
    f().DeleteBuffers(1, &vbo_);
  }
}

void VertexBuffer::Bind() {
  f().BindBuffer(type_, vbo_);
  is_bound_ = true;
}

void VertexBuffer::Unbind() {
  f().BindBuffer(type_, 0);
  is_bound_ = false;
}

}  // namespace gl
