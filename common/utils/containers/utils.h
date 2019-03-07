// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <algorithm>
#include <array>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "glog/logging.h"

namespace utils {

template <typename T, int N>
static constexpr int array_size(const T (&)[N]) {
  return N;
}

template <typename T, uint64_t N>
static constexpr int array_size(const std::array<T, N>&) {
  return N;
}

template <typename T>
std::string ArrayToString(const T& array) {
  std::ostringstream str_out;
  str_out << "[";
  bool first = true;
  for (const auto& item : array) {
    if (first) {
      first = false;
    } else {
      str_out << ", ";
    }
    str_out << item;
  }
  str_out << "]";
  return str_out.str();
}

template <typename T>
std::string SetToString(const std::unordered_set<T>& set) {
  std::vector<T> array(set.begin(), set.end());
  std::sort(array.begin(), array.end());
  return ArrayToString(array);
}

// In a map, returns a const reference of an element's value using its key. If the key does not
// exist, then crash. It's nearly same as std::map::at() or std::unorderd_map::at(), except that
// at() method throws an exception, which can be relayed upwards on the call stack, so the true
// place that causes the exception is lost when the program crashes because of the uncaught
// exception.
// This version assumes the key is printable.
template <class Container>
const typename Container::value_type::second_type& FindOrDie(
    const Container& container, const typename Container::value_type::first_type& key) {
  const auto it = container.find(key);
  CHECK(it != container.end()) << "Map key not found: " << key;
  return it->second;
}

// Same as above, but returns a non-const reference.
template <class Container>
typename Container::value_type::second_type* FindOrDie(
    Container* container, const typename Container::value_type::first_type& key) {
  auto it = container->find(key);
  CHECK(it != container->end()) << "Map key not found: " << key;
  return &it->second;
}

// In a const map, returns a const pointer of an element's value using its key. If the key does not
// exist, then return nullptr.
template <class Container>
const typename Container::value_type::second_type* FindOrNull(
    const Container& container, const typename Container::value_type::first_type& key) {
  const auto it = container.find(key);
  return it == container.end() ? nullptr : &it->second;
}

// Same as above, but returns a non-const pointer.
template <class Container>
typename Container::value_type::second_type* FindOrNull(
    Container* container, const typename Container::value_type::first_type& key) {
  const auto it = container->find(key);
  return it == container->end() ? nullptr : &it->second;
}

}  // namespace utils
