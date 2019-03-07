// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <functional>
#include <iomanip>
#include <sstream>
#include <string>

#include <fmt/printf.h>
#include <glog/logging.h>

#include "common/utils/strings/utils.h"

namespace strings {

template <typename T>
class ValueWithPrecision {
 public:
  ValueWithPrecision(const T& value, int precision) : value_(value), precision_(precision) {}

 private:
  T value_;
  int precision_;

  template <typename T1>
  friend std::ostream& operator<<(std::ostream& os, const ValueWithPrecision<T1>& value);
};

template <typename T>
std::ostream& operator<<(std::ostream& os, const ValueWithPrecision<T>& value) {
  int old_precision = os.precision();
  return os << std::setprecision(value.precision_) << value.value_
            << std::setprecision(old_precision);
}

template <typename T>
ValueWithPrecision<T> WithPrecision(const T& value, int precision) {
  return ValueWithPrecision<T>(value, precision);
}

// Format a string with the specified arguments.
// This function will CHECK fail if the number of arguments expected by format string fmt does not
// match the number of arguments actually supplied.
// See documentation of {fmt} for format specifiers supported:
// http://fmtlib.net/4.0.0/syntax.html
template <typename... Args>
std::string Format(const char* fmt, const Args&... args) {
  try {
    return fmt::format(fmt, args...);
  } catch (const std::exception& e) {
    LOG(FATAL) << "Exception caught in strings::Format(): " << e.what();
    return "";
  } catch (...) {
    LOG(FATAL) << "Unknown exception caught in strings::Format().";
    return "";
  }
}

}  // namespace strings
