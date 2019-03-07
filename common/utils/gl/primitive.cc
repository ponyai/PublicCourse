// Copyright @2018 Pony AI Inc. All rights reserved.

#include "common/utils/gl/primitive.h"

#include "common/utils/gl/binding_interface.h"

namespace gl {

void Primitive::SetData(const utils::ConstArrayView<math::Vec3d>& vertices,
                        const utils::ConstArrayView<math::Vec2d>& texcoords) {
  vertices_count_ = vertices.size();
  ResetAllArrayBuffers();
  CHECK(vertices_buffer_->SetData(vertices));
  CHECK(texcoords_buffer_->SetData(texcoords));
}

void Primitive::SetData(const utils::ConstArrayView<math::Vec3d>& vertices,
                        const utils::ConstArrayView<math::Vec2d>& texcoords,
                        const utils::ConstArrayView<::utils::display::Color>& colors) {
  vertices_count_ = vertices.size();
  ResetAllArrayBuffers();
  CHECK(vertices_buffer_->SetData(vertices));
  CHECK(texcoords_buffer_->SetData(texcoords));
  CHECK(colors_buffer_->SetData(colors));
}

void Primitive::SetData(const utils::ConstArrayView<math::Vec3d>& vertices,
                        const utils::ConstArrayView<::utils::display::Color>& colors) {
  vertices_count_ = vertices.size();
  ResetAllArrayBuffers();
  CHECK(vertices_buffer_->SetData(vertices));
  CHECK(colors_buffer_->SetData(colors));
}

void Primitive::SetData(const utils::ConstArrayView<math::Vec3d>& vertices,
                        const ::utils::display::Color& color) {
  std::vector<::utils::display::Color> colors(vertices.size(), color);
  SetData(vertices, utils::ConstArrayView<::utils::display::Color>(colors.data(), colors.size()));
}

void Primitive::SetVertices(const utils::ConstArrayView<glm::vec3>& vertices) {
  vertices_count_ = vertices.size();
  vertices_buffer_ = std::make_unique<VertexBuffer>(context_);
  CHECK(vertices_buffer_->SetData(vertices));
}

void Primitive::SetTexcoords(const utils::ConstArrayView<glm::vec2>& texcoords) {
  texcoords_buffer_ = std::make_unique<VertexBuffer>(context_);
  CHECK(texcoords_buffer_->SetData(texcoords));
}

void Primitive::SetColors(const utils::ConstArrayView<utils::display::Color>& colors) {
  colors_buffer_ = std::make_unique<VertexBuffer>(context_);
  CHECK(colors_buffer_->SetData(colors));
}

void Primitive::Draw() const {
  const auto& gl = context_->functions();
  gl.EnableClientState(GL_VERTEX_ARRAY);
  if (use_texture_) {
    gl.EnableClientState(GL_TEXTURE_COORD_ARRAY);
  } else {
    gl.EnableClientState(GL_COLOR_ARRAY);
  }
  if (vertices_buffer_->IsSet()) {
    BindingGuard guard(vertices_buffer_.get());
    gl.VertexPointer(vertices_buffer_->Dimension(), vertices_buffer_->GLFloatType(), 0, nullptr);
  }
  if (use_texture_) {
    if (texcoords_buffer_->IsSet()) {
      BindingGuard guard(texcoords_buffer_.get());
      gl.TexCoordPointer(texcoords_buffer_->Dimension(), texcoords_buffer_->GLFloatType(), 0,
                         nullptr);
    }
  } else {
    if (colors_buffer_->IsSet()) {
      gl::BindingGuard guard(colors_buffer_.get());
      gl.ColorPointer(colors_buffer_->Dimension(), colors_buffer_->GLFloatType(), 0, nullptr);
    }
  }
  // Call the OpenGL draw call.
  DrawData();
  if (use_texture_) {
    gl.DisableClientState(GL_TEXTURE_COORD_ARRAY);
  } else {
    gl.DisableClientState(GL_COLOR_ARRAY);
  }
  gl.DisableClientState(GL_VERTEX_ARRAY);
}

void Primitive::ResetAllArrayBuffers() {
  vertices_buffer_ = std::make_unique<VertexBuffer>(context_);
  texcoords_buffer_ = std::make_unique<VertexBuffer>(context_);
  colors_buffer_ = std::make_unique<VertexBuffer>(context_);
}

}  // namespace gl
