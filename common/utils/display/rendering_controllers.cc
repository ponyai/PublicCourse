// Copyright @2018 Pony AI Inc. All rights reserved.

#include "common/utils/display/rendering_controllers.h"

namespace utils {
namespace display {

std::string ToString(View view) {
  switch (view) {
    case View::kUnrestricted:
      return "Unrestricted View.";
    case View::kTop:
      return "Top View.";
    case View::kHorizontal:
      return "Horizontal View.";
    default:
      return "View should not exist.";
  }
}

void MouseController::UpdateStatus(MouseStatus status) { mouse_status_ = status; }

void MouseController::UpdateMouseLocation(int x, int y) {
  mouse_location_.pos_x = x;
  mouse_location_.pos_y = y;
}

CameraController::CameraController(std::unique_ptr<Camera> camera) {
  camera_ = std::move(camera);

  if (typeid(camera_.get()) == typeid(PerspectiveViewCamera*)) {
    camera_reference_ = std::make_unique<PerspectiveViewCamera>(camera_->y_upwards_on_screen());
  }
  if (typeid(camera_.get()) == typeid(OrthographicViewCamera*)) {
    camera_reference_ = std::make_unique<OrthographicViewCamera>(camera_->y_upwards_on_screen());
  }

  CHECK_NOTNULL(camera_.get());
  CHECK_NOTNULL(camera_reference_.get());
}

CameraController::CameraController(ScreenCoordSystem screen_coord_sys, const CameraView view) {
  y_upwards_on_screen_ = (screen_coord_sys == ScreenCoordSystem::OpenGl);

  switch (view) {
    case CameraView::kPerspective:
      camera_ = std::make_unique<PerspectiveViewCamera>(y_upwards_on_screen_);
      camera_reference_ = std::make_unique<PerspectiveViewCamera>(y_upwards_on_screen_);
      break;
    case CameraView::kOrthographic:
      camera_ = std::make_unique<OrthographicViewCamera>(y_upwards_on_screen_);
      camera_reference_ = std::make_unique<OrthographicViewCamera>(y_upwards_on_screen_);
      break;
  }

  CHECK_NOTNULL(camera_.get());
  CHECK_NOTNULL(camera_reference_.get());
}

std::unique_ptr<Camera> CameraController::SwitchCamera(std::unique_ptr<Camera> new_camera) {
  CHECK_NOTNULL(new_camera.get());

  std::unique_ptr<Camera> old_camera = std::move(camera_);
  camera_ = std::move(new_camera);

  if (typeid(camera_.get()) == typeid(PerspectiveViewCamera*)) {
    camera_reference_ = std::make_unique<PerspectiveViewCamera>(camera_->y_upwards_on_screen());
  }
  if (typeid(camera_.get()) == typeid(OrthographicViewCamera*)) {
    camera_reference_ = std::make_unique<OrthographicViewCamera>(camera_->y_upwards_on_screen());
  }

  camera_->UpdateWidgetSize(widget_width_, widget_height_);
  return old_camera;
}

void CameraController::Initialize(double center_x, double center_y, double center_z,
                                  double altitude_angle, double azimuth_angle, double distance) {
  UpdateCenter(center_x, center_y, center_z);
  UpdateEye(altitude_angle, azimuth_angle, distance);
}

void CameraController::UpdateCamera() { camera_->Setup(); }

void CameraController::MarkAsReference() { *camera_reference_ = *camera_; }

void CameraController::RotateAroundReference(double dx, double dy) {
  double altitude_angle = camera_reference_->params().altitude_angle() + dy * 0.001;
  double azimuth_angle = camera_reference_->params().azimuth_angle() + dx * 0.003;
  const auto& params = camera_->params();
  camera_->set_params(altitude_angle, azimuth_angle, params.distance());
}

void CameraController::Panning(double dx, double dy) {
  const auto& camera_params_reference = camera_reference_->params();
  double cos_value = std::cos(camera_params_reference.azimuth_angle());
  double sin_value = std::sin(camera_params_reference.azimuth_angle());
  double scale = std::max(0.01, camera_->params().distance() / 300.0) * panning_ratio_;

  double x = camera_reference_->center().x + (dx * sin_value - dy * cos_value) * scale;
  double y = camera_reference_->center().y - (dx * cos_value + dy * sin_value) * scale;
  UpdateCenter(x, y, camera_->center().z);
}

void CameraController::ZoomIn() { camera_->Zoom(1 / 1.1); }

void CameraController::ZoomOut() { camera_->Zoom(1.1); }

void CameraController::UpdateEye(double altitude_angle, double azimuth_angle, double distance) {
  camera_->set_params(altitude_angle, azimuth_angle, distance);
}

void CameraController::UpdateCenter(double x, double y, double z) { camera_->set_center(x, y, z); }

void CameraController::ResizeWidget(double widget_width, double widget_height) {
  widget_width_ = widget_width;
  widget_height_ = widget_height;

  camera_->UpdateWidgetSize(widget_width, widget_height);
}

void CameraController::ResetOrientation() {
  // Do not reset zooming level.
  camera_->set_params(Camera::CameraParams().altitude_angle(),
                      Camera::CameraParams().azimuth_angle(), camera_->params().distance());
}

ViewController::ViewController(CameraController* camera_controller)
    : camera_controller_(CHECK_NOTNULL(camera_controller)) {
  bool y_upwards_on_screen = camera_controller->y_upwards_on_screen();
  // unrestricted_camera_ has been in camera_controller.
  top_view_camera_ = std::make_unique<TopOrthographicViewCamera>(y_upwards_on_screen);
  horizontal_view_camera_ = std::make_unique<HorizontalOrthographicViewCamera>(y_upwards_on_screen);
}

void ViewController::SwitchToUnrestrictedView() {
  switch (view_) {
    case View::kTop:
      CHECK_NOTNULL(unrestricted_view_camera_.get());
      unrestricted_view_camera_->set_center(camera_controller_->center().x,
                                            camera_controller_->center().y,
                                            camera_controller_->center().z);
      // Top View's altitude angle will be inexplicable in Unrestricted View.
      unrestricted_view_camera_->set_params(unrestricted_view_camera_->params().altitude_angle(),
                                            camera_controller_->azimuth_angle(),
                                            camera_controller_->distance());
      top_view_camera_ = camera_controller_->SwitchCamera(std::move(unrestricted_view_camera_));
      break;

    case View::kHorizontal: {
      CHECK_NOTNULL(unrestricted_view_camera_.get());
      const double z_near_plane_width =
          static_cast<HorizontalOrthographicViewCamera*>(camera_controller_->camera())
              ->z_near_plane_width();
      const double fov_y_deg =
          static_cast<PerspectiveViewCamera*>(unrestricted_view_camera_.get())->fov_y_deg();
      const double radian = math::DegreeToRadian(fov_y_deg * 0.5);
      const double distance = z_near_plane_width / std::tan(radian) * 0.5;
      // Horizontal View's center_z will be inexplicable in Unrestricted View.
      unrestricted_view_camera_->set_center(camera_controller_->center().x,
                                            camera_controller_->center().y,
                                            unrestricted_view_camera_->center().z);
      // Horizontal View's altitude angle will be inexplicable in Unrestricted
      // View.
      unrestricted_view_camera_->set_params(unrestricted_view_camera_->params().altitude_angle(),
                                            camera_controller_->azimuth_angle(), distance);
      horizontal_view_camera_ =
          camera_controller_->SwitchCamera(std::move(unrestricted_view_camera_));
      break;
    }

    case View::kUnrestricted:
      break;
  }

  view_ = View::kUnrestricted;
}

void ViewController::SwitchToTopView() {
  switch (view_) {
    case View::kUnrestricted: {
      CHECK_NOTNULL(top_view_camera_.get());
      top_view_camera_->set_center(camera_controller_->center().x, camera_controller_->center().y,
                                   camera_controller_->center().z);
      top_view_camera_->set_params(M_PI * 0.5, camera_controller_->azimuth_angle(),
                                   camera_controller_->distance());
      const double fov_y_deg =
          static_cast<PerspectiveViewCamera*>(camera_controller_->camera())->fov_y_deg();
      const double current_view_width = camera_controller_->camera()->params().distance() *
                                        std::tan(math::DegreeToRadian(fov_y_deg * 0.5)) * 2.0;
      const double scale =
          current_view_width /
          static_cast<OrthographicViewCamera*>(top_view_camera_.get())->widget_width();
      static_cast<TopOrthographicViewCamera*>(top_view_camera_.get())->set_scale(scale);
      unrestricted_view_camera_ = camera_controller_->SwitchCamera(std::move(top_view_camera_));
      break;
    }

    case View::kHorizontal:
      CHECK_NOTNULL(top_view_camera_.get());
      // Horizontal View's center_z will be inexplicable in Top View.
      top_view_camera_->set_center(camera_controller_->center().x, camera_controller_->center().y,
                                   top_view_camera_->center().z);
      top_view_camera_->set_params(M_PI * 0.5, camera_controller_->azimuth_angle(),
                                   camera_controller_->distance());
      horizontal_view_camera_ = camera_controller_->SwitchCamera(std::move(top_view_camera_));
      break;

    case View::kTop:
      break;
  }

  view_ = View::kTop;
}

void ViewController::SwitchToHorizontalView(double z) {
  switch (view_) {
    case View::kUnrestricted:
      CHECK_NOTNULL(horizontal_view_camera_.get());
      horizontal_view_camera_->set_center(camera_controller_->center().x,
                                          camera_controller_->center().y, z);
      horizontal_view_camera_->set_params(0.0, camera_controller_->azimuth_angle(),
                                          camera_controller_->distance());
      unrestricted_view_camera_ =
          camera_controller_->SwitchCamera(std::move(horizontal_view_camera_));
      break;

    case View::kTop: {
      CHECK_NOTNULL(horizontal_view_camera_.get());
      horizontal_view_camera_->set_center(camera_controller_->center().x,
                                          camera_controller_->center().y, z);
      horizontal_view_camera_->set_params(0.0, camera_controller_->azimuth_angle(),
                                          camera_controller_->distance());
      auto* camera = static_cast<HorizontalOrthographicViewCamera*>(horizontal_view_camera_.get());
      // Keep scale consistence in top and horizontal view.
      double scale =
          static_cast<HorizontalOrthographicViewCamera*>(camera_controller_->camera())->scale();
      camera->set_scale(scale);
      top_view_camera_ = camera_controller_->SwitchCamera(std::move(horizontal_view_camera_));
      break;
    }

    case View::kHorizontal:
      break;
  }

  view_ = View::kHorizontal;
}

}  // namespace display
}  // namespace utils
