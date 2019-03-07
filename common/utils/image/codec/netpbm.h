// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include "common/utils/common/defines.h"
#include "common/utils/image/codec/codec.h"
#include "common/utils/image/image.h"
#include "common/utils/status/status.h"

// Netpbm format(contains PBM, PGM, PPM) is encoded by text formating.
// It is convenient for reading, changing and diff but less effiency in storage.
// So usually, the format is only used to store unit-test image data with gzipped.
//
// For now, we just support PPM format with max value 255.

namespace image {

template <int C, int P>
class NetpbmEncoder : public ImageEncoder<uint8_t, C> {
 public:
  NetpbmEncoder() = default;
  ~NetpbmEncoder() override = default;
  utils::Status Encode(const Image<uint8_t, C>& image, std::string* encoded) override;

 private:
  DISALLOW_COPY_AND_ASSIGN(NetpbmEncoder);
};

template <int C, int P>
class NetpbmDecoder : public ImageDecoder<uint8_t, C> {
 public:
  NetpbmDecoder() = default;
  ~NetpbmDecoder() override = default;

  utils::Status Decode(const std::string& encoded, ImageBuffer<uint8_t, C>* image) override;
  utils::Status DecodeToPreallocatedImage(const std::string& encoded, Image<uint8_t, C>* image,
                                          bool check_size = true) override;

 private:
  DISALLOW_COPY_AND_ASSIGN(NetpbmDecoder);
};

using PGMTextEncoder = NetpbmEncoder<1, 2>;
using PPMTextEncoder = NetpbmEncoder<3, 3>;
using PGMTextDecoder = NetpbmDecoder<1, 2>;
using PPMTextDecoder = NetpbmDecoder<3, 3>;

using PGMEncoder = NetpbmEncoder<1, 5>;
using PPMEncoder = NetpbmEncoder<3, 6>;
using PGMDecoder = NetpbmDecoder<1, 5>;
using PPMDecoder = NetpbmDecoder<3, 6>;

}  // namespace image
