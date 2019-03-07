// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <glm/glm.hpp>

#include "common/utils/common/defines.h"
#include "common/utils/display/camera.h"
#include "common/utils/math/vec3d.h"

namespace utils {
namespace display {

enum class View {
  kUnrestricted = 0,
  kTop = 1,
  kHorizontal = 2,
};
std::string ToString(View view);

enum class MouseStatus {
  kNone,
  kRotate,
  kMove,
  kPickPoint,
};

struct MouseLocation {
  int pos_x = 0;
  int pos_y = 0;
  MouseLocation() = default;
  MouseLocation(int x, int y) : pos_x(x), pos_y(y) {}
};

enum class ScreenCoordSystem {
  OpenGl = 0,  // X axis right-wards, Y axis up-wards
  Qt = 1       // X axis right-wards, Y axis down-wards
};

enum class CameraView { kPerspective = 0, kOrthographic = 1 };

class MouseController {
 public:
  MouseController() = default;
  ~MouseController() = default;
  const MouseLocation& mouse_location() const { return mouse_location_; }
  MouseStatus status() const { return mouse_status_; }

  void UpdateStatus(MouseStatus status);
  void UpdateMouseLocation(int x, int y);

 private:
  MouseStatus mouse_status_ = MouseStatus::kNone;
  MouseLocation mouse_location_;
  DISALLOW_COPY_AND_ASSIGN(MouseController);
};

class CameraController {
 public:
  explicit CameraController(ScreenCoordSystem screen_coord_sys)
      : CameraController(screen_coord_sys, CameraView::kPerspective) {}
  explicit CameraController(std::unique_ptr<Camera> camera);
  CameraController(ScreenCoordSystem screen_coord_sys, const CameraView view);

  ~CameraController() = default;

  void Initialize(double center_x, double center_y, double center_z, double altitude_angle,
                  double azimuth_angle, double distance);

  std::unique_ptr<Camera> SwitchCamera(std::unique_ptr<Camera> new_camera);

  // Update Interfaces.
  void UpdateCamera();
  void MarkAsReference();
  void UpdateEye(double altitude_angle, double azimuth_angle, double distance);
  void UpdateCenter(double x, double y, double z);
  void UpdateDistance(double distance) { camera_->set_distance(distance); }
  void RotateAroundReference(double dx, double dy);
  // In video technology, panning refers to the horizontal scrolling of an image wider than the
  // display.
  void Panning(double dx, double dy);
  void ResizeWidget(double widget_width, double widget_height);
  void ZoomIn();
  void ZoomOut();
  // Resets altitude/zimuth angles, but does NOT reset camera distance (i.e. zooming level).
  void ResetOrientation();
  void set_panning_ratio(double panning_ratio) { panning_ratio_ = panning_ratio; }

  // The position_x/position_y on screen will be normlized to [-1.0, 1.0].
  const math::Vec2d normalize_screen_position(const math::Vec2d& screen_pos) const {
    double norm_x = 2.0 * screen_pos.x / widget_width() - 1.0;
    double norm_y = 2.0 * screen_pos.y / widget_height() - 1.0;
    if (y_upwards_on_screen()) {
      norm_y = -norm_y;
    }
    return math::Vec2d(norm_x, norm_y);
  }

  math::Vec2d WorldDirectionToScreen(const math::Vec3d& world_vec) const {
    return camera_->WorldDirectionToScreen(world_vec);
  }
  math::Vec3d PositionOnZNearPlane(const math::Vec2d& screen_pos) const {
    return camera_->PositionOnZNearPlane(normalize_screen_position(screen_pos));
  }
  math::Vec2d PickPointOnXYPlane(const math::Vec2d& screen_pos) const {
    return camera_->PickPointOnXYPlane(normalize_screen_position(screen_pos));
  }
  math::Vec3d PickPointOnHorizontalPlane(const math::Vec2d& screen_pos, double z) const {
    const math::Vec2d norm_screen_pos = normalize_screen_position(screen_pos);
    return camera_->PickPointOnHorizontalPlane(norm_screen_pos, z);
  }
  math::Ray3d SightLine(const math::Vec2d& screen_pos) const {
    return camera_->SightLine(normalize_screen_position(screen_pos));
  }

  // Access Interfaces.
  const math::Vec3d& eye() const { return camera_->eye(); }
  const math::Vec3d& center() const { return camera_->center(); }
  const math::Vec3d& up_direction() const { return camera_->up_direction(); }
  double altitude_angle() const { return camera_->params().altitude_angle(); }
  double azimuth_angle() const { return camera_->params().azimuth_angle(); }
  double distance() const { return camera_->params().distance(); }
  double widget_width() const { return widget_width_; }
  double widget_height() const { return widget_height_; }
  bool y_upwards_on_screen() const { return y_upwards_on_screen_; }
  glm::mat4 view_matrix() const { return camera_->view_matrix(); }

 private:
  utils::display::Camera* camera() { return camera_.get(); }

  std::unique_ptr<utils::display::Camera> camera_;
  std::unique_ptr<utils::display::Camera> camera_reference_;
  double widget_width_ = 1.0;
  double widget_height_ = 1.0;
  bool y_upwards_on_screen_ = false;
  double panning_ratio_ = 1.0;

  friend class ViewController;

  DISALLOW_COPY_AND_ASSIGN(CameraController);
};

class ViewController {
 public:
  explicit ViewController(CameraController* camera_controller);
  virtual ~ViewController() = default;

  View view() const { return view_; }
  void SwitchToUnrestrictedView();
  void SwitchToTopView();
  void SwitchToHorizontalView(double z);

 private:
  CameraController* camera_controller_ = nullptr;  // Not owned.
  View view_ = View::kUnrestricted;
  std::unique_ptr<utils::display::Camera> unrestricted_view_camera_;
  std::unique_ptr<utils::display::Camera> top_view_camera_;
  std::unique_ptr<utils::display::Camera> horizontal_view_camera_;
  DISALLOW_COPY_AND_ASSIGN(ViewController);
};

}  // namespace display
}  // namespace utils
