// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include "common/utils/gl/basic_primitives.h"

#include "common/utils/containers/fixed_array.h"
#include "common/utils/gl/context.h"
#include "common/utils/gl/primitive.h"

namespace gl {

class Points : public Primitive {
 public:
  explicit Points(const Context* context);
  ~Points() override = default;
  static void DrawData(const Context* context, const RenderingState& rendering_state,
                       int vertices_count);

 protected:
  void DrawData() const override;
};

class Lines : public Primitive {
 public:
  enum class DrawMode : unsigned {
    kLines = GL_LINES,
    kLineLoop = GL_LINE_LOOP,
    kLineStrip = GL_LINE_STRIP,
  };

  explicit Lines(const Context* context, DrawMode draw_mode = DrawMode::kLines);
  ~Lines() override = default;
  void SetDrawMode(DrawMode draw_mode);
  static void DrawData(const Context* context, DrawMode draw_mode,
                       const RenderingState& rendering_state, int vertices_count);

 protected:
  void DrawData() const override;

 private:
  DrawMode draw_mode_ = DrawMode::kLines;
};

class Triangles : public Primitive {
 public:
  enum class DrawMode : unsigned {
    kTriangles = GL_TRIANGLES,
    kTriangleFan = GL_TRIANGLE_FAN,
    kTriangleStrip = GL_TRIANGLE_STRIP,
  };

  explicit Triangles(const Context* context, DrawMode draw_mode = DrawMode::kTriangles);
  ~Triangles() override = default;
  void SetDrawMode(DrawMode draw_mode);
  static void DrawData(const Context* context, DrawMode draw_mode,
                       const RenderingState& rendering_state, int vertices_count);

 protected:
  void DrawData() const override;

 private:
  DrawMode draw_mode_ = DrawMode::kTriangles;
};

class Quads : public Primitive {
 public:
  explicit Quads(const Context* contex);
  ~Quads() override = default;

 protected:
  void DrawData() const override;
};

class Polygon : public Primitive {
 public:
  explicit Polygon(const Context* context);
  ~Polygon() override = default;

  // Since polygon needs triangulation, we overwrite Primitive's default SetData functions.
  // Set the internal data buffers.
  void SetData(const utils::ConstArrayView<math::Vec3d>& vertices,
               const utils::ConstArrayView<math::Vec2d>& texcoords) override;
  void SetData(const utils::ConstArrayView<math::Vec3d>& vertices,
               const utils::ConstArrayView<::utils::display::Color>& colors) override;
  void SetData(const utils::ConstArrayView<math::Vec3d>& vertices,
               const utils::ConstArrayView<math::Vec2d>& texcoords,
               const utils::ConstArrayView<::utils::display::Color>& colors) override;
  void SetData(const utils::ConstArrayView<math::Vec3d>& vertices,
               const ::utils::display::Color& color = ::utils::display::Color::White()) override;

 protected:
  void DrawData() const override;

 private:
  void Triangulate(const utils::ConstArrayView<math::Vec3d>& vertices,
                   std::vector<int>* triangle_indices);
};

}  // namespace gl
