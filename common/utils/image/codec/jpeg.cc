// Copyright @2018 Pony AI Inc. All rights reserved.

#include "common/utils/image/codec/jpeg.h"

#include "glog/logging.h"

namespace image {

namespace {

int NumChannels(TJPF pixel_format) {
  switch (pixel_format) {
    case TJPF_RGB:
    case TJPF_BGR:
      return 3;
    case TJPF_GRAY:
      return 1;

    // Unsupported formats.
    case TJPF_ABGR:
    case TJPF_BGRA:
    case TJPF_ARGB:
    case TJPF_RGBA:
    case TJPF_XBGR:
    case TJPF_BGRX:
    case TJPF_XRGB:
    case TJPF_RGBX:
    case TJPF_CMYK:
    default:
      return -1;
  }
}

}  // namespace

template <int C>
JpegEncoder<C>::JpegEncoder() : handle_(tjInitCompress()) {
  CHECK(handle_ != nullptr);
  ValidateOptions();
}

template <int C>
JpegEncoder<C>::JpegEncoder(const Options& options) : options_(options), handle_(tjInitCompress()) {
  CHECK(handle_ != nullptr);
  ValidateOptions();
}

template <int C>
JpegEncoder<C>::~JpegEncoder() {
  if (handle_) {
    tjDestroy(handle_);
  }
}

template <int C>
void JpegEncoder<C>::ValidateOptions() const {
  CHECK_GE(options_.quality, 1);
  CHECK_LE(options_.quality, 100);

  CHECK_EQ(C, NumChannels(options_.pixel_format)) << "Pixel format (" << options_.pixel_format
                                                  << ") does not match num of channels (" << C
                                                  << ").";

  if ((options_.color_sampling == TJSAMP_GRAY) != (options_.pixel_format == TJPF_GRAY)) {
    LOG(FATAL) << "Pixel format (" << options_.pixel_format
               << ") does not match color sampling scheme (" << options_.color_sampling << ").";
  }
}

template <int C>
utils::Status JpegEncoder<C>::Encode(const Image<uint8_t, C>& image, std::string* encoded) {
  encoded->resize(tjBufSize(image.width(), image.height(), options_.color_sampling));
  uint8_t* buf = reinterpret_cast<uint8_t*>(&(*encoded)[0]);
  uint64_t size = encoded->size();
  if (tjCompress2(handle_, image.data(), image.width(), image.stride_in_bytes(), image.height(),
                  options_.pixel_format, &buf, &size, options_.color_sampling, options_.quality,
                  0) != 0) {
    std::string error_message = "Failed to compress image into JPEG: ";
    error_message += tjGetErrorStr();
    return utils::Status(utils::error::INTERNAL_ERROR, error_message);
  }
  CHECK(reinterpret_cast<char*>(buf) == encoded->data())
      << "Somehow, libjpeg-turbo reallocates the buffer, which should never "
         "happen if the "
         "preallocated buffer size is determined by tjBufSize().";
  encoded->resize(size);
  return utils::Status::OK;
}

template <int C>
JpegDecoder<C>::JpegDecoder() : handle_(tjInitDecompress()) {
  CHECK(handle_ != nullptr);
}

template <int C>
JpegDecoder<C>::~JpegDecoder() {
  if (handle_) {
    tjDestroy(handle_);
  }
}

template <int C>
utils::Status JpegDecoder<C>::Decode(const std::string& encoded, ImageBuffer<uint8_t, C>* image) {
  CHECK(image != nullptr);

  const auto* jpeg_buf = reinterpret_cast<const uint8_t*>(&encoded[0]);
  const uint64_t jpeg_size = encoded.size();

  int width = 0;
  int height = 0;
  if (tjDecompressHeader(handle_, const_cast<uint8_t*>(jpeg_buf), jpeg_size, &width, &height) !=
      0) {
    std::string error_message = "Failed to get image dimensions from the encoded JPEG: ";
    error_message += tjGetErrorStr();
  }
  image->Create(width, height);

  return DecodeToPreallocatedImage(encoded, image, false);
}

template <int C>
utils::Status JpegDecoder<C>::DecodeToPreallocatedImage(const std::string& encoded,
                                                        Image<uint8_t, C>* image, bool check_size) {
  CHECK(image != nullptr);

  const auto* jpeg_buf = reinterpret_cast<const uint8_t*>(&encoded[0]);
  const uint64_t jpeg_size = encoded.size();

  const int width = image->width();
  const int height = image->height();
  if (check_size) {
    int actual_width = 0;
    int actual_height = 0;
    tjDecompressHeader(handle_, const_cast<uint8_t*>(jpeg_buf), jpeg_size, &actual_width,
                       &actual_height);
    CHECK_EQ(width, actual_width);
    CHECK_EQ(height, actual_height);
  }

  const TJPF pixel_format = C == 1 ? TJPF_GRAY : TJPF_RGB;
  if (tjDecompress2(handle_, jpeg_buf, jpeg_size, image->data(), image->width(),
                    image->stride_in_bytes(), image->height(), pixel_format, 0) != 0) {
    std::string error_message = "Failed to decompress image from JPEG: ";
    error_message += tjGetErrorStr();
    return utils::Status(utils::error::INTERNAL_ERROR, error_message);
  }
  return utils::Status::OK;
}

// Explicit template instantiation.
template class JpegEncoder<1>;
template class JpegEncoder<3>;
template class JpegDecoder<1>;
template class JpegDecoder<3>;

}  // namespace image
