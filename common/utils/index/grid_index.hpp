// Copyright @2018 Pony AI Inc. All rights reserved.

#include "common/utils/index/grid_index.h"
#include "common/utils/math/math_utils.h"

namespace utils {

template<class T>
GridIndex<T>::GridIndex(double min_x, double min_y, double max_x, double max_y, double dimension)
    : min_x_(min_x),
      min_y_(min_y),
      max_x_(max_x),
      max_y_(max_y),
      dimension_(dimension) {
  CHECK_LE(min_x_, max_x_);
  CHECK_LE(min_y_, max_y_);
  CHECK_GT(dimension_, 0.0);
  num_rows_ = static_cast<int>((max_x_ - min_x_) / dimension) + 1;
  num_columns_ = static_cast<int>((max_y_ - min_y_) / dimension) + 1;
  map_index_to_container_.clear();
}

template<class T>
const T* GridIndex<T>::Get(double x, double y) const {
  int64_t idx = GetIndex(x, y);
  if (idx < 0) {
    return nullptr;
  }
  return GetByIndex(idx);
}

template<class T>
T* GridIndex<T>::GetMutable(double x, double y) {
  int64_t idx = GetIndex(x, y);
  if (idx < 0) {
    return nullptr;
  }
  return GetMutableByIndex(idx);
}

template<class T>
void GridIndex<T>::ForEachIndexInRect(double min_x, double min_y, double max_x, double max_y,
                                      std::function<void(int64_t index)> function) const {
  min_x = std::max(min_x, min_x_);
  min_y = std::max(min_y, min_y_);
  max_x = std::min(max_x, max_x_);
  max_y = std::min(max_y, max_y_);
  if (min_x > max_x || min_y > max_y) {
    return;
  }
  const int gx1 = static_cast<int>((min_x - min_x_) / dimension_);
  const int gy1 = static_cast<int>((min_y - min_y_) / dimension_);
  const int gx2 = static_cast<int>((max_x - min_x_) / dimension_);
  const int gy2 = static_cast<int>((max_y - min_y_) / dimension_);
  for (int x = gx1; x <= gx2; ++x) {
    const int64_t id1 = Pack2D(x, gy1);
    const int64_t id2 = Pack2D(x, gy2);
    for (int64_t id = id1; id <= id2; ++id) {
      function(id);
    }
  }
}

template<class T>
void GridIndex<T>::ForEachIndexInCircle(double cx, double cy, double radius,
                                        std::function<void(int64_t index)> function) const {
  radius = std::max(0.0, radius);
  const double min_x = std::max(cx - radius, min_x_);
  const double max_x = std::min(cx + radius, max_x_);
  if (min_x > max_x) {
    return;
  }
  const double radius_sqr = math::Sqr(radius);
  const int gx1 = static_cast<int>((min_x - min_x_) / dimension_);
  const int gx2 = static_cast<int>((max_x - min_x_) / dimension_);

  double split_x = min_x_ + dimension_ * static_cast<double>(gx1);
  for (int x = gx1; x <= gx2; ++x) {
    const double next_split_x = split_x + dimension_;
    double projection = 0.0;
    if (split_x >= cx) {
      projection = sqrt(std::max(0.0, radius_sqr - math::Sqr(split_x - cx)));
    } else if (next_split_x <= cx) {
      projection = sqrt(std::max(0.0, radius_sqr - math::Sqr(cx - next_split_x)));
    } else {
      projection = radius;
    }
    const double min_y = std::max(cy - projection, min_y_);
    const double max_y = std::min(cy + projection, max_y_);
    if (min_y <= max_y) {
      const int gy1 = static_cast<int>((min_y - min_y_) / dimension_);
      const int gy2 = static_cast<int>((max_y - min_y_) / dimension_);
      const int64_t id1 = Pack2D(x, gy1);
      const int64_t id2 = Pack2D(x, gy2);
      for (int64_t id = id1; id <= id2; ++id) {
        function(id);
      }
    }
    split_x = next_split_x;
  }
}

template<class T>
std::vector<const T*> GridIndex<T>::GetAll() const {
  std::vector<const T*> result;
  for (const auto& key_value : map_index_to_container_) {
    result.push_back(key_value.second.get());
  }
  return result;
}

template<class T>
std::vector<const T*> GridIndex<T>::GetAllInRect(
    double min_x, double min_y, double max_x, double max_y) const {
  std::vector<const T*> result;
  ForEachIndexInRect(min_x, min_y, max_x, max_y, [&result, this](const int64_t id) {
    const T* pointer = GetByIndex(id);
    if (pointer != nullptr) {
      result.push_back(pointer);
    }
  });
  return result;
}

template<class T>
std::vector<T*> GridIndex<T>::GetAllMutableInRect(
    double min_x, double min_y, double max_x, double max_y) {
  std::vector<T*> result;
  ForEachIndexInRect(min_x, min_y, max_x, max_y, [&result, this](const int64_t id) {
    result.push_back(GetMutableByIndex(id));
  });
  return result;
}

template<class T>
std::vector<const T*> GridIndex<T>::GetAllInCircle(double cx, double cy, double radius) const {
  if (cx - radius * 0.7 < min_x_ && cx + radius * 0.7 > max_x_ && cy - radius * 0.7 < min_y_
      && cy + radius * 0.7 > max_y_) {
    return GetAll();
  }
  std::vector<const T*> result;
  ForEachIndexInCircle(cx, cy, radius, [&result, this](const int64_t id) {
    const T* pointer = GetByIndex(id);
    if (pointer != nullptr) {
      result.push_back(pointer);
    }
  });
  return result;
}

template<class T>
std::vector<T*> GridIndex<T>::GetAllMutableInCircle(double cx, double cy, double radius) {
  std::vector<T*> result;
  ForEachIndexInCircle(cx, cy, radius, [&result, this](const int64_t id) {
    result.push_back(GetMutableByIndex(id));
  });
  return result;
}

template<class T>
int64_t GridIndex<T>::Pack2D(int x,int y) const {
  CHECK_GE(x, 0);
  CHECK_LT(x, num_rows_);
  CHECK_GE(y, 0);
  CHECK_LT(y, num_columns_);
  return static_cast<int64_t>(x) * num_columns_ + static_cast<int64_t>(y);
}

template<class T>
int64_t GridIndex<T>::GetIndex(double x, double y) const {
  if (x < min_x_ || y < min_y_ || x > max_x_ || y > max_y_) {
    return -1;
  }
  const int gx = static_cast<int>((x - min_x_) / dimension_);
  const int gy = static_cast<int>((y - min_y_) / dimension_);
  return Pack2D(gx, gy);
}

template<class T>
const T* GridIndex<T>::GetByIndex(int64_t idx) const {
  const auto& it = map_index_to_container_.find(idx);
  if (it == map_index_to_container_.end()) {
    return nullptr;
  }
  return it->second.get();
}

template<class T>
T* GridIndex<T>::GetMutableByIndex(int64_t idx) {
  auto& pointer = map_index_to_container_[idx];
  if (pointer == nullptr) {
    pointer.reset(new T());
  }
  return pointer.get();
}

}  // namespace utils
