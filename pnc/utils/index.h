// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include "common/utils/file/file.h"

namespace utils {

class IndexReader {
 public:
  explicit IndexReader(const std::string& name) {
    auto index_file_ = std::make_unique<file::File>(name, "r");
    double timestamp = 0;
    int64_t offset = 0;
    while (1) {
      if (index_file_->Read(&timestamp, sizeof(timestamp)) == 0) {
        break;
      }
      if (index_file_->Read(&offset, sizeof(offset)) == 0) {
        break;
      }
      timestamp_list_.push_back(timestamp);
      offset_list_.push_back(offset);
    }
  }
  virtual ~IndexReader() = default;

  double GetStartTime() { return timestamp_list_.front(); }

  double GetEndTime() { return timestamp_list_.back(); }

  int64_t GetOffsetByTime(double t) {
    int k = std::lower_bound(timestamp_list_.begin(), timestamp_list_.end(), t) -
            timestamp_list_.begin();
    if (k == timestamp_list_.size()) {
      return offset_list_.back();
    } else {
      return offset_list_[k];
    }
  }

 private:
  std::vector<double> timestamp_list_;
  std::vector<int64_t> offset_list_;
};

}  // namespace utils
