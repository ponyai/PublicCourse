// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <glm/glm.hpp>

#include "common/utils/display/color.h"
#include "common/utils/gl/context.h"
#include "common/utils/gl/rendering_state.h"
#include "common/utils/gl/vertexbuffer.h"

namespace gl {

class Primitive {
 public:
  explicit Primitive(const Context* context) : context_(CHECK_NOTNULL(context)) {}
  virtual ~Primitive() = default;

  // The primary function to draw the primitive.
  virtual void Draw() const;
  // Set the internal data buffers.
  virtual void SetData(const utils::ConstArrayView<math::Vec3d>& vertices,
                       const utils::ConstArrayView<math::Vec2d>& texcoords);
  virtual void SetData(const utils::ConstArrayView<math::Vec3d>& vertices,
                       const utils::ConstArrayView<math::Vec2d>& texcoords,
                       const utils::ConstArrayView<::utils::display::Color>& colors);
  virtual void SetData(const utils::ConstArrayView<math::Vec3d>& vertices,
                       const utils::ConstArrayView<::utils::display::Color>& colors);
  virtual void SetData(const utils::ConstArrayView<math::Vec3d>& vertices,
                       const ::utils::display::Color& color = ::utils::display::Color::White());
  // Newer interfaces.
  virtual void SetVertices(const utils::ConstArrayView<glm::vec3>& vertices);
  virtual void SetTexcoords(const utils::ConstArrayView<glm::vec2>& texcoords);
  virtual void SetColors(const utils::ConstArrayView<utils::display::Color>& colors);
  // Remember to set this flag to true if you want to use the texcoord-array-buffer before any draw
  // call.
  void SetTextureFlag(bool use_texture) { use_texture_ = use_texture; }
  RenderingState* GetMutableRenderingState() { return &rendering_state_; }

 protected:
  // This pure virtual functions defines how to send data to OpenGL(the exact OpenGL draw-call).
  virtual void DrawData() const = 0;
  void ResetAllArrayBuffers();

 protected:
  const Context* context_ = nullptr;
  int vertices_count_ = 0;
  bool use_texture_ = false;
  RenderingState rendering_state_;
  std::unique_ptr<VertexBuffer> vertices_buffer_;
  std::unique_ptr<VertexBuffer> texcoords_buffer_;
  std::unique_ptr<VertexBuffer> colors_buffer_;
};

}  // namespace gl
