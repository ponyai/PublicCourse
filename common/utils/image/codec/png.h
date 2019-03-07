// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <algorithm>
#include <sstream>

#include <png.h>

#include "common/utils/image/codec/codec.h"

namespace image {

// Returns whether a byte string is JPEG encoding result.
bool IsPngEncoded(const std::string& s);

// NOT thread-safe.
template <int C>
class PngEncoder : public ImageEncoder<uint8_t, C> {
 public:
  PngEncoder();
  ~PngEncoder() override{};

  bool IsLossy() const override { return false; }

  utils::Status Encode(const Image<uint8_t, C>& image, std::string* encoded) override;
};

// NOT thread-safe.
template <int C>
class PngDecoder : public ImageDecoder<uint8_t, C> {
 public:
  PngDecoder() {}
  ~PngDecoder() override {}

  utils::Status Decode(const std::string& encoded, ImageBuffer<uint8_t, C>* image) override;
  utils::Status DecodeToPreallocatedImage(const std::string& encoded, Image<uint8_t, C>* image,
                                          bool check_size = true) override;

 private:
  void InitializeHandlers();

 private:
  png_structp png_ptr_ = nullptr;
  png_infop info_ptr_ = nullptr;
};

using ColorAlphaPngEncoder = PngEncoder<4>;
using ColorPngEncoder = PngEncoder<3>;
using GrayscalePngEncoder = PngEncoder<1>;
using ColorAlphaPngDecoder = PngDecoder<4>;
using ColorPngDecoder = PngDecoder<3>;
using GrayscalePngDecoder = PngDecoder<1>;

// ======================================== Implementation ========================================

inline bool IsPngEncoded(const std::string& s) {
  constexpr int kPngSigSize = 8;
  // Allocate a buffer of 8 bytes, where we can put the file signature.
  png_byte pngsig[kPngSigSize];

  std::stringstream source(s);
  // Read the 8 bytes from the stream into the sig buffer.
  source.read(reinterpret_cast<char*>(pngsig), kPngSigSize);

  // Check if the read worked...
  if (!source.good()) return false;

  return png_sig_cmp(pngsig, 0, kPngSigSize) == 0;
}

}  // namespace image
