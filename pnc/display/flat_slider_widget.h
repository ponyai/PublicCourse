// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include "QtCore/QtCore"
#include "QtGui/QtGui"
#include "QtWidgets/QWidget"

#include "common/utils/display/camera.h"
#include "common/utils/display/qt_macro.h"

namespace utils {
namespace display {

class FlatSliderWidget : public QWidget {
  Q_OBJECT
 public:
  struct Options {
    int height = 5;
    double min_value = 0.0;
    double max_value = 100.0;
  };

  FlatSliderWidget(Options options, QWidget* parent);
  ~FlatSliderWidget() override = default;

  void SetValue(double value);

 protected:
  void paintEvent(QPaintEvent* event) override;
  Options options_;
  double value_ = 0.0;
};

}  // namespace display
}  // namespace utils
