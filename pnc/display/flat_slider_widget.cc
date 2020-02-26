// Copyright @2018 Pony AI Inc. All rights reserved.

#include "pnc/display/flat_slider_widget.h"

#include <utility>

namespace utils {
namespace display {

FlatSliderWidget::FlatSliderWidget(Options options, QWidget* parent)
    : QWidget(parent), options_(std::move(options)) {
  setFixedHeight(options_.height);
  setFocusPolicy(Qt::NoFocus);
}

void FlatSliderWidget::SetValue(double value) { value_ = value; }

void FlatSliderWidget::paintEvent(QPaintEvent* /*event*/) {
  QPainter painter(this);
  painter.setBrush(QColor(0x29, 0x79, 0xFF));

  double ratio = (value_ - options_.min_value) / (options_.max_value - options_.min_value);
  ratio = math::Clamp(ratio, 0.0, 1.0);
  int w = static_cast<int>(width() * ratio);
  painter.drawRect(0, 0, w, height());
}

}  // namespace display
}  // namespace utils
