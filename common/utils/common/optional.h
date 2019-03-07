// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <boost/optional.hpp>

namespace utils {

template <typename T>
using Optional = boost::optional<T>;

using None = boost::none_t;

const None none{};

// Returns Optional<T>(v)
template <class T>
inline Optional<T> make_optional(T const& v) {
  return Optional<T>(v);
}

// Returns Optional<T>(cond,v)
template <class T>
inline Optional<T> make_optional(bool cond, T const& v) {
  return Optional<T>(cond, v);
}

}  // namespace utils
