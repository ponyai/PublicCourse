// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <glog/logging.h>

namespace utils {

// A view of a mutable contiguous array of elements with type T. Use ConstArrayView<T> if you want
// a immutable view.
//
// ArrayView does NOT take the ownership of the memory. Caller has the responsibility to make sure
// the referenced memory chunk is valid until the array view is destroyed.
//
// Copy of ArrayView is a trivial shallow copy. Use std::copy or std::copy_n for deep copy. Because
// the copy operation is so trivial, copy should be always used instead of move.
//
// NOTE: The [] operator only checks index in debug mode.
template <typename T>
class ArrayView {
 public:
  ArrayView(T* data, int size);
  ~ArrayView() = default;

  int size() const { return size_; }

  T* data() { return data_; }
  const T* data() const { return data_; }

  T* begin() { return data_; }
  const T* begin() const { return data_; }

  T* end() { return data_ + size_; }
  const T* end() const { return data_ + size_; }

  T& front() { return (*this)[0]; }
  const T& front() const { return (*this)[0]; }

  T& back() { return (*this)[size_ - 1]; }
  const T& back() const { return (*this)[size_ - 1]; }

  T& operator[](int index);
  const T& operator[](int index) const;

 protected:
  T* data_ = nullptr;  // Not owned.
  int size_ = 0;

  // Shallow copy and move are OK.
};

template <typename T>
using ConstArrayView = ArrayView<const T>;

template <typename T>
ArrayView<T>::ArrayView(T* data, int size) : data_(data), size_(size) {
  CHECK_GE(size, 0) << "ArrayView should not have negative size.";
  if (size > 0) {
    CHECK(data != nullptr)
        << "Non-empty ArrayView should not have its beginning data address at nullptr.";
  }
}

template <typename T>
T& ArrayView<T>::operator[](int index) {
  DCHECK(0 <= index && index < size_);
  return data_[index];
}

template <typename T>
const T& ArrayView<T>::operator[](int index) const {
  DCHECK(0 <= index && index < size_);
  return data_[index];
}

}  // namespace utils
