// Copyright @2018 Pony AI Inc. All rights reserved.

#include "pnc/display/q_variable_dock_widget.h"

#include <QtCore/QSettings>
#include <algorithm>
#include <utility>
#include <vector>

#include "glog/logging.h"

#include "common/utils/math/vec2d.h"

namespace utils {
namespace display {

using std::string;
using interface::display::VariableView;

namespace {

class QTreeViewWithSizeHint : public QTreeView {
 public:
  QTreeViewWithSizeHint(QWidget* parent, const QSize& size_hint)
      : QTreeView(parent), size_hint_(size_hint) {}
  QSize sizeHint() const override { return size_hint_; }

 private:
  const QSize size_hint_;
};

}  // namespace

QVariableDockWidget::QVariableDockWidget(QWidget* parent) : QDockWidget(tr("Variables"), parent) {
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
  tree_model_->setColumnCount(2);
  tree_model_->setHorizontalHeaderLabels({QString("Variable"), QString("Value")});
  tree_view_->setColumnWidth(0, 200);
  connect(tree_view_, SIGNAL(collapsed(const QModelIndex&)), this,
          SLOT(VariablesCollapsed(const QModelIndex&)));
  connect(tree_view_, SIGNAL(expanded(const QModelIndex&)), this,
          SLOT(VariablesExpanded(const QModelIndex&)));
  layout_->addWidget(tree_view_);
}

namespace {

QBrush ColorToQBrush(const interface::display::Color& color) {
  QColor qcolor;
  qcolor.setRedF(color.red());
  qcolor.setGreenF(color.green());
  qcolor.setBlueF(color.blue());
  return QBrush(qcolor);
}

constexpr char kVariableViewCollapsed[] = "variable_view_collapsed/";

}  // namespace

void QVariableDockWidget::UpdateVariables(
    const std::unordered_map<std::string, interface::display::VariableViewList>&
        module_name_to_variable_view_list) {
  using NameAndVariableView = std::pair<std::string, const interface::display::VariableView*>;
  std::vector<NameAndVariableView> sorted_name_and_variable_view;
  for (const auto& module_name_and_list : module_name_to_variable_view_list) {
    const std::string& module_name = module_name_and_list.first;
    for (const auto& name_and_variable_view : module_name_and_list.second.variable_view()) {
      sorted_name_and_variable_view.emplace_back(
          name_and_variable_view.name() + " (" + module_name + ")", &name_and_variable_view);
    }
  }
  std::sort(sorted_name_and_variable_view.begin(), sorted_name_and_variable_view.end(),
            [](const NameAndVariableView& p0, const NameAndVariableView& p1) -> bool {
              return p0.first < p1.first;
            });

  QStandardItem* root = tree_model_->invisibleRootItem();
  for (int row_id = 0; row_id < root->rowCount(); ++row_id) {
    QStandardItem* child = root->child(row_id);
    for (int k = 0; k < child->rowCount(); ++k) {
      child->child(k, 0)->setText(QString());
      child->child(k, 0)->setBackground(Qt::white);
      child->child(k, 1)->setText(QString());
      child->child(k, 1)->setBackground(Qt::white);
    }
  }
  int first_layer_row_id = 0;
  for (const auto& variable_view : sorted_name_and_variable_view) {
    while (first_layer_row_id < root->rowCount() &&
           root->child(first_layer_row_id)->text().toStdString() < variable_view.first) {
      ++first_layer_row_id;
    }
    if (first_layer_row_id == root->rowCount() ||
        root->child(first_layer_row_id)->text().toStdString() != variable_view.first) {
      QStandardItem* new_child = new QStandardItem(QString::fromStdString(variable_view.first));
      // root->insertRow takes the ownership of child
      root->insertRow(first_layer_row_id, new_child);
      QSettings qsettings;
      if (!qsettings.value(kVariableViewCollapsed + QString::fromStdString(variable_view.first))
               .toBool()) {
        tree_view_->expand(new_child->index());
      }
    }
    QStandardItem* child = root->child(first_layer_row_id);
    for (int k = 0; k < child->rowCount() && k < variable_view.second->variable_size(); ++k) {
      const auto& variable = variable_view.second->variable(k);
      child->child(k, 0)->setText(QString::fromStdString(variable.name()));
      child->child(k, 0)->setBackground(ColorToQBrush(variable.background_color()));
      child->child(k, 0)->setForeground(ColorToQBrush(variable.foreground_color()));
      child->child(k, 1)->setText(QString::fromStdString(variable.value()));
      child->child(k, 1)->setBackground(ColorToQBrush(variable.background_color()));
      child->child(k, 1)->setForeground(ColorToQBrush(variable.foreground_color()));
    }
    while (child->rowCount() < variable_view.second->variable_size()) {
      const auto& variable = variable_view.second->variable(child->rowCount());
      QStandardItem* const name_item = new QStandardItem(QString::fromStdString(variable.name()));
      name_item->setBackground(ColorToQBrush(variable.background_color()));
      name_item->setForeground(ColorToQBrush(variable.foreground_color()));
      QStandardItem* const value_item = new QStandardItem(QString::fromStdString(variable.value()));
      value_item->setTextAlignment(Qt::AlignRight);
      value_item->setBackground(ColorToQBrush(variable.background_color()));
      value_item->setForeground(ColorToQBrush(variable.foreground_color()));
      // child->appendRow takes the ownership of name_item and value_item.
      child->appendRow(QList<QStandardItem*>{name_item, value_item});
    }
  }
}

void QVariableDockWidget::VariablesCollapsed(const QModelIndex& index) {
  QSettings qsettings;
  qsettings.setValue(
      kVariableViewCollapsed + tree_model_->invisibleRootItem()->child(index.row())->text(), true);
}

void QVariableDockWidget::VariablesExpanded(const QModelIndex& index) {
  QSettings qsettings;
  qsettings.setValue(
      kVariableViewCollapsed + tree_model_->invisibleRootItem()->child(index.row())->text(), false);
}

}  // namespace display
}  // namespace utils
