// Copyright @2019 Pony AI Inc. All rights reserved.

#pragma once

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <QtWidgets/QApplication>

#include "common/utils/display/painter_widget_base.h"
#include "common/utils/file/file.h"
#include "common/utils/file/path.h"
#include "common/utils/strings/format.h"
#include "homework3/obstacle.h"
#include "homework3/single_frame_detector.h"

class PointCloudViewer : public utils::display::PainterWidgetBase {
 public:
  using PointCloudLabel = Obstacle;

  using Options = utils::display::PainterWidgetBase::Options;

  PointCloudViewer(Options options, QWidget* parent, const std::string& data_dir,
      SingleFrameDetector* detector);

  ~PointCloudViewer() override = default;

 protected:
  // Qt event handlers.
  void timerEvent(QTimerEvent* /*event*/) override {
    update();
  }

  utils::display::OpenglPainter* gl_painter() override {
    return gl_painter_.get();
  }

  void Initialize() override {};

  void InitializeGlPainter() override;

  void keyPressEvent(QKeyEvent* event) override;

  void Paint3D() override;

 private:
  void DrawPointCloudLabel(const PointCloudLabel& label);

  std::string data_dir_;
  std::vector<std::string> pointcloud_files_;
  int file_index_ = -1;
  std::vector<math::Vec3d> points_;
  std::vector<math::Vec3d> ground_points_;
  std::vector<PointCloudLabel> labels_;

  std::unique_ptr<utils::display::OpenglPainter> gl_painter_;
  utils::display::OpenglPainter::SurfaceStyle default_prism_style_;
  utils::display::OpenglPainter::PointStyle default_point_style_;
  utils::display::OpenglPainter::PointStyle ground_point_style_;

  SingleFrameDetector* detector_;

  DISALLOW_COPY_MOVE_AND_ASSIGN(PointCloudViewer);
};
