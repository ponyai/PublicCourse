// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <algorithm>

#include <turbojpeg.h>

#include "common/utils/image/codec/codec.h"

namespace image {

// Returns whether a byte string is JPEG encoding result.
bool IsJpegEncoded(const std::string& s);

// NOT thread-safe.
template <int C>
class JpegEncoder : public ImageEncoder<uint8_t, C> {
 public:
  struct Options {
    TJPF pixel_format = (C == 1 ? TJPF_GRAY : TJPF_RGB);
    TJSAMP color_sampling = (C == 1 ? TJSAMP_GRAY : TJSAMP_422);
    int quality = 90;
  };

  JpegEncoder();
  explicit JpegEncoder(const Options& options);
  ~JpegEncoder() override;

  utils::Status Encode(const Image<uint8_t, C>& image, std::string* encoded) override;

 private:
  void ValidateOptions() const;

  Options options_;
  tjhandle handle_ = nullptr;
};

// NOT thread-safe.
template <int C>
class JpegDecoder : public ImageDecoder<uint8_t, C> {
 public:
  JpegDecoder();
  ~JpegDecoder() override;

  utils::Status Decode(const std::string& encoded, ImageBuffer<uint8_t, C>* image) override;
  utils::Status DecodeToPreallocatedImage(const std::string& encoded, Image<uint8_t, C>* image,
                                          bool check_size = true) override;

 private:
  tjhandle handle_ = nullptr;
};

using ColorJpegEncoder = JpegEncoder<3>;
using GrayscaleJpegEncoder = JpegEncoder<1>;
using ColorJpegDecoder = JpegDecoder<3>;
using GrayscaleJpegDecoder = JpegDecoder<1>;

// ======================================== Implementation ========================================

inline bool IsJpegEncoded(const std::string& s) {
  return strings::StartWith(s, "\xff\xd8") && strings::EndWith(s, "\xff\xd9");
}

}  // namespace image
