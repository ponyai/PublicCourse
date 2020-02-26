#pragma once

#include "common/proto/map.pb.h"
#include "common/proto/simulation_config.pb.h"
#include "pnc/map/segment.h"

#include "common/utils/common/optional.h"
#include "common/utils/index/grid_index.h"

namespace pnc {
namespace map {

class MapMeta {
 public:
  explicit MapMeta(const interface::map::Map& map) : map_(map) {}

  void Initialize() {
    GetMinMaxValue();
    BuildCentralLineSegmentIndex();
    BuildTrafficLightIndex();
  }

  void BuildCentralLineSegmentIndex();
  void BuildTrafficLightIndex();
  void GetMinMaxValue();

  utils::Optional<Segment> GetNearestCentralLineSegment(double x, double y);
  utils::Optional<interface::map::TrafficLight> GetNearestTrafficLight(double x, double y);

  double min_x() { return min_x_; }
  double min_y() { return min_y_; }
  double max_x() { return max_x_; }
  double max_y() { return max_y_; }

 private:
  using CentralLineSegmentVector = std::vector<Segment>;
  using CentralLineSegmentIndex = utils::GridIndex<CentralLineSegmentVector>;
  std::unique_ptr<CentralLineSegmentIndex> central_line_segment_index_;

  using TrafficLightVector = std::vector<interface::map::TrafficLight>;
  using TrafficLightIndex = utils::GridIndex<TrafficLightVector>;
  std::unique_ptr<TrafficLightIndex> traffic_light_index_;

  interface::map::Map map_;

  double min_x_ = std::numeric_limits<double>::max();
  double min_y_ = std::numeric_limits<double>::max();
  double max_x_ = std::numeric_limits<double>::min();
  double max_y_ = std::numeric_limits<double>::min();
};
};
}  // namespace pnc
