// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <glog/logging.h>
#include <memory>
#include <unordered_map>
#include <vector>

namespace utils {

// Structure presents 2D grid index.
// Each grid contains all points fall into [x, x + dimension) * [y, y + dimension).
template <class T>
class GridIndex {
 public:
  GridIndex(double min_x, double min_y, double max_x, double max_y, double dimension);

  double min_x() const { return min_x_; }
  double min_y() const { return min_y_; }
  double max_x() const { return max_x_; }
  double max_y() const { return max_y_; }
  double dimension() const { return dimension_; }
  int num_rows() const { return num_rows_; }
  int num_columns() const { return num_columns_; }

  const T* Get(double x, double y) const;

  T* GetMutable(double x, double y);

  std::vector<const T*> GetAll() const;

  std::vector<const T*> GetAllInRect(double min_x, double min_y, double max_x, double max_y) const;

  std::vector<T*> GetAllMutableInRect(double min_x, double min_y, double max_x, double max_y);

  std::vector<const T*> GetAllInCircle(double cx, double cy, double radius) const;

  std::vector<T*> GetAllMutableInCircle(double cx, double cy, double radius);

 private:
  int64_t Pack2D(int x, int y) const;

  int64_t GetIndex(double x, double y) const;

  const T* GetByIndex(int64_t idx) const;

  T* GetMutableByIndex(int64_t idx);

  void ForEachIndexInRect(double min_x, double min_y, double max_x, double max_y,
                          std::function<void(int64_t index)> function) const;
  void ForEachIndexInCircle(double cx, double cy, double radius,
                            std::function<void(int64_t index)> function) const;

  double min_x_ = 0.0;
  double min_y_ = 0.0;
  double max_x_ = 0.0;
  double max_y_ = 0.0;
  double dimension_ = 0.0;
  int num_rows_ = 0;
  int num_columns_ = 0;

  // Map (x, y) in Pack2D format (x * num_columns + y) to the container in each grid.
  std::unordered_map<int64_t, std::unique_ptr<T>> map_index_to_container_;
};

}  // namespace utils

#include "common/utils/index/grid_index.hpp"
