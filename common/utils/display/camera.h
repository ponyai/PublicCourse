// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <cmath>
#include <limits>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glog/logging.h>

#include "common/utils/math/ray.h"
#include "common/utils/math/vec2d.h"
#include "common/utils/math/vec3d.h"

namespace utils {
namespace display {

// * Camera is a point at 'eye'.
// * Camera screen can be considered as a plane which is perpendicular to the line connecting
//   'eye' and 'center'. Its y direction is the projection of 'up_direction' on this plane.
// * There are two 'clip planes', which are parallel to the camera screen. Only points lying between
//   these two planes are rendered. Their centers are at (eye + view_dir * z_near), and
//   (eye + view_dir * z_far), where 'view_dir' is normalize(center - eye), which is the *negative*
//   z axis in camera screen coordinate system.
//
// As of now, in our UI, we actually keep 'up_direction' to be (0, 0, 1). This is
// fine for most of the time. However, when screen plane is perpendicular to this direction (e.g.
// watching the z=0 plane downwards or upwards), the camera model will be broken.
class Camera {
 public:
  class CameraParams {
   public:
    static constexpr double kError = 0.001;
    double altitude_angle() const { return altitude_angle_; }
    double azimuth_angle() const { return azimuth_angle_; }
    double distance() const { return distance_; }
    void set_altitude_angle(double angle);
    void set_azimuth_angle(double angle);
    void set_distance(double distance);

   private:
    // Angle between eye sight and xy plane.
    double altitude_angle_ = 45.0 / 180.0 * M_PI;
    // Angle on the xy plane.
    double azimuth_angle_ = 0.0;
    // Distance between eye and screen center.
    double distance_ = 30.0;
  };

  explicit Camera(bool y_upwards) : y_upwards_on_screen_(y_upwards) {}
  virtual ~Camera() = default;

  virtual double z_near_plane_width() const = 0;
  virtual double z_near_plane_height() const = 0;

  virtual void Setup();
  virtual void UpdateWidgetSize(double widget_width, double widget_height) = 0;
  virtual void Zoom(double scale) = 0;

  // 'view_dir' is actually the *negative* z axis of the camera screen coordinate system.
  void ComputeAxisDirections(math::Vec3d* view_dir, math::Vec3d* x_dir, math::Vec3d* y_dir) const;
  // Given the screen coordinates, returns the 2D coordinates on the near clipping plane.
  // Screen coordinates are normalized coordinates. Screen center is the origin. Screen border
  // is screen_{x,y} = -1.0 or 1.0.
  math::Vec3d PositionOnZNearPlane(const math::Vec2d& norm_screen_pos) const;
  // Given the screen coordinates, returns the 3D coordinates on horizontal plane.
  // Screen coordinates are normalized coordinates. Screen center is the origin. Screen border
  // is screen_{x,y} = -1.0 or 1.0.
  virtual math::Vec3d PickPointOnHorizontalPlane(const math::Vec2d& norm_screen_pos,
                                                 double z) const = 0;
  // Given the screen coordinates, return sight line whose starting point is mouse position on
  // z-near plane, whose direction has been normalized. Screen border is screen_{x,y} = -1.0 or
  // 1.0.
  virtual math::Ray3d SightLine(const math::Vec2d& norm_screen_pos) const = 0;
  // Given the screen coordinates, returns the 2D coordinates on xy (i.e. z=0) plane.
  // Screen coordinates are normalized coordinates. Screen center is the origin. Screen border
  // is screen_{x,y} = -1.0 or 1.0.
  math::Vec2d PickPointOnXYPlane(const math::Vec2d& norm_screen_pos) const;
  // Converts a 3D direction vector in world coordinate system to a 2D direction vector in the
  // camera screen coordinate system. Input is not required to be normalized, but the output will
  // be normalized.
  math::Vec2d WorldDirectionToScreen(const math::Vec3d& world_vec) const;

  const CameraParams& params() const { return params_; }
  virtual void set_params(double altitude_angle, double azimuth_angle, double distance);

  const math::Vec3d& eye() const { return eye_; }
  const math::Vec3d& center() const { return center_; }
  void set_center(double x, double y, double z);
  const math::Vec3d& up_direction() const { return up_direction_; }
  bool y_upwards_on_screen() const { return y_upwards_on_screen_; }
  virtual void set_distance(double distance) = 0;
  glm::mat4 view_matrix() const {
    return glm::lookAt(glm::vec3(eye_.x, eye_.y, eye_.z),
                       glm::vec3(center_.x, center_.y, center_.z),
                       glm::vec3(up_direction_.x, up_direction_.y, up_direction_.z));
  }

 protected:
  // World coordinate of camera position.
  math::Vec3d eye_;
  // World coordinate of screen center.
  math::Vec3d center_;
  // Sets up the 'y' direction on screen. The actual y direction is the result of projecting this
  // direction onto the screen plane.
  math::Vec3d up_direction_ = math::Vec3d(0.0, 0.0, 1.0);
  CameraParams params_;

  // The distance from camera to the two clipping planes.
  double z_near_ = 0.25;
  double z_far_ = 15000.0;

  // when y_upwards_on_screen was set true, 'camera screen coordinate system' is the camera
  // coordinate system of OpenGL, otherwise it will be coordinate system of Qt. While both
  // coordinate systems have the same X direction.
  bool y_upwards_on_screen_ = true;
};

// OpenGL camera model of gluLookAt() and gluPerspective():
class PerspectiveViewCamera : public Camera {
 public:
  explicit PerspectiveViewCamera(bool y_upwards_on_screen) : Camera(y_upwards_on_screen) {}

  void Setup() final;
  void UpdateWidgetSize(double widget_width, double widget_height) final;
  void Zoom(double scale) final { params_.set_distance(params_.distance() * scale); }
  math::Vec3d PickPointOnHorizontalPlane(const math::Vec2d& norm_screen_pos, double z) const final;
  math::Ray3d SightLine(const math::Vec2d& norm_screen_pos) const final;

  double z_near_plane_height() const final {
    return z_near_ * std::tan(math::DegreeToRadian(fov_y_deg_ * 0.5)) * 2;
  }
  double z_near_plane_width() const final { return z_near_plane_height() * aspect_ratio_; }
  double fov_y_deg() const { return fov_y_deg_; }
  double aspect_ratio() const { return aspect_ratio_; }
  void set_distance(double distance) override { params_.set_distance(distance); };

 private:
  // Field of view on y axis, in degrees.
  double fov_y_deg_ = 50.0;
  // screen_width / screen_height.
  double aspect_ratio_ = 1.0;
};

// OpenGL camera model of gluLookAt() and glOrtho():
class OrthographicViewCamera : public Camera {
 public:
  explicit OrthographicViewCamera(bool y_upwards_on_screen) : Camera(y_upwards_on_screen) {}

  void Setup() final;
  void UpdateWidgetSize(double widget_width, double widget_height) final;
  void Zoom(double scale) final { scale_ *= scale; }
  math::Vec3d PickPointOnHorizontalPlane(const math::Vec2d& norm_screen_pos, double z) const final;
  math::Ray3d SightLine(const math::Vec2d& norm_screen_pos) const final;

  double widget_width() const { return widget_width_; }
  double widget_height() const { return widget_height_; }
  double z_near_plane_width() const final { return widget_width_ * scale_; }
  double z_near_plane_height() const final { return widget_height_ * scale_; }
  double scale() const { return scale_; }
  void set_scale(double scale) { scale_ = scale; }
  void set_distance(double distance) override;

 private:
  double widget_width_ = 1920.0;
  double widget_height_ = 1080.0;
  double scale_ = 0.1;
};

class TopOrthographicViewCamera : public OrthographicViewCamera {
 public:
  explicit TopOrthographicViewCamera(bool y_upwards_on_screen)
      : OrthographicViewCamera(y_upwards_on_screen) {
    params_.set_altitude_angle(M_PI * 0.5);
  }

  void set_params(double /*altitude_angle*/, double azimuth_angle, double distance) final {
    params_.set_azimuth_angle(azimuth_angle);
    params_.set_distance(distance);
  }
};

class HorizontalOrthographicViewCamera : public OrthographicViewCamera {
 public:
  explicit HorizontalOrthographicViewCamera(bool y_upwards_on_screen)
      : OrthographicViewCamera(y_upwards_on_screen) {
    params_.set_altitude_angle(0.0);
  }

  void set_params(double /*altitude_angle*/, double azimuth_angle, double distance) final {
    params_.set_azimuth_angle(azimuth_angle);
    params_.set_distance(distance);
  }
};

}  // namespace display
}  // namespace utils
