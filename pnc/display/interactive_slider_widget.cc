// Copyright @2018 Pony AI Inc. All rights reserved.

#include "pnc/display/interactive_slider_widget.h"

#include "gflags/gflags.h"

#include "common/utils/file/file.h"
#include "common/utils/file/path.h"
#include "common/utils/strings/utils.h"

namespace utils {
namespace display {

InteractiveSliderWidget::InteractiveSliderWidget(Options options, QWidget* parent)
    : utils::display::FlatSliderWidget(options, parent) {}

void InteractiveSliderWidget::mouseDoubleClickEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    QPoint pos = event->pos();
    const double ratio = static_cast<double>(pos.x()) / width();
    emit SeekToTime(options_.min_value + ratio * (options_.max_value - options_.min_value));
  }
}

void InteractiveSliderWidget::paintEvent(QPaintEvent* event) {
  FlatSliderWidget::paintEvent(event);
}

}  // namespace display
}  // namespace utils
