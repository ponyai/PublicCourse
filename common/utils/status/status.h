// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <string>

#include <glog/logging.h>

#include "common/proto/status.pb.h"
#include "common/utils/common/defines.h"

#define RETURN_IF_ERROR(statement)                                                 \
  do {                                                                             \
    /* Using _status below to avoid capture problems if statement is "status". */  \
    ::utils::Status _status = (statement);                                         \
    if (PREDICT_FALSE(!_status.ok())) {                                            \
      _status.AppendTrace(std::string(__FILE__) + ":" + std::to_string(__LINE__)); \
      return _status;                                                              \
    }                                                                              \
  } while (0)

#define CHECK_OK(statement) CHECK_EQ(::utils::Status::OK, (statement))
#define QCHECK_OK(statement) QCHECK_EQ(::utils::Status::OK, (statement))

namespace utils {

#if defined(__clang__)
// Only clang supports warn_unused_result as a type annotation.
class MUST_USE_RESULT Status;
#endif

using error = interface::status::ErrorCode;

// Status objects are used to propagate errors rather than using C++ exceptions.
class Status {
 public:
  using ErrorCode = interface::status::ErrorCode;

  // Create a "successful" status.
  Status() : code_(ErrorCode::OK) {}

  // Create a status in the canonical error space with the specified code and error message.
  // If "code == OK", error_message is ignored and a Status object identical to Status::OK is
  // constructed.
  Status(error code, const std::string& error_message) : code_(code) {
    if (code != ErrorCode::OK) {
      error_message_ = error_message;
    }
  }

  static Status OK;

  static Status Error(const std::string& error_message) {
    return Status(ErrorCode::UNKNOWN, error_message);
  }

  // Equality operator.
  bool operator==(const Status& status) const {
    return code_ == status.code_ && error_message_ == status.error_message_;
  }

  // Inequality operator.
  bool operator!=(const Status& status) const {
    return code_ != status.code_ || error_message_ != status.error_message_;
  }

  // Determine if the status is ok.
  bool ok() const { return code_ == ErrorCode::OK; }

  void AppendTrace(const std::string& new_line) {
    trace_ += new_line;
    trace_ += '\n';
  }

  const std::string& error_message() const { return error_message_; }

  ErrorCode error_code() const { return code_; }

  std::string ToString() const;

 private:
  ErrorCode code_;
  std::string error_message_;

  // RETURN_IF_ERROR will append the filename and line number of the macro callsite in this member
  // variable.
  std::string trace_;
};

std::ostream& operator<<(std::ostream& os, const Status& status);

}  // namespace utils
