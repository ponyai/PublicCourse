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
#include "common/utils/display/qt_macro.h"

namespace utils {
namespace display {

class QVariableDockWidget : public QDockWidget {
  Q_OBJECT

 public:
  explicit QVariableDockWidget(QWidget* parent);
  ~QVariableDockWidget() override = default;

  void UpdateVariables(const std::unordered_map<std::string, interface::display::VariableViewList>&
                           module_name_to_variable_view_list);

 public slots:  // NOLINT(whitespace/indent)
  void VariablesCollapsed(const QModelIndex& index);
  void VariablesExpanded(const QModelIndex& index);

 private:
  QStandardItemModel* tree_model_ = nullptr;
  QTreeView* tree_view_ = nullptr;
  QVBoxLayout* layout_ = nullptr;
  QWidget* container_widget_ = nullptr;
};

}  // namespace display
}  // namespace utils
