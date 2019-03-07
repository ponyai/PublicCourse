// Copyright @2017 Pony AI Inc. All rights reserved.
// Authors: bocongl@pony.ai (Bocong Liu)

#pragma once

#include <algorithm>
#include <vector>

#include "Eigen/Core"
#include "glog/logging.h"

namespace utils {

// Input matrix follows Eigen's column major convention. The column num of input matrix
// represents the total number of samples. The row num stands for the dimension of each
// sample. Note, points need to be valid during the lifetime of KNN. Only Euclidean
// distance is supported now.
template <typename FloatType>
class KnnInterface {
 public:
  using MatrixType = Eigen::Matrix<FloatType, Eigen::Dynamic, Eigen::Dynamic>;
  using VectorType = Eigen::Matrix<FloatType, Eigen::Dynamic, 1>;
  using VectorOfVectorType = std::vector<VectorType, Eigen::aligned_allocator<VectorType>>;

  KnnInterface() = default;

  virtual ~KnnInterface() = default;

  virtual int Search(const VectorType& query_point,
                     int k,
                     std::vector<int>* k_indices,
                     std::vector<FloatType>* k_sqr_distances) const = 0;

  virtual int RadiusSearch(const VectorType& query_point,
                           FloatType radius,
                           std::vector<int>* k_indices,
                           std::vector<FloatType>* k_sqr_distances) const = 0;
};

}  // namespace utils
