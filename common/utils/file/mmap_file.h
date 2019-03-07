// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <cstdio>

#include "common/utils/containers/array_view.h"
#include "common/utils/file/file.h"

namespace file {

class MmapFile {
 public:
  explicit MmapFile(const std::string& file_path) : MmapFile(file_path, MAP_PRIVATE) {}

  explicit MmapFile(const std::string& file_path, int flags) {
    int fd = open(file_path.c_str(), O_RDONLY);
    size_ = GetFileSize(file_path);
    data_ = static_cast<char*>(mmap(nullptr, size_, PROT_READ, flags, fd, 0));
    CHECK(data_ != nullptr);
    close(fd);
  }

  ~MmapFile() {
    if (data_ != nullptr) {
      if (!Close()) {
        LOG(ERROR) << "Error while closing MmapFile.";
      }
    }
  }

  MUST_USE_RESULT bool Close() {
    if (data_ != nullptr) {
      int res = munmap(data_, size_);
      data_ = nullptr;
      return res == 0;
    }
    return false;
  }

  const char* data() const { return data_; }

  int64_t size() const { return size_; }

  template <typename T>
  utils::ConstArrayView<T> View(int64_t offset_in_bytes, int64_t num_items) const {
    CHECK_GE(offset_in_bytes, 0);
    CHECK_LE(offset_in_bytes + num_items * sizeof(T), size_);
    return utils::ConstArrayView<T>(reinterpret_cast<const T*>(data_ + offset_in_bytes), num_items);
  }

 private:
  char* data_ = nullptr;
  int64_t size_ = 0;
};

}  // namespace file
