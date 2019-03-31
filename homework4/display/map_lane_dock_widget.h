// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <iomanip>
#include <memory>
#include <string>
#include <unordered_map>

#include "QtCore/QItemSelectionModel"
#include "QtGui/QStandardItemModel"
#include "QtWidgets/QDockWidget"
#include "QtWidgets/QGridLayout"
#include "QtWidgets/QTreeView"
#include "gflags/gflags.h"

#include "common/proto/display.pb.h"
#include "common/proto/map.pb.h"
#include "common/utils/display/qt_macro.h"

namespace utils {
namespace display {

class MapLaneDockWidget : public QDockWidget {
  Q_OBJECT

 public:
  explicit MapLaneDockWidget(QWidget* parent);
  ~MapLaneDockWidget() override = default;
  void Initialize(const interface::map::Map& map_data);

 signals:
  void HighlightLane(std::string id);

 public slots:
  void OnTreeViewClicked(const QModelIndex& index);

 private:
  QStandardItemModel* tree_model_ = nullptr;
  QTreeView* tree_view_ = nullptr;
  QVBoxLayout* layout_ = nullptr;
  QWidget* container_widget_ = nullptr;

  interface::map::Map map_data_;

  std::string selected_map_id_;
};

}  // namespace display
}  // namespace utils
