// Copyright @2018 Pony AI Inc. All rights reserved.
//
#include "common/utils/math/vec3d.h"

namespace math {

const Eigen::Matrix3Xd ToMatrix(const utils::ConstArrayView<Vec3d>& points) {
  CHECK_GT(points.size(), 0);
  // We need to check sizeof(Vec3) == sizeof(T) * 3,
  // if we may re-implement this function using template.

  double* data = const_cast<double*>(reinterpret_cast<const double*>(points.data()));
  constexpr int kNumRows = 3;
  const int num_columns = points.size();

  using MatrixMap = Eigen::Map<Eigen::Matrix3Xd, 0, Eigen::Stride<3, 1>>;
  return MatrixMap(data, kNumRows, num_columns);
}

}  // namespace math
