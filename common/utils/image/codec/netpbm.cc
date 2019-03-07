// Copyright @2018 Pony AI Inc. All rights reserved.

#include "common/utils/image/codec/netpbm.h"

#include <fstream>
#include <sstream>

#include <glog/logging.h>

#include "common/utils/file/file.h"
#include "common/utils/file/gzip_file.h"
#include "common/utils/strings/format.h"

namespace image {

namespace {

struct NetpbmHeader {
  char magic;
  int width;
  int height;
  int max_value;
  int data_offset;
};

utils::Status DecodeNetpbmHeader(const std::string& encoded, NetpbmHeader* header) {
  CHECK_NOTNULL(header);
  if (encoded.front() != 'P') {
    return utils::Status::Error("Invalid Netpbm header.");
  }

  std::stringstream ss(encoded);
  ss.ignore();
  ss >> header->magic;
  ss >> header->width;
  ss >> header->height;
  ss >> header->max_value;
  // cleanup whitespace
  ss.ignore();
  // save header size for decoding data
  header->data_offset = ss.tellg();

  if (header->magic != '2' && header->magic != '3' && header->magic != '5' &&
      header->magic != '6') {
    return utils::Status::Error(strings::Format("Invalid magic number: {}", header->magic));
  }

  if (header->max_value != 255) {
    return utils::Status::Error(
        strings::Format("Max value must be 255. {} is not supported.", header->max_value));
  }

  if (header->width < 0 || header->height < 0) {
    return utils::Status::Error(
        strings::Format("Invalid image size: {} x {}", header->width, header->height));
  }

  return utils::Status::OK;
}

template <int C>
utils::Status DecodeNetpbmData(const std::string& encoded, const NetpbmHeader& header,
                               image::Image<uint8_t, C>* image) {
  CHECK_NOTNULL(image);

  if (header.magic == '2' || header.magic == '5') {
    DCHECK_EQ(C, 1);
  } else if (header.magic == '3' || header.magic == '6') {
    DCHECK_EQ(C, 3);
  }

  if (header.magic == '2' || header.magic == '3') {
    std::stringstream ss(encoded);
    ss.seekg(header.data_offset);
    int value = 0;
    for (int y = 0; y < header.height; ++y) {
      for (int x = 0; x < header.width; ++x) {
        for (int k = 0; k < C; ++k) {
          CHECK(!ss.eof());
          ss >> value;
          (*image)(x, y)[k] = value;
        }
      }
    }
  } else if (header.magic == '5' || header.magic == '6') {
    size_t dst_size = image->height() * image->width() * C;
    size_t src_size = encoded.size() - header.data_offset;
    if (dst_size != src_size) {
      return utils::Status::Error(
          strings::Format("Data size {} does not match image size {}.", src_size, dst_size));
    }
    char* raw_data = const_cast<char*>(encoded.data());
    uint8_t* src = reinterpret_cast<uint8_t*>(raw_data) + header.data_offset;
    int64_t pitch = image->width() * C;
    image->CopyFrom(ImageView<uint8_t, C>(src, image->width(), image->height(), pitch));
  }

  return utils::Status::OK;
}

}  // namespace

template <int C, int P>
utils::Status NetpbmEncoder<C, P>::Encode(const image::Image<uint8_t, C>& image,
                                          std::string* encoded) {
  if (C == 1) {
    CHECK(P == 2 || P == 5) << "Magic number must be 2 or 5 for Portable GrayMap.";
  } else if (C == 3) {
    CHECK(P == 3 || P == 6) << "Magic number must be 3 or 6 for Portable PixMap.";
  } else {
    LOG(FATAL) << "Unsupported Netpbm format.";
  }

  std::stringstream ss;
  ss << "P" << P << "\n";
  ss << image.width() << " " << image.height() << "\n";
  ss << "255\n";

  if (P == 2 || P == 3) {
    for (int y = 0; y < image.height(); ++y) {
      for (int x = 0; x < image.width(); ++x) {
        for (int k = 0; k < C; ++k) {
          ss << static_cast<int>(image(x, y)[k]) << " ";
        }
      }
      ss << "\n";
    }
    *encoded = ss.str();
  } else if (P == 5 || P == 6) {
    encoded->reserve(ss.str().size() + image.height() * image.width() * C);
    *encoded = ss.str();
    for (int y = 0; y < image.height(); ++y) {
      *encoded += std::string(reinterpret_cast<char const*>(image.Row(y)), image.width() * C);
    }
  }

  return utils::Status::OK;
}

template <int C, int P>
utils::Status NetpbmDecoder<C, P>::Decode(const std::string& encoded,
                                          ImageBuffer<uint8_t, C>* image) {
  CHECK_NOTNULL(image);
  NetpbmHeader header;
  RETURN_IF_ERROR(DecodeNetpbmHeader(encoded, &header));

  image->Create(header.width, header.height);

  return DecodeNetpbmData(encoded, header, image);
}

template <int C, int P>
utils::Status NetpbmDecoder<C, P>::DecodeToPreallocatedImage(const std::string& encoded,
                                                             Image<uint8_t, C>* image,
                                                             bool check_size) {
  CHECK_NOTNULL(image);
  NetpbmHeader header;
  RETURN_IF_ERROR(DecodeNetpbmHeader(encoded, &header));

  if (check_size && (image->width() != header.width || image->height() != header.height)) {
    return utils::Status::Error(
        strings::Format("Decoded size {}x{} does not match image size {}x{}", header.width,
                        header.height, image->width(), image->height()));
  }

  return DecodeNetpbmData(encoded, header, image);
}

// Explicit template instantiation.

// text pgm encoder
template class NetpbmEncoder<1, 2>;
// binary pgm encoder
template class NetpbmEncoder<1, 5>;
// text ppm encoder
template class NetpbmEncoder<3, 3>;
// binary ppm encoder
template class NetpbmEncoder<3, 6>;

// text pgm decoder
template class NetpbmDecoder<1, 2>;
// binary pgm decoder
template class NetpbmDecoder<1, 5>;
// text ppm decoder
template class NetpbmDecoder<3, 3>;
// binary ppm decoder
template class NetpbmDecoder<3, 6>;

}  // namespace image
