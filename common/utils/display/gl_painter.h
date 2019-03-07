// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <queue>

#include <GL/gl.h>
#include <QtWidgets/QOpenGLWidget>
#include <glm/glm.hpp>

#include "common/utils/display/color.h"
#include "common/utils/display/font_renderer.h"
#include "common/utils/display/primitive_painter.h"
#include "common/utils/gl/context.h"
#include "common/utils/gl/texture.h"
#include "common/utils/gl_support/base.h"
#include "common/utils/gl_support/mesh_model.h"
#include "common/utils/math/transform/rigid_transform.h"
#include "common/utils/math/transform/transform.h"
#include "common/utils/math/vec2d.h"

namespace utils {
namespace display {

class OpenglPainter {
 public:
  using PointStyle = PrimitivePainter::PointStyle;
  using LineStyle = PrimitivePainter::LineStyle;
  using StippleLineStyle = PrimitivePainter::StippleLineStyle;
  using PlaneStyle = PrimitivePainter::PlaneStyle;
  using SurfaceStyle = PrimitivePainter::SurfaceStyle;
  using PrismStyle = PrimitivePainter::PrismStyle;

  template <typename PointT>
  struct ArrowStyle {
    LineStyle line_style;
    PointT up_dir = PointT(0.0, 0.0, 1.0);
    double wing_angle = 30;

    ArrowStyle() = default;
    ArrowStyle(const LineStyle& line_style, const PointT& up_dir, double wing_angle)
        : line_style(line_style), up_dir(up_dir), wing_angle(wing_angle) {}
  };

  template <typename PointT>
  struct AxisStyle {
    bool show_origin_point = false;
    PointStyle origin_point_style;
    LineStyle x_axis_style;
    double x_axis_length = 1.0;
    LineStyle y_axis_style;
    double y_axis_length = 1.0;
    LineStyle z_axis_style;
    double z_axis_length = 1.0;

    AxisStyle() {
      // Default axis style is as http://wiki.ros.org/rviz/DisplayTypes/TF.
      // X,Y,Z -> R,G,B
      x_axis_style.line_color = utils::display::Color::Red();
      y_axis_style.line_color = utils::display::Color::Green();
      z_axis_style.line_color = utils::display::Color::Blue();
    }

    AxisStyle(const PointStyle origin_point, bool show_origin_point, const LineStyle& x_style,
              const LineStyle& y_style, const LineStyle& z_style)
        : origin_point_style(origin_point),
          show_origin_point(show_origin_point),
          x_axis_style(x_style),
          y_axis_style(y_style),
          z_axis_style(z_style) {}

    void set_axises_are_dash(bool is_dash) {
      x_axis_style.is_dash_line = is_dash;
      y_axis_style.is_dash_line = is_dash;
      z_axis_style.is_dash_line = is_dash;
    }

    void set_axises_width(double width) {
      x_axis_style.line_width = width;
      y_axis_style.line_width = width;
      z_axis_style.line_width = width;
    }
  };

  OpenglPainter(gl::Context* context, FontRenderer* font_renderer);
  virtual ~OpenglPainter() = default;

  const gl::Context* gl_context() { return gl_context_; }

  void Begin2D();
  void End2D();

  template <typename PointT>
  void DrawPoints(const utils::ConstArrayView<PointT>& points, const PointStyle& style) const {
    return primitive_painter_->DrawPoints(points, style);
  }
  template <typename T>
  void DrawPoints(const ConstArrayView<T>& points, const ConstArrayView<T>& colors,
                  float pt_size) const {
    return primitive_painter_->DrawPoints(points, colors, pt_size);
  }
  template <typename PointT>
  void DrawLines(const utils::ConstArrayView<PointT>& points, GLenum mode,
                 const LineStyle& style) const {
    return primitive_painter_->DrawLines(points, mode, style);
  }
  template <typename PointT>
  void DrawPlane(const utils::ConstArrayView<PointT>& vertices, const PlaneStyle& style) const {
    return primitive_painter_->DrawPlane(vertices, style);
  }
  template <typename PointT>
  void DrawSurface(const utils::ConstArrayView<PointT>& points, const SurfaceStyle& style) const {
    return primitive_painter_->DrawSurface(points, style);
  }
  template <typename Point2DT>
  void DrawPrism(const utils::ConstArrayView<Point2DT>& polygon, double ceiling, double floor,
                 const PrismStyle& style) const {
    return primitive_painter_->DrawPrism(polygon, ceiling, floor, style);
  }

  template <typename PointT>
  void DrawArrow(const PointT& start, const PointT& end, const ArrowStyle<PointT>& style) const;

  template <typename T>
  void DrawAxis(const glm::tmat4x4<T>& axises, const AxisStyle<math::Vec3<T>>& style) const;

  int target_width() const { return width_; }
  int target_height() const { return height_; }

  void SetupOpenGL();

  void OnTargetResize(int width, int height) {
    DCHECK_GE(width, 0);
    DCHECK_GE(height, 0);
    width_ = width;
    height_ = height;
  }

 protected:
  ::gl::Context* gl_context_ = nullptr;
  int width_ = 0;
  int height_ = 0;

  std::unique_ptr<PrimitivePainter> primitive_painter_;
  FontRenderer* font_renderer_ = nullptr;  // Not owned.
};

}  // namespace display
}  // namespace utils
