// Copyright @2017 Pony AI Inc. All rights reserved.
// Authors: acrush@pony.ai (Tiancheng Lou)
//          jiruyi@pony.ai (Ruyi Ji)

#pragma once

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

#include "glog/logging.h"

#include "common/utils/common/defines.h"

namespace hungarian {

class HungarianSparse {
 public:
  struct Edge {
    int row_id;
    int column_id;
    int64_t cost;
  };

  static int64_t MinCostMatching(int n, int m, const std::vector<Edge>& edges);

 private:
  static int64_t MinCostPerfectMatchingInternal(int n, int m, int* degrees, const Edge** edges[]);

  DISALLOW_COPY_MOVE_AND_ASSIGN(HungarianSparse);
};

}  // namespace hungarian
