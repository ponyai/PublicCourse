// Copyright @2018 Pony AI Inc. All rights reserved.

#include "common/utils/gl/geometry_utils.h"

namespace gl {
namespace geometry_utils {

bool IsZero(double x) { return (x > 0 ? x : -x) < kUtilsEpsilon; }

double TriangleArea(const math::Vec3d& a, const math::Vec3d& b, const math::Vec3d& c) {
  return (a - b).OuterProd(a - c).Length();
}

// Check whether the given point is in the triangle. Please make sure that
// there're no 3 points in
// the same line first.
bool IsPointInTriangle(const math::Vec3d& p, const math::Vec3d& s1, const math::Vec3d& s2,
                       const math::Vec3d& s3) {
  return IsZero(TriangleArea(s1, s2, s3) - TriangleArea(p, s1, s2) - TriangleArea(p, s2, s3) -
                TriangleArea(p, s3, s1));
}

// OpenGL doesn't draw polygon perfectly, we triangulate it first. This
// functions can only handle
// simple polygons which means the input SHOULDN'T be self-intersecting. You can
// find more details
// on the triangulation algorithm here:
//    https://en.wikipedia.org/wiki/Polygon_triangulation#Ear_clipping_method
//
// Noting that this is a simple yet timetime-consuming implementation( O(V^3) ),
// NEVER run it on
// polygons who have more than 100 vertices.
void EarClippingTriangulation(const utils::ConstArrayView<math::Vec3d>& points,
                              std::vector<int>* triangles_indices_ptr) {
  CHECK_NOTNULL(triangles_indices_ptr);
  triangles_indices_ptr->clear();
  std::vector<int> points_buffer[2];
  for (int i = 0; i < points.size(); ++i) {
    points_buffer[1].push_back(i);
  }
  // There're at most N ears.
  for (int step = 0; step < points.size(); ++step) {
    const int target = step & 1;
    const int source = !target;
    bool ear_to_cut = -1;
    const int n = points_buffer[source].size();
    if (n <= 3) {
      for (int i = 0; i < n; ++i) {
        triangles_indices_ptr->push_back(points_buffer[source][i]);
      }
      break;
    }
    for (int i = 0; i < n; ++i) {
      bool no_other_points_inside = true;
      const int prev = (i + n - 1) % n;
      const int next = (i + 1) % n;
      const math::Vec3d& s1 = points[points_buffer[source][prev]];
      const math::Vec3d& s2 = points[points_buffer[source][i]];
      const math::Vec3d& s3 = points[points_buffer[source][next]];
      for (int j = 0; j < n; ++j) {
        if (j != i && j != prev && j != next &&
            IsPointInTriangle(points[points_buffer[source][j]], s1, s2, s3)) {
          no_other_points_inside = false;
          break;
        }
      }
      // Found 'ear': (p_prev, p_i, p_next)
      if (no_other_points_inside) {
        triangles_indices_ptr->push_back(points_buffer[source][prev]);
        triangles_indices_ptr->push_back(points_buffer[source][i]);
        triangles_indices_ptr->push_back(points_buffer[source][next]);
        ear_to_cut = i;
        break;
      }
    }
    points_buffer[target].clear();
    for (int i = 0; i < n; ++i) {
      if (i != ear_to_cut) {
        points_buffer[target].push_back(points_buffer[source][i]);
      }
    }
  }
}

}  // namespace geometry_utils
}  // namespace gl
