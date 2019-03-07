// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <GL/gl.h>
#include <GL/glu.h>

#include <glog/logging.h>

#include "common/utils/common/defines.h"
#include "common/utils/containers/array_view.h"
#include "common/utils/containers/fixed_array.h"
#include "common/utils/display/color.h"
#include "common/utils/gl/context.h"
#include "common/utils/gl/functions.h"
#include "common/utils/gl/vertexbuffer.h"
#include "common/utils/math/dimension.h"
#include "common/utils/math/transform/transform.h"
#include "common/utils/math/vec2d.h"
#include "common/utils/math/vec3d.h"

namespace gl {
class Context;
class Texture;
}  // namespace gl

namespace utils {
namespace display {

class PrimitivePainter {
 public:
  struct PointStyle {
    Color point_color = Color::White();
    float point_size = 1.0;

    PointStyle() = default;
    PointStyle(Color point_color_, float point_size_)
        : point_color(point_color_), point_size(point_size_) {}
  };

  struct LineStyle {
    Color line_color = Color::White();
    float line_width = 1.0f;
    bool is_dash_line = false;

    LineStyle() = default;
    LineStyle(Color line_color_, float line_width_, bool is_dash_line_)
        : line_color(line_color_), line_width(line_width_), is_dash_line(is_dash_line_) {}
  };

  struct StippleLineStyle {
    LineStyle line_style;
    int stipple_factor = -1;
    int stipple_pattern = -1;

    StippleLineStyle() = default;
    StippleLineStyle(Color line_color_, float line_width_)
        : line_style(line_color_, line_width_, false) {}
    StippleLineStyle(Color color, float width, int factor, int pattern)
        : line_style(color, width, false), stipple_factor(factor), stipple_pattern(pattern) {}
  };

  struct PlaneStyle {
    PlaneStyle() = default;
    explicit PlaneStyle(double alpha) : alpha(alpha) {}
    explicit PlaneStyle(Color color) : plane_color(color) {}
    PlaneStyle(Color color, double alpha) : plane_color(color), alpha(alpha) {}
    Color plane_color = Color::White();
    double alpha = 1.0;
  };

  struct SurfaceStyle {
    bool show_vertices = true;
    PointStyle vertice_style;
    bool show_edge = true;
    LineStyle edge_style;
    bool show_plane = true;
    PlaneStyle plane_style;

    SurfaceStyle() = default;
    explicit SurfaceStyle(Color color) { set_color(color); }
    void set_color(Color color) {
      vertice_style.point_color = color;
      edge_style.line_color = color;
      plane_style.plane_color = color;
    }
  };

  using PrismStyle = SurfaceStyle;

  explicit PrimitivePainter(gl::Context* context);

  template <typename PointT>
  constexpr GLenum ToGLFloatType() const {
    return std::is_same<typename math::Dimension<PointT>::ElemType, float>::value ? GL_FLOAT
                                                                                  : GL_DOUBLE;
  }

  template <typename PointT>
  constexpr int GetDimension() const {
    return math::Dimension<PointT>::kValue;
  }

  // Points.
  template <typename PointT>
  void DrawPoints(const utils::ConstArrayView<PointT>& points, const PointStyle& style) const;
  template <typename T>
  void DrawPoints(const utils::ConstArrayView<T>& points, const ConstArrayView<T>& colors,
                  float pt_size) const;
  // Lines.
  template <typename PointT>
  void DrawLines(const utils::ConstArrayView<PointT>& points, GLenum mode,
                 const LineStyle& style) const;
  template <typename PointT>
  void DrawStippleLines(const utils::ConstArrayView<PointT>& points, GLenum mode,
                        const StippleLineStyle& style) const;
  // Quads. (Deprecated)
  // In latter version openGL(3.x +), GL_QUADs not supported any more. So used GL_TRIANGLE_FAN
  // here to implement DrawQuadsXXXX.
  // And the recommended way is using DrawTriangle with GL_TRIANGLE_FAN as triangle params value.
  template <typename PointT>
  void DrawQuadsWithTexcoord(const utils::ConstArrayView<PointT>& points,
                             const utils::ConstArrayView<math::Vec2d>& texcoordds) const;
  // Plane.
  template <typename PointT>
  void DrawPlane(const utils::ConstArrayView<PointT>& vertices, const PlaneStyle& style) const;
  // Surface.
  template <typename PointT>
  void DrawSurface(const utils::ConstArrayView<PointT>& points, const SurfaceStyle& style) const;
  // Prism.
  template <typename Point2DT>
  void DrawPrism(const utils::ConstArrayView<Point2DT>& polygon, double ceiling, double floor,
                 const PrismStyle& style) const;

 private:
  gl::Context* context_;
  DISALLOW_COPY_AND_ASSIGN(PrimitivePainter);
};
}  // namespace display
}  // namespace utils

#include "common/utils/display/primitive_painter-inl.h"
