// Copyright @2018 Pony AI Inc. All rights reserved.
#include "common/utils/math/vec2d.h"

namespace math {

const Eigen::Matrix2Xd ToMatrix(const utils::ConstArrayView<Vec2d>& points) {
  CHECK_GT(points.size(), 0);
  // We need to check sizeof(Vec2) == sizeof(T) * 2,
  // if we may re-implement this function using template.

  double* data = const_cast<double*>(reinterpret_cast<const double*>(points.data()));
  constexpr int kNumRows = 2;
  const int num_columns = points.size();

  using MatrixMap = Eigen::Map<Eigen::Matrix2Xd, 0, Eigen::Stride<2, 1>>;
  return MatrixMap(data, kNumRows, num_columns);
}

}  // namespace math
