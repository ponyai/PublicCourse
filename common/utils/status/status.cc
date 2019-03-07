// Copyright @2018 Pony AI Inc. All rights reserved.

#include "common/utils/status/status.h"

namespace utils {

Status Status::OK = Status();

std::string Status::ToString() const {
  std::string result;
  if (error_message_.empty()) {
    result = interface::status::ErrorCode_Name(code_);
  } else {
    result = interface::status::ErrorCode_Name(code_) + ": " + error_message_;
  }

  if (!trace_.empty()) {
    result += "\nThis status was forwarded by RETURN_IF_ERROR. Trace: \n";
    result += trace_;
  }
  return result;
}

std::ostream& operator<<(std::ostream& os, const Status& status) {
  os << status.ToString();
  return os;
}

}  // namespace utils
