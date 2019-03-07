// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <stdint.h>
#include <string.h>

#include <string>
#include <vector>

namespace grading {

void Eval(double overlap, double s1, double s2,
          double* precision, double* recall, double* jaccard);

template<class ListType1, class ListType2>
int GetOverlapSizeOfSortedLists(const ListType1& a, const ListType2& b) {
  int result = 0;
  auto pa = a.begin();
  auto pb = b.begin();
  while (pa != a.end() && pb != b.end()) {
    if (*pa == *pb) {
      ++result;
      ++pa;
      ++pb;
    } else if (*pa < *pb) {
      ++pa;
    } else {
      ++pb;
    }
  }
  return result;
}

}  // namespace grading
