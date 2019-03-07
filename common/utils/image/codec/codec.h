// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <algorithm>

#include "common/utils/file/file.h"
#include "common/utils/image/image.h"

namespace image {

// Reasoning about templating the class rather than method functions: ImageEncoder and ImageDecoder
// base classes. Their methods have to be virtual functions. However, virtual function is not
// allowed to be templated.

// For low-dynamic-range (i.e. 8-bit-per-channel) images. NOT thread-safe.
template <typename T, int C>
class ImageEncoder {
 public:
  ImageEncoder() = default;
  virtual ~ImageEncoder() = default;

  // Whether this encoding method is lossy or lossless. It's recommended to implement this method
  // explicitly since it helps users understand the codec's output image better.
  // For more details, please refere to:
  //   https://en.wikipedia.org/wiki/Lossy_compression
  //   https://en.wikipedia.org/wiki/Lossless_compression
  virtual bool IsLossy() const { return true; }

  // Encodes the image content into 'encoded'. The encoder will resize 'encoded' to exactly fit in
  // the encoding result.
  virtual utils::Status Encode(const image::Image<T, C>& image, std::string* encoded) = 0;

  // Encodes the image content and writes into a file with the specified filename. By default, it is
  // implemented as encoding into a string and then writing it into the file. However, if the
  // underlying library has a faster implementation, this function can be overriden to use that.
  virtual utils::Status Save(const image::Image<T, C>& image, const std::string& filename);

 protected:
  std::string workspace_;
};

// For low-dynamic-range (i.e. 8-bit-per-channel) images. NOT thread-safe.
template <typename T, int C>
class ImageDecoder {
 public:
  ImageDecoder() = default;
  virtual ~ImageDecoder() = default;

  // Decodes the encoded the string, set size for 'image', and store the decoded pixels into it.
  virtual utils::Status Decode(const std::string& encoded, image::ImageBuffer<T, C>* image) = 0;

  // Decodes the encoded the string, and store the decoded pixels into 'image', assuming its
  // original size is correct. If 'check_size' is true, then if the actual size does not match the
  // size of 'image', an error status will be returned.
  virtual utils::Status DecodeToPreallocatedImage(const std::string& encoded,
                                                  image::Image<T, C>* image,
                                                  bool check_size = true) = 0;

  // Loads the image content from a file with the specified filename. By default, it is implemented
  // as reading the encoded string from the file and decode into an image. However, if the
  // underlying library has a faster implementation, this function can be overriden to use that.
  virtual utils::Status Load(const std::string& filename, image::ImageBuffer<T, C>* image);

  // Nearly same as the Load(), except that 'image' is assumed to be preallocated.
  virtual utils::Status LoadToPreallocatedImage(const std::string& filename,
                                                image::Image<T, C>* image, bool check_size = true);

 protected:
  std::string workspace_;
};

// ======================================== Implementation ========================================

template <typename T, int C>
inline utils::Status ImageEncoder<T, C>::Save(const image::Image<T, C>& image,
                                              const std::string& filename) {
  RETURN_IF_ERROR(Encode(image, &workspace_));
  const bool success = file::WriteContents(filename, workspace_);
  if (success) {
    return utils::Status::OK;
  }
  return utils::Status(utils::error::INTERNAL_ERROR,
                       "Failed to write the encoded image into file at " + filename);
}

template <typename T, int C>
inline utils::Status ImageDecoder<T, C>::Load(const std::string& filename,
                                              image::ImageBuffer<T, C>* image) {
  file::ReadContents(filename, &workspace_);
  return Decode(workspace_, image);
}

template <typename T, int C>
inline utils::Status ImageDecoder<T, C>::LoadToPreallocatedImage(const std::string& filename,
                                                                 image::Image<T, C>* image,
                                                                 bool check_size) {
  file::ReadContents(filename, &workspace_);
  return DecodeToPreallocatedImage(workspace_, image, check_size);
}

}  // namespace image
