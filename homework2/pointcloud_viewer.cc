// Copyright @2019 Pony AI Inc. All rights reserved.

#include "homework3/pointcloud_viewer.h"

DEFINE_string(lidar_device, "VelodyneDevice32c", "");

PointCloudViewer::PointCloudViewer(Options options, QWidget* parent, const std::string& data_dir,
    SingleFrameDetector* detector)
    : PainterWidgetBase(options, parent), data_dir_(data_dir), detector_(detector) {
  CHECK(detector_ != nullptr);
  const std::string pointcloud_dir = file::path::Join(data_dir_, FLAGS_lidar_device);
  CHECK(file::path::Exists(pointcloud_dir)) << pointcloud_dir << " doesn't exist!";
  pointcloud_files_ = file::path::FindFilesWithPrefixSuffix(pointcloud_dir, "", "txt");
  std::sort(pointcloud_files_.begin(), pointcloud_files_.end(), file::path::Compare);

  const std::string pointcloud_label_dir =
      file::path::Join(data_dir_, "label", FLAGS_lidar_device);

  default_prism_style_.show_vertices = true;
  default_prism_style_.show_edge = true;
  default_prism_style_.show_plane = true;
  default_prism_style_.vertice_style.point_size = 4.0;
  default_prism_style_.edge_style.line_width = 2;
  default_prism_style_.plane_style.alpha = 0.2;
  default_prism_style_.set_color(utils::display::Color::Red());

  default_point_style_.point_size = 1.0;
  default_point_style_.point_color = utils::display::Color::Green();
  ground_point_style_.point_size = 1.0;
  ground_point_style_.point_color = utils::display::Color::Blue();

  startTimer(100);
}

void PointCloudViewer::InitializeGlPainter() {
  gl_painter_ = std::make_unique<utils::display::OpenglPainter>(gl_context(), font_renderer());
  gl_painter_->SetupOpenGL();
}

void PointCloudViewer::keyPressEvent(QKeyEvent* event) {
  const int key = event->key();
  switch (key) {
    case Qt::Key_N:
      file_index_ = (++file_index_) % pointcloud_files_.size();
      // Load pointcloud data.
      const std::string pointcloud_file = pointcloud_files_[file_index_];
      const PointCloud pointcloud_ = ReadPointCloudFromTextFile(pointcloud_file);
      CHECK(!pointcloud_.points.empty());
      LOG(INFO) << "Load pointcloud: " << pointcloud_file;
      points_.clear();
      points_.reserve(pointcloud_.points.size());
      for (const auto& point : pointcloud_.points) {
        Eigen::Vector3d point_in_world = pointcloud_.rotation * point + pointcloud_.translation;
        points_.emplace_back(point_in_world.x(), point_in_world.y(), point_in_world.z());
      }

      // Load object label if there is a corresponding one.
      labels_.clear();
      ground_points_.clear();
      std::vector<Eigen::Vector3d> eigen_points;
      detector_->GetGroundAndObstacles(pointcloud_, &eigen_points, &labels_);
      for (const auto& point : eigen_points) {
        ground_points_.emplace_back(point.x(), point.y(), point.z());
      }
      // Update camera center.
      painter_widget_controller_->MutableCamera()->UpdateCenter(
          pointcloud_.translation.x(),
          pointcloud_.translation.y(),
          pointcloud_.translation.z());
      break;
  }
}

void PointCloudViewer::Paint3D() {
  gl_painter()->DrawPoints<math::Vec3d>(
      utils::ConstArrayView<math::Vec3d>(ground_points_.data(), ground_points_.size()),
      ground_point_style_);
  gl_painter()->DrawPoints<math::Vec3d>(
      utils::ConstArrayView<math::Vec3d>(points_.data(), points_.size()), default_point_style_);
  if (!labels_.empty()) {
    for (const auto& label : labels_) {
      DrawPointCloudLabel(label);
    }
  }
}

void PointCloudViewer::DrawPointCloudLabel(const PointCloudLabel& label) {
  font_renderer()->DrawText3D(
      label.id, math::Vec3d(label.polygon[0].x, label.polygon[0].y, label.ceiling + 0.25),
      utils::display::Color::Yellow(), "Arial", 12);

  gl_painter()->DrawPrism<math::Vec2d>(
      utils::ConstArrayView<math::Vec2d>(label.polygon.data(), label.polygon.size()),
      label.ceiling, label.floor, default_prism_style_);
}
