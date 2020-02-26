// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <string>
#include <utility>
#include <vector>

#include "pnc/display/flat_slider_widget.h"

namespace utils {
namespace display {

class InteractiveSliderWidget : public utils::display::FlatSliderWidget {
  Q_OBJECT

 public:
  using Options = utils::display::FlatSliderWidget::Options;
  InteractiveSliderWidget(Options options, QWidget* parent);
  ~InteractiveSliderWidget() override = default;

  void mouseDoubleClickEvent(QMouseEvent* event) override;

 signals:
  void SeekToTime(double timestamp);

 private:
  void paintEvent(QPaintEvent* event) override;
};

}  // namespace display
}  // namespace utils
