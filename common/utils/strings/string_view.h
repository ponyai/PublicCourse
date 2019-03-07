// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <algorithm>
#include <string>

#include <glog/logging.h>

namespace strings {

// A mimic of std::string_view in C++17. StringView is a trivially-copyable memory view of (part of)
// a string. Since copying StringView is so trivial, move is not recommended to use. StringView does
// NOT take ownership of the referenced string. Caller has responsibility to make sure the
// referenced string is alive before the StringView is destroyed.
class StringView {
 public:
  static constexpr int kEnd = -1;

  // Takes a substring of s, starting from the 'pos'-th character, and 'size' characters.
  // size = kEnd means take substring from 'pos' till the end of 's'.
  static StringView Substring(StringView s, int pos, int size = kEnd);

  StringView(const char* data, int size);
  StringView(const char* s);         // NOLINT(runtime/explicit)
  StringView(const std::string& s);  // NOLINT(runtime/explicit)
  ~StringView() = default;

  operator std::string() const;

  const char* data() const { return data_; }
  int size() const { return size_; }
  int length() const { return size_; }

  const char* begin() const { return data_; }
  const char* end() const { return data_ + size_; }

  // Compares self and 's'. Returns:
  //  -1: if it is less than s.
  //   0: if it matches s.
  //  +1: if it is greater than s.
  int Compare(StringView s) const;

  // Takes a substring starting from the 'pos'-th character, and 'size' characters.
  // size = kEnd means take substring from 'pos' till the end.
  StringView Substring(int pos, int size = kEnd) const;

  // Searches the value for the first occurrence in the string.
  // When start is specified, the search only includes characters at or after position start.
  int Find(char value, int start = 0) const;
  int Find(StringView value, int start = 0) const;

 private:
  const char* data_ = nullptr;
  int size_ = 0;

  // Shallow copy is OK.
};

inline bool operator==(StringView left, StringView right) {
  return left.size() == right.size() && strncmp(left.data(), right.data(), left.size()) == 0;
}

// =================================== Implementation ===================================

inline StringView StringView::Substring(StringView s, int pos, int size) {
  return s.Substring(pos, size);
}

inline StringView::StringView(const char* data, int size)
    : data_(DCHECK_NOTNULL(data)), size_(size) {
  DCHECK_GE(size, 0);
}

inline StringView::StringView(const char* s) : StringView(DCHECK_NOTNULL(s), strlen(s)) {}

inline StringView::StringView(const std::string& s) : StringView(s.data(), s.size()) {}

inline StringView::operator std::string() const { return std::string(data_, size_); }

inline int StringView::Compare(StringView s) const {
  int min_size = std::min(size_, s.size_);
  int result = memcmp(data_, s.data_, min_size * sizeof(char));
  if (result != 0) {
    return result;
  }

  if (size_ > min_size) {
    return 1;
  }
  if (s.size_ > min_size) {
    return -1;
  }
  return 0;
}

inline StringView StringView::Substring(int pos, int size) const {
  DCHECK(pos >= 0 && pos <= size_);
  if (size == kEnd) {
    return StringView(data_ + pos, size_ - pos);
  }

  DCHECK(size >= 0 && pos + size <= size_);
  return StringView(data_ + pos, size);
}

inline int StringView::Find(char value, int start) const {
  DCHECK(start >= 0 && start <= size_);
  if (start >= size_) {
    return kEnd;
  }
  auto it = std::find(data_ + start, data_ + size_, value);
  return it == data_ + size_ ? kEnd : static_cast<int>(it - data_);
}

inline int StringView::Find(StringView value, int start) const {
  DCHECK(start >= 0 && start <= size_);
  if (value.size() == 0) {
    return start;
  }
  if (value.size() > size_) {
    return kEnd;
  }
  for (int pos = start; pos <= size_ - value.size(); ++pos) {
    if (memcmp(data_ + pos, value.data(), value.size() * sizeof(char)) == 0) {
      return pos;
    }
  }
  return kEnd;
}

}  // namespace strings
