// Copyright @2017 Pony AI Inc. All rights reserved.
// Authors: acrush@pony.ai (Tiancheng Lou)

#include "common/utils/hungarian/hungarian_sparse.h"

#include <vector>

#include "common/utils/containers/fixed_array.h"

using utils::FixedArray;

namespace hungarian {

int64_t HungarianSparse::MinCostMatching(int n, int m, const std::vector<Edge>& input_edges) {
  if (n == 0 || m == 0) {
    return 0;
  }
  CHECK_GT(n, 0);
  CHECK_GT(m, 0);
  FixedArray<int> degrees(n);
  std::fill(degrees.begin(), degrees.end(), 1);
  for (const auto& edge : input_edges) {
    CHECK(edge.row_id >= 0 && edge.row_id < n && edge.column_id >= 0 && edge.column_id < m);
    if (edge.cost < 0) {
      ++degrees[edge.row_id];
    }
  }
  FixedArray<const Edge*> edge_buffer(n + static_cast<int>(input_edges.size()));
  FixedArray<const Edge**> edges(n);
  int buffer_offset = 0;
  for (int i = 0; i < n; ++i) {
    edges[i] = edge_buffer.data() + buffer_offset;
    buffer_offset += degrees[i];
  }

  FixedArray<Edge> redundent_edges(n);
  for (int i = 0; i < n; ++i) {
    redundent_edges[i].row_id = i;
    redundent_edges[i].column_id = i + m;
    redundent_edges[i].cost = 0;
    edges[i][0] = &redundent_edges[i];
    degrees[i] = 1;
  }
  for (const auto& edge : input_edges) {
    if (edge.cost < 0) {
      edges[edge.row_id][degrees[edge.row_id]++] = &edge;
    }
  }
  return MinCostPerfectMatchingInternal(n, n + m, degrees.data(), edges.data());
}

int64_t HungarianSparse::MinCostPerfectMatchingInternal(
    int n, int m, int* degrees, const Edge** edges[]) {
  CHECK(degrees != nullptr);
  CHECK(edges != nullptr);

  if (n == 0 || m == 0) {
    return 0;
  }
  CHECK_GT(n, 0);
  CHECK_GT(m, 0);

  FixedArray<int64_t> row_factor(n);
  for (int i = 0; i < n; i++) {
    CHECK_GT(degrees[i], 0);
    row_factor[i] = std::numeric_limits<int64_t>::max();
    for (int j = 0; j < degrees[i]; ++j) {
      row_factor[i] = std::min(row_factor[i], edges[i][j]->cost);
    }
  }
  FixedArray<int64_t> col_factor(m);
  std::fill(col_factor.begin(), col_factor.end(), 0);

  FixedArray<int> row_to_col(n);
  FixedArray<int> col_to_row(m);
  std::fill(row_to_col.begin(), row_to_col.end(), -1);
  std::fill(col_to_row.begin(), col_to_row.end(), -1);

  FixedArray<int> queue(n);
  FixedArray<int> previous(n);
  FixedArray<bool> visited_rows(n);
  FixedArray<bool> visited_cols(m);
  FixedArray<int64_t> row_factor_delta(n);

  for (int s = 0; s < n; s++) {
    std::fill(visited_rows.begin(), visited_rows.end(), false);
    std::fill(visited_cols.begin(), visited_cols.end(), false);
    int queue_size = 0;
    queue[queue_size++] = s;
    previous[s] = -1;
    visited_rows[s] = true;
    std::fill(row_factor_delta.begin(), row_factor_delta.end(), 0);
    bool find_argument_path = false;
    while (1) {
      CHECK(visited_rows[s]);
      int64_t min_row_factor_delta = std::numeric_limits<int64_t>::max();
      for (int i = 0; i < n; i++) {
        if (visited_rows[i] && row_factor_delta[i] < min_row_factor_delta) {
          min_row_factor_delta = row_factor_delta[i];
        }
      }
      if (min_row_factor_delta > 0) {
        for (int cl = 0; cl < queue_size; cl++) {
          row_factor_delta[queue[cl]] -= min_row_factor_delta;
        }
        for (int i = 0; i < n; i++) {
          if (visited_rows[i]) {
            row_factor_delta[i] -= min_row_factor_delta;
            row_factor[i] += min_row_factor_delta;
          }
        }
        for (int i = 0; i < m; i++) {
          if (visited_cols[i]) {
            col_factor[i] -= min_row_factor_delta;
          }
        }
      }
      for (int cl = 0; cl < queue_size; cl++) {
        int row = queue[cl];
        if (row_factor_delta[row] > 0) {
          continue;
        }
        row_factor_delta[row] = std::numeric_limits<int64_t>::max();
        for (int j = 0; j < degrees[row]; ++j) {
          const Edge* edge = edges[row][j];
          int p = edge->column_id;
          if (visited_cols[p]) {
            continue;
          }
          int64_t delta = edge->cost - row_factor[row] - col_factor[p];
          if (delta <= 0) {
            visited_cols[p] = true;
            const int next_row = col_to_row[p];
            if (next_row < 0) {
              do {
                const int previous_col = row_to_col[row];
                row_to_col[row] = p;
                col_to_row[p] = row;
                row = previous[row];
                p = previous_col;
              } while (row >= 0);
              find_argument_path = true;
              break;
            }
            previous[next_row] = row;
            visited_rows[next_row] = true;
            queue[queue_size++] = next_row;
          } else if (delta < row_factor_delta[row]) {
            row_factor_delta[row] = delta;
          }
        }
        if (find_argument_path) {
          break;
        }
        CHECK_NE(row_factor_delta[row], std::numeric_limits<int64_t>::max());
      }
      if (find_argument_path) {
        break;
      }
    }
  }

  int64_t total_cost = 0;
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < degrees[i]; ++j) {
      if (edges[i][j]->column_id == row_to_col[i]) {
        total_cost += edges[i][j]->cost;
      }
    }
  }
  return total_cost;
}

}  // namespace hungarian
