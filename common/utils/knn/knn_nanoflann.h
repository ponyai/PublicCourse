// Copyright @2017 Pony AI Inc. All rights reserved.
// Authors: bocongl@pony.ai (Bocong Liu)

#pragma once

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif  // __clang__

#include <KDTreeVectorOfVectorsAdaptor.h>
#include <nanoflann.hpp>

#ifdef __clang__
#pragma clang diagnostic pop
#endif  // __clang__

#include "common/utils/common/defines.h"
#include "common/utils/knn/knn_interface.h"

namespace utils {

namespace NanoflannAdaptor {

template <typename FloatType>
using EigenMatrix =
    nanoflann::KDTreeEigenMatrixAdaptor<Eigen::Matrix<FloatType, Eigen::Dynamic, Eigen::Dynamic>>;
template <typename FloatType>
using VectorOfVector = nanoflann::KDTreeVectorOfVectorsAdaptor<
    std::vector<Eigen::Matrix<FloatType, Eigen::Dynamic, 1>,
                Eigen::aligned_allocator<Eigen::Matrix<FloatType, Eigen::Dynamic, 1>>>,
    FloatType>;
}  // namespace NanoflannAdaptor

template <typename KdTreeAdaptor, typename FloatType>
class KnnNanoflann : public KnnInterface<FloatType> {
 public:
  using typename KnnInterface<FloatType>::MatrixType;
  using typename KnnInterface<FloatType>::VectorType;
  using typename KnnInterface<FloatType>::VectorOfVectorType;

  explicit KnnNanoflann(const MatrixType* points);

  explicit KnnNanoflann(const VectorOfVectorType* points);

  explicit KnnNanoflann(const VectorOfVectorType* points, int point_dim);

  KnnNanoflann(const MatrixType* points, int leaf_size);

  ~KnnNanoflann() override = default;

  int Search(const VectorType& query_point,
             int k,
             std::vector<int>* k_indices,
             std::vector<FloatType>* k_sqr_distances) const override;

  int RadiusSearch(const VectorType& query_point,
                   FloatType radius,
                   std::vector<int>* k_indices,
                   std::vector<FloatType>* k_sqr_distances) const override;

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

 private:
  const nanoflann::SearchParams default_search_params_ = nanoflann::SearchParams(10);

  const MatrixType* mtype_points_ = nullptr;
  const VectorOfVectorType* vtype_points_ = nullptr;
  const int num_points_;
  const int point_dim_;
  int leaf_size_ = 20;
  std::unique_ptr<KdTreeAdaptor> kdtree_;

  DISALLOW_COPY_MOVE_AND_ASSIGN(KnnNanoflann);
};

template <typename KdTreeAdaptor, typename FloatType>
KnnNanoflann<KdTreeAdaptor, FloatType>::KnnNanoflann(const MatrixType* points)
    : mtype_points_(CHECK_NOTNULL(points)),
      num_points_(mtype_points_->cols()),
      point_dim_(mtype_points_->rows()) {
  kdtree_ = std::make_unique<KdTreeAdaptor>(point_dim_, *mtype_points_);
  kdtree_->index->buildIndex();
}
template <typename KdTreeAdaptor, typename FloatType>
KnnNanoflann<KdTreeAdaptor, FloatType>::KnnNanoflann(const VectorOfVectorType* points)
    : KnnNanoflann(points, (*CHECK_NOTNULL(points))[0].size()) {}

template <typename KdTreeAdaptor, typename FloatType>
KnnNanoflann<KdTreeAdaptor, FloatType>::KnnNanoflann(const VectorOfVectorType* points,
                                                     int point_dim)
    : vtype_points_(CHECK_NOTNULL(points)),
      num_points_(vtype_points_->size()),
      point_dim_(point_dim) {
  if (num_points_) {
    kdtree_ = std::make_unique<KdTreeAdaptor>(point_dim_, *vtype_points_);
    kdtree_->index->buildIndex();
  }
}

template <typename KdTreeAdaptor, typename FloatType>
KnnNanoflann<KdTreeAdaptor, FloatType>::KnnNanoflann(const MatrixType* points, int leaf_size)
    : mtype_points_(CHECK_NOTNULL(points)),
      num_points_(mtype_points_->cols()),
      point_dim_(mtype_points_->rows()),
      leaf_size_(leaf_size) {
  kdtree_ = std::make_unique<KdTreeAdaptor>(point_dim_, *mtype_points_, leaf_size);
  kdtree_->index->buildIndex();
}

template <typename KdTreeAdaptor, typename FloatType>
int KnnNanoflann<KdTreeAdaptor, FloatType>::Search(const VectorType& query_point,
                                                   int k,
                                                   std::vector<int>* k_indices,
                                                   std::vector<FloatType>* k_sqr_distances) const {
  CHECK_GT(k, 0);
  CHECK_EQ(point_dim_, query_point.rows());

  if (!kdtree_) {
    if (k_indices) {
      k_indices->clear();
    }
    if (k_sqr_distances) {
      k_sqr_distances->clear();
    }
    return 0;
  }

  k = std::min(k, num_points_);
  std::vector<int> indices(k);
  std::vector<FloatType> sqr_distances(k);
  nanoflann::KNNResultSet<FloatType, int> result(k);
  result.init(&indices[0], &sqr_distances[0]);
  kdtree_->index->findNeighbors(result, query_point.data(), default_search_params_);

  if (k_indices) {
    k_indices->swap(indices);
  }
  if (k_sqr_distances) {
    k_sqr_distances->swap(sqr_distances);
  }
  return result.size();
}

template <typename KdTreeAdaptor, typename FloatType>
int KnnNanoflann<KdTreeAdaptor, FloatType>::RadiusSearch(
    const VectorType& query_point,
    FloatType radius,
    std::vector<int>* k_indices,
    std::vector<FloatType>* k_sqr_distances) const {
  CHECK_EQ(point_dim_, query_point.rows());

  if (k_indices) {
    k_indices->clear();
  }
  if (k_sqr_distances) {
    k_sqr_distances->clear();
  }
  if (!kdtree_) {
    return 0;
  }

  std::vector<std::pair<int, FloatType>> indices_sqrdists;
  // NanoFlann needs radius sqr as the input.
  nanoflann::RadiusResultSet<FloatType, int> results(radius * radius, indices_sqrdists);
  int num_found = kdtree_->index->radiusSearchCustomCallback(
      query_point.data(), results, default_search_params_);

  std::sort(indices_sqrdists.begin(),
            indices_sqrdists.end(),
            [](const std::pair<int, FloatType>& a, const std::pair<int, FloatType>& b) {
              return a.second < b.second;
            });

  if (k_indices) {
    k_indices->resize(num_found);
  }
  if (k_sqr_distances) {
    k_sqr_distances->resize(num_found);
  }
  for (int i = 0; i < num_found; ++i) {
    if (k_indices) {
      (*k_indices)[i] = results.m_indices_dists[i].first;
    }
    if (k_sqr_distances) {
      (*k_sqr_distances)[i] = results.m_indices_dists[i].second;
    }
  }
  return num_found;
}

}  // namespace utils
