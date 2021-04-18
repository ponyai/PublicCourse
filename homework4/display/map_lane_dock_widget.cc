// Copyright @2018 Pony AI Inc. All rights reserved.

#include "homework4/display/map_lane_dock_widget.h"

#include <QtCore/QSettings>
#include <algorithm>
#include <utility>
#include <vector>

#include "glog/logging.h"

#include "common/utils/math/vec2d.h"

namespace utils {
namespace display {

using std::string;

class QTreeViewWithSizeHint : public QTreeView {
 public:
  QTreeViewWithSizeHint(QWidget* parent, const QSize& size_hint)
      : QTreeView(parent), size_hint_(size_hint) {}
  QSize sizeHint() const override { return size_hint_; }

 private:
  const QSize size_hint_;
};

MapLaneDockWidget::MapLaneDockWidget(QWidget* parent) : QDockWidget(tr("Lanes"), parent) {
  setFeatures(DockWidgetMovable | DockWidgetFloatable);
  setFocusPolicy(Qt::NoFocus);

  // use a container widget to hold multiple widgets and manage layout
  layout_ = new QVBoxLayout();
  container_widget_ = new QWidget();
  container_widget_->setLayout(layout_);
  setWidget(container_widget_);

  // Set up sub-widgets
  tree_view_ = new QTreeViewWithSizeHint(this, QSize(300, -1));
  tree_view_->setFocusPolicy(Qt::NoFocus);
  tree_view_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  // setWidget takes the ownership.
  tree_model_ = new QStandardItemModel;
  tree_view_->setModel(tree_model_);
  // setModel takes the ownership.
  tree_model_->setColumnCount(1);
  tree_model_->setHorizontalHeaderLabels({QString("Id")});
  tree_view_->setColumnWidth(0, 200);
  layout_->addWidget(tree_view_);

  connect(tree_view_, SIGNAL(clicked(const QModelIndex&)), this,
          SLOT(OnTreeViewClicked(const QModelIndex&)));
}

QBrush ColorToQBrush(const interface::display::Color& color) {
  QColor qcolor;
  qcolor.setRedF(color.red());
  qcolor.setGreenF(color.green());
  qcolor.setBlueF(color.blue());
  return QBrush(qcolor);
}

void MapLaneDockWidget::Initialize(const interface::map::Map& map_data) {
  map_data_.CopyFrom(map_data);
  for (const interface::map::Lane& lane : map_data_.lane()) {
    QStandardItem* const id_item = new QStandardItem(QString::fromStdString(lane.id().id()));
    tree_model_->appendRow(id_item);
  }
}

void MapLaneDockWidget::OnTreeViewClicked(const QModelIndex& index) {
  selected_map_id_ = tree_model_->item(index.row())->text().toStdString();
  emit HighlightLane(selected_map_id_);
}

}  // namespace display
}  // namespace utils
