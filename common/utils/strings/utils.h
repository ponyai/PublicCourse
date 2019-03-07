// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <algorithm>
#include <memory>
#include <string>

#include "common/utils/strings/string_view.h"

namespace strings {

bool StartWith(StringView full_string, StringView search_string);
bool EndWith(StringView full_string, StringView search_string);
std::string ToLower(StringView s);

// A safer version of strcpy. Copies the null-terminated string src to dst. At most (dst_size - 1)
// characters would be copied from src to dst. A null character ('\0') would always be in the end
// of the copying result. Returns number of the copied characters, not including the final '\0'.
//
// This function is part of the standard C library on many UNIX platforms, including BSD.
// Unfortunately it is not shipped with glibc, so we implement it by ourselves.
int64_t strlcpy(char* dst, const char* src, int64_t dst_size);

std::unique_ptr<char[]> ToCharPtr(StringView s);

// Split a string into multiple substrings based on given delimiter.
// Argument allow_empty_tokens is to determine whether empty strings are included.
std::vector<std::string> Split(StringView s, char delimiter, bool allow_empty_tokens = false);

// Similar as above, but the delimiter is also a string. Delimiter is not allowed to be empty.
// Typical examples:
//  * Split("one unit test", " ") = {"one", "unit, "test"};
//  * Split(" a  b c ", " ") = {"a", "b", "c"};
//  * Split(" a  b c ", " ", true) = {"", "a", "", "b", "c", ""};
//
// Note: delimiter is searched for from the left to the right of 's'. Whenever one occurrence is
// found, the string 's' gets split there *immediately*, before searching for the next occurrence.
// As a result, in some special cases, only the first one of a pair of consecutive occurrences will
// be found. Example:
//  * Split("baaabbaac", "aa") = {"b", "abb", "c"}.
// "aaa" can match "aa" twice, but after the matching the first occurrence, the remaining string
// becomes "abbaacc", therefore the second occurrence would not be found.
std::vector<std::string> Split(StringView s, StringView delimiter, bool allow_empty_tokens = false);

// Replace all the search string with replace string in subject.
// Return the count of replacement.
// NOTE: "search" param should not be empty, otherwise will check failed.
int Replace(const std::string& search, const std::string& replace, std::string* subject);

// Concatenate strings in s into returned string, separated by delimiter.
// Current implementation will ignore empty strings in s. For example:
//   Join({"a", "b", "c"}, ',') = "a,b,c"
//   Join({"a", "", "c"}, ',') = "a,c"
//   Join({"a", "", ""}, ',') = "a"
std::string Join(const std::vector<std::string>& s, char delimiter);

// =================================== Implementation ===================================

namespace internal {

template <class OutStream, class DelimType>
inline void Split(StringView s, DelimType delimeter, int delimeter_length, bool allow_empty_tokens,
                  OutStream out) {
  CHECK_GT(delimeter_length, 0);
  int start_pos = 0;
  int end_pos;
  while (start_pos < s.length() && (end_pos = s.Find(delimeter, start_pos)) != StringView::kEnd) {
    if (allow_empty_tokens || start_pos != end_pos) {
      *out = s.Substring(start_pos, end_pos - start_pos);
    }
    start_pos = end_pos + delimeter_length;
  }
  if (allow_empty_tokens || start_pos != s.length()) {
    *out = s.Substring(start_pos);
  }
}

}  // namespace internal

inline bool StartWith(StringView full_string, StringView search_string) {
  return full_string.size() >= search_string.size() &&
         full_string.Substring(0, search_string.size()) == search_string;
}

inline bool EndWith(StringView full_string, StringView search_string) {
  return full_string.size() >= search_string.size() &&
         full_string.Substring(full_string.size() - search_string.size()) == search_string;
}

inline std::string ToLower(StringView s) {
  std::string result;
  result.resize(s.size());
  std::transform(s.begin(), s.end(), result.begin(), ::tolower);
  return result;
}

inline int64_t strlcpy(char* dst, const char* src, int64_t dst_size) {
  CHECK_GE(dst_size, 1)
      << "dst_size should be at least 1. It should include the terminating null character.";
  int64_t i = 0;
  for (; i < dst_size - 1 && src[i] != '\0'; ++i) {
    dst[i] = src[i];
  }
  dst[i] = '\0';
  return i;
}

inline std::unique_ptr<char[]> ToCharPtr(StringView s) {
  auto result = std::make_unique<char[]>(s.size() + 1);
  strlcpy(result.get(), s.data(), s.size() + 1);
  return result;
}

inline std::vector<std::string> Split(StringView s, char delimiter, bool allow_empty_tokens) {
  std::vector<std::string> tokens;
  internal::Split(s, delimiter, 1, allow_empty_tokens, std::back_inserter(tokens));
  return tokens;
}

inline std::vector<std::string> Split(StringView s, StringView delimiter, bool allow_empty_tokens) {
  CHECK_GT(delimiter.size(), 0);
  std::vector<std::string> tokens;
  internal::Split(s, delimiter, delimiter.size(), allow_empty_tokens, std::back_inserter(tokens));
  return tokens;
}

inline int Replace(const std::string& search, const std::string& replace, std::string* subject) {
  CHECK(subject != nullptr);
  CHECK(!search.empty()) << "'search' string shouldn't be empty.";

  int count = 0;
  int start_pos = 0;
  std::string result;
  while (start_pos < subject->size()) {
    size_t found = subject->find(search, start_pos);
    if (found == std::string::npos) {
      result += subject->substr(start_pos);
      break;
    }
    result += subject->substr(start_pos, found - start_pos);
    result += replace;
    start_pos = found + search.size();
    ++count;
  }
  subject->swap(result);
  return count;
}

inline std::string Join(const std::vector<std::string>& strs, char delimiter) {
  std::string result;
  for (const auto& s : strs) {
    if (!result.empty() && !s.empty()) {
      result += delimiter;
    }
    result += s;
  }
  return result;
}

}  // namespace strings
