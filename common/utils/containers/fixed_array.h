// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <memory>

#include "glog/logging.h"

#include "common/utils/common/defines.h"

namespace utils {

// Movable and non-copyable container with fixed number of elements.
template <typename T>
class FixedArray {
 public:
  // If array size is not known at construction time, use this default constructor, and use
  // move assignment operator to actually create the fixed array after size is known.
  FixedArray() = default;

  // If array size is known at construction time, use this contructor.
  explicit FixedArray(int size);

  ~FixedArray() = default;

  FixedArray(FixedArray&& from);
  FixedArray& operator=(FixedArray&& from);

  int size() const { return size_; }

  T* data() { return data_.get(); }
  const T* data() const { return data_.get(); }

  T& operator[](int index) {
    DCHECK(index >= 0 && index < size_);
    return data_[index];
  }
  const T& operator[](int index) const {
    DCHECK(index >= 0 && index < size_);
    return data_[index];
  }

  const T& front() const {
    DCHECK_GT(size_, 0);
    return data_[0];
  }
  T& front() {
    DCHECK_GT(size_, 0);
    return data_[0];
  }

  const T& back() const {
    DCHECK_GT(size_, 0);
    return data_[size_ - 1];
  }
  T& back() {
    DCHECK_GT(size_, 0);
    return data_[size_ - 1];
  }

  // Range-based for requires begin() and end().
  const T* begin() const { return data_.get(); }
  T* begin() { return data_.get(); }

  const T* end() const { return data_.get() + size_; }
  T* end() { return data_.get() + size_; }

 private:
  int size_ = 0;
  std::unique_ptr<T[]> data_;

  DISALLOW_COPY_AND_ASSIGN(FixedArray);
};

template <typename T>
FixedArray<T>::FixedArray(int size) : size_(size) {
  CHECK_GE(size, 0) << "FixedArray should not have negative size.";
  data_ = std::make_unique<T[]>(size);
}

template <typename T>
FixedArray<T>::FixedArray(FixedArray&& from) {
  size_ = from.size_;
  data_ = std::move(from.data_);
}

template <typename T>
FixedArray<T>& FixedArray<T>::operator=(FixedArray&& from) {
  size_ = from.size_;
  data_ = std::move(from.data_);
  return *this;
}

}  // namespace utils
