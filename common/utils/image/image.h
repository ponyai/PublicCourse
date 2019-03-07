// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <algorithm>

#include "common/utils/common/defines.h"
#include "glog/logging.h"

namespace image {

template <typename T, int C>
class ImageView;

// An image with C channels. It can be
//  * a view of a chunk of memory, which is defined in the derived class 'ImageView', OR
//  * a container that owns the memory by itself, which is defined in the derived class 'Image'.
//
// Pixels in an image are referenced as image(x, y)[channel], in the following layout:
//  * ADDRESS(image(x, y)[c + 1]) - ADDRESS(image(x, y)[c]) = sizeof(T);
//  * ADDRESS(image(x + 1, y)[c]) - ADDRESS(image(x, y)[c]) = sizeof(T) * C;
//  * ADDRESS(image(x, y + 1)[c]) - ADDRESS(image(x, y)[c]) = stride_in_bytes;
// where 'stride_in_bytes' is the distance between the beginning addresses of adjacent rows, in
// bytes.
template <typename T, int C>
class Image {
 public:
  static_assert(C > 0, "An image should have at least one channel.");

  virtual ~Image() = default;

  static constexpr int NumBytesPerPixel() { return sizeof(T) * C; }

  // Number of bytes for a row if it is stored compactly.
  static int64_t CompactStrideInBytes(int64_t width);

  const T* operator()(int64_t x, int64_t y) const;
  T* operator()(int64_t x, int64_t y);

  const T* data() const { return data_; }
  T* data() { return data_; }

  int64_t width() const { return width_; }
  int64_t height() const { return height_; }
  int64_t stride_in_bytes() const { return stride_in_bytes_; }

  const T* Row(int64_t y) const;
  T* Row(int64_t y);

  // NOTE: If a const Image uses this function, the result image view will lose its constness.
  // The ultimate resolution for this issue is to have a ConstImageView class, but it will make both
  // implementation and usage of {Const,}ImageView very complex. After discussion, we decided to
  // use the current design.
  ImageView<T, C> Region(int64_t start_x, int64_t start_y, int64_t width, int64_t height) const;

  // NOTE: If a const Image uses this function, the result image view will lose its constness.
  // The ultimate resolution for this issue is to have a ConstImageView class, but it will make both
  // implementation and usage of {Const,}ImageView very complex. After discussion, we decided to
  // use the current design.
  ImageView<T, C> View() const;

  // Applies a deep copy.
  void CopyFrom(const Image& image);

  // Fills one channel of all pixels with a given value.
  void FillValue(T value, int channel);

 protected:
  // In any case, Image should not be created. Based on the use case, create either ImageView or
  // ImageBuffer.
  Image() = default;

  T* data_ = nullptr;
  int64_t width_ = 0;
  int64_t height_ = 0;
  int64_t stride_in_bytes_ = 0;

  DISALLOW_COPY_MOVE_AND_ASSIGN(Image);
};

// Does not own its referenced memory chunks, so it can be trivially and safely shallow-copied.
template <typename T, int C>
class ImageView : public Image<T, C> {
 public:
  // If 'stride_in_bytes' is zero, then the actual stride is automatically computed as
  // 'width * C * sizeof(T)'.
  ImageView(T* data, int64_t width, int64_t height, int64_t stride_in_bytes = 0);
  ~ImageView() override = default;

  // ImageView's copy is a shallow copy.
  ImageView(const ImageView& image);
  ImageView& operator=(const ImageView& view);

 protected:
  using Image<T, C>::data_;
  using Image<T, C>::width_;
  using Image<T, C>::height_;
  using Image<T, C>::stride_in_bytes_;
  using Image<T, C>::CompactStrideInBytes;
};

// Nearly same as ImageView, except for the owernship of the memory.
template <typename T, int C>
class ImageBuffer : public Image<T, C> {
 public:
  ImageBuffer() = default;
  ImageBuffer(int64_t width, int64_t height, int64_t stride_in_bytes = 0);
  ~ImageBuffer() override;

  ImageBuffer(ImageBuffer&&);
  ImageBuffer& operator=(ImageBuffer&&);

  int64_t buffer_size() { return stride_in_bytes_ * height_; }

  // Allocates just-sufficient, i.e., (stride_in_bytes * height)-byte,  memory for the specified
  // size. The original memory content will be destroyed if its size does not match the new size.
  // 'stride_in_bytes' is the distance between the beginning addresses of adjacent rows, in bytes.
  // If it is zero, then the actual stride is automatically computed as 'width * C * sizeof(T)'.
  void Create(int64_t width, int64_t height, int64_t stride_in_bytes = 0);

  // Deallocates memory, and resets data pointer and image size.
  // It is okay to destroy an unallocated image.
  void Destroy();

 protected:
  using Image<T, C>::data_;
  using Image<T, C>::width_;
  using Image<T, C>::height_;
  using Image<T, C>::stride_in_bytes_;
  using Image<T, C>::CompactStrideInBytes;

 private:
  DISALLOW_COPY_AND_ASSIGN(ImageBuffer);
};

// ---------------------------------------- Implementation ----------------------------------------

template <typename T, int C>
inline int64_t Image<T, C>::CompactStrideInBytes(int64_t width) {
  return width * C * sizeof(T);
}

template <typename T, int C>
inline const T* Image<T, C>::Row(int64_t y) const {
  DCHECK(y >= 0 && y < height_);
  const uint8_t* ptr_u8 = reinterpret_cast<const uint8_t*>(data_);
  ptr_u8 += stride_in_bytes_ * y;
  return reinterpret_cast<const T*>(ptr_u8);
}

template <typename T, int C>
inline T* Image<T, C>::Row(int64_t y) {
  DCHECK(y >= 0 && y < height_);
  uint8_t* ptr_u8 = reinterpret_cast<uint8_t*>(data_);
  ptr_u8 += stride_in_bytes_ * y;
  return reinterpret_cast<T*>(ptr_u8);
}

template <typename T, int C>
inline const T* Image<T, C>::operator()(int64_t x, int64_t y) const {
  DCHECK(x >= 0 && x < width_);
  DCHECK(y >= 0 && y < height_);
  return Row(y) + x * C;
}

template <typename T, int C>
T* Image<T, C>::operator()(int64_t x, int64_t y) {
  DCHECK(x >= 0 && x < width_);
  DCHECK(y >= 0 && y < height_);
  return Row(y) + x * C;
}

template <typename T, int C>
void Image<T, C>::FillValue(T value, int channel) {
  for (int i = 0; i < height(); i++) {
    T* row_data = Row(i);
    for (int j = 0; j < width(); j++) {
      row_data[j * C + channel] = value;
    }
  }
}

template <typename T, int C>
ImageView<T, C> Image<T, C>::Region(int64_t start_x, int64_t start_y, int64_t width,
                                    int64_t height) const {
  DCHECK_GE(start_x, 0);
  DCHECK_GE(start_y, 0);
  DCHECK(width >= 0 && start_x + width <= width_);
  DCHECK(height >= 0 && start_y + height <= height_);
  return ImageView<T, C>(const_cast<T*>((*this)(start_x, start_y)), width, height,
                         stride_in_bytes_);
}

template <typename T, int C>
ImageView<T, C> Image<T, C>::View() const {
  return ImageView<T, C>(const_cast<T*>(data_), width_, height_, stride_in_bytes_);
}

template <typename T, int C>
void Image<T, C>::CopyFrom(const Image& image) {
  CHECK(image.width_ == width_ && image.height_ == height_);
  for (int y = 0; y < height_; ++y) {
    std::copy_n(image.Row(y), width_ * C, Row(y));
  }
}

template <typename T, int C>
inline ImageView<T, C>::ImageView(T* data, int64_t width, int64_t height, int64_t stride_in_bytes) {
  DCHECK_GT(width, 0);
  DCHECK_GT(height, 0);

  data_ = DCHECK_NOTNULL(data);
  width_ = width;
  height_ = height;

  if (stride_in_bytes == 0) {
    stride_in_bytes_ = CompactStrideInBytes(width);
  } else {
    DCHECK_GE(stride_in_bytes, CompactStrideInBytes(width));
    stride_in_bytes_ = stride_in_bytes;
  }
}

template <typename T, int C>
inline ImageView<T, C>::ImageView(const ImageView<T, C>& view)
    : ImageView(const_cast<T*>(view.data()), view.width(), view.height(), view.stride_in_bytes()) {}

template <typename T, int C>
inline ImageView<T, C>& ImageView<T, C>::operator=(const ImageView<T, C>& view) {
  data_ = view.data_;
  width_ = view.width_;
  height_ = view.height_;
  stride_in_bytes_ = view.stride_in_bytes_;
  return *this;
}

template <typename T, int C>
inline ImageBuffer<T, C>::ImageBuffer(int64_t width, int64_t height, int64_t stride_in_bytes) {
  Create(width, height, stride_in_bytes);
}

template <typename T, int C>
inline ImageBuffer<T, C>::~ImageBuffer() {
  Destroy();
}

template <typename T, int C>
inline ImageBuffer<T, C>::ImageBuffer(ImageBuffer&& rhs) {
  using std::swap;
  swap(data_, rhs.data_);
  swap(width_, rhs.width_);
  swap(height_, rhs.height_);
  swap(stride_in_bytes_, rhs.stride_in_bytes_);
}

template <typename T, int C>
ImageBuffer<T, C>& ImageBuffer<T, C>::operator=(ImageBuffer&& rhs) {
  using std::swap;
  swap(data_, rhs.data_);
  swap(width_, rhs.width_);
  swap(height_, rhs.height_);
  swap(stride_in_bytes_, rhs.stride_in_bytes_);
  return *this;
}

template <typename T, int C>
inline void ImageBuffer<T, C>::Create(int64_t width, int64_t height, int64_t stride_in_bytes) {
  CHECK_GT(width, 0);
  CHECK_GT(height, 0);
  if (stride_in_bytes == 0) {
    stride_in_bytes = CompactStrideInBytes(width);
  } else {
    CHECK_GE(stride_in_bytes, CompactStrideInBytes(width));
  }

  if (height * stride_in_bytes == height_ * stride_in_bytes_) {
    // If memory size does not change, then no need to do anything.
    return;
  }

  Destroy();

  width_ = width;
  height_ = height;
  stride_in_bytes_ = stride_in_bytes;

  int64_t num_bytes = stride_in_bytes_ * height_;
  // Since 'num_bytes' potentially is not divisible by sizeof(T), we can't use
  // 'new' to allocate memory.
  data_ = reinterpret_cast<T*>(::malloc(num_bytes));
  CHECK(data_ != nullptr) << "Failed to allocate an image with " << num_bytes
                          << " bytes. Error number: " << errno << ".";
}

template <typename T, int C>
inline void ImageBuffer<T, C>::Destroy() {
  ::free(data_);
  data_ = nullptr;
  width_ = 0;
  height_ = 0;
  stride_in_bytes_ = 0;
}

}  // namespace image
