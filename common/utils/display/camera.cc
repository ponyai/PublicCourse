// Copyright @2018 Pony AI Inc. All rights reserved.

#include "common/utils/display/camera.h"

#include <GL/glu.h>

namespace utils {
namespace display {

void Camera::CameraParams::set_altitude_angle(double angle) {
  angle = math::Clamp(angle, 0.0, M_PI * 0.5);
  altitude_angle_ = std::min(M_PI * 0.5 - kError, angle);
}

void Camera::CameraParams::set_azimuth_angle(double angle) { azimuth_angle_ = angle; }

void Camera::CameraParams::set_distance(double distance) { distance_ = distance; }

void Camera::ComputeAxisDirections(math::Vec3d* view_dir, math::Vec3d* x_dir,
                                   math::Vec3d* y_dir) const {
  CHECK(view_dir != nullptr && x_dir != nullptr && y_dir != nullptr);

  *view_dir = center() - eye();
  view_dir->Normalize();

  math::Vec3d up_dir = up_direction();
  up_dir.Normalize();

  *x_dir = view_dir->OuterProd(up_dir);
  x_dir->Normalize();

  if (y_upwards_on_screen()) {
    *y_dir = x_dir->OuterProd(*view_dir);
  } else {
    *y_dir = view_dir->OuterProd(*x_dir);
  }
  y_dir->Normalize();
}

math::Vec3d Camera::PositionOnZNearPlane(const math::Vec2d& norm_screen_pos) const {
  math::Vec3d view_dir, x_dir, y_dir;
  ComputeAxisDirections(&view_dir, &x_dir, &y_dir);

  // Compute the world coordinate of z_near plane center.
  math::Vec3d result = eye() + view_dir * z_near_;

  double screen_x = norm_screen_pos.x;
  double screen_y = norm_screen_pos.y;
  screen_x *= z_near_plane_width() * 0.5;
  screen_y *= z_near_plane_height() * 0.5;
  result += x_dir * screen_x + y_dir * screen_y;
  return result;
}

math::Vec2d Camera::PickPointOnXYPlane(const math::Vec2d& norm_screen_pos) const {
  return PickPointOnHorizontalPlane(norm_screen_pos, 0.0).xy();
}

math::Vec2d Camera::WorldDirectionToScreen(const math::Vec3d& world_vec) const {
  math::Vec3d view_dir, x_dir, y_dir;
  ComputeAxisDirections(&view_dir, &x_dir, &y_dir);

  math::Vec2d result;
  result.x = world_vec.InnerProd(x_dir);
  result.y = world_vec.InnerProd(y_dir);
  result.Normalize();
  return result;
}

void Camera::Setup() {
  const auto& p = params_;
  eye_.x = center_.x + p.distance() * std::cos(p.azimuth_angle()) * std::cos(p.altitude_angle());
  eye_.y = center_.y + p.distance() * std::sin(p.azimuth_angle()) * std::cos(p.altitude_angle());
  eye_.z = center_.z + p.distance() * std::sin(p.altitude_angle());
}

void Camera::set_center(double x, double y, double z) {
  center_.x = x;
  center_.y = y;
  center_.z = z;
}

void Camera::set_params(double altitude_angle, double azimuth_angle, double distance) {
  params_.set_altitude_angle(altitude_angle);
  params_.set_azimuth_angle(azimuth_angle);
  params_.set_distance(distance);
}

void PerspectiveViewCamera::UpdateWidgetSize(double width, double height) {
  aspect_ratio_ = width / height;
}

void PerspectiveViewCamera::Setup() {
  Camera::Setup();

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluPerspective(fov_y_deg_, aspect_ratio_, z_near_, z_far_);

  glMatrixMode(GL_MODELVIEW);
}

math::Vec3d PerspectiveViewCamera::PickPointOnHorizontalPlane(const math::Vec2d& norm_screen_pos,
                                                              double z) const {
  math::Vec3d picked_point_on_z_near = PositionOnZNearPlane(norm_screen_pos);
  math::Vec3d dir = picked_point_on_z_near - eye();

  if (std::abs(dir.z) < 1e-6) {
    double nan = std::numeric_limits<double>::quiet_NaN();
    return math::Vec3d(nan, nan, nan);
  }

  double coef = -(eye().z - z) / dir.z;
  return math::Vec3d(eye().x + coef * dir.x, eye().y + coef * dir.y, eye().z + coef * dir.z);
}

math::Ray3d PerspectiveViewCamera::SightLine(const math::Vec2d& norm_screen_pos) const {
  math::Vec3d starting_point = PositionOnZNearPlane(norm_screen_pos);
  math::Vec3d direction = starting_point - eye_;
  direction.Normalize();
  return math::Ray3d(starting_point, direction);
}

void OrthographicViewCamera::UpdateWidgetSize(double widget_width, double widget_height) {
  widget_width_ = widget_width;
  widget_height_ = widget_height;
}

void OrthographicViewCamera::Setup() {
  Camera::Setup();

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  const double half_width = z_near_plane_width() * 0.5;
  const double half_height = z_near_plane_height() * 0.5;
  glOrtho(-half_width, half_width, -half_height, half_height, z_near_, z_far_);

  glMatrixMode(GL_MODELVIEW);
}

math::Vec3d OrthographicViewCamera::PickPointOnHorizontalPlane(const math::Vec2d& norm_screen_pos,
                                                               double z) const {
  const math::Vec3d dir = center() - eye();
  if (std::abs(dir.z) < 1e-6) {
    double nan = std::numeric_limits<double>::quiet_NaN();
    return math::Vec3d(nan, nan, nan);
  }

  math::Vec3d picked_point_on_z_near = PositionOnZNearPlane(norm_screen_pos);

  double coef = -(picked_point_on_z_near.z - z) / dir.z;
  return math::Vec3d(picked_point_on_z_near.x + coef * dir.x,
                     picked_point_on_z_near.y + coef * dir.y,
                     picked_point_on_z_near.z + coef * dir.z);
}

math::Ray3d OrthographicViewCamera::SightLine(const math::Vec2d& norm_screen_pos) const {
  math::Vec3d starting_point = PositionOnZNearPlane(norm_screen_pos);
  math::Vec3d direction = center_ - eye_;
  direction.Normalize();
  return math::Ray3d(starting_point, direction);
}

void OrthographicViewCamera::set_distance(double distance) {
  // Calculate scale according to view field in perspective view with 'distance'
  // and 'fov_y_dag'
  const double fov_y_deg = 50.0;
  const double z_near_plane_height = distance * std::tan(math::DegreeToRadian(fov_y_deg * 0.5)) * 2;
  scale_ = z_near_plane_height / widget_width_;
}

}  // namespace display
}  // namespace utils
