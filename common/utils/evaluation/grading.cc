// Copyright @2018 Pony AI Inc. All rights reserved.

#include "common/utils/evaluation/grading.h"

#include <limits>

namespace grading {
namespace {

const double kEpsilon = 1e-7;

}  // namespace

void Eval(double overlap, double s1, double s2,
          double* precision, double* recall, double* jaccard) {
  if (precision != nullptr) {
    *precision = (overlap <= kEpsilon ? 0.0 : overlap / s2);
  }
  if (recall != nullptr) {
    *recall = (overlap <= kEpsilon ? 0.0 : overlap / s1);
  }
  if (jaccard != nullptr) {
    *jaccard = (overlap <= kEpsilon ? 0.0 : overlap / (s1 + s2 - overlap));
  }
}

}  // namespace grading
