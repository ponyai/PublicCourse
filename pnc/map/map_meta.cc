#include "pnc/map/map_meta.h"

namespace pnc {
namespace map {

using interface::map::TrafficLight;
using utils::Optional;
using utils::none;

namespace {

void UpdateMinMaxValue(double& min_x_, double& max_x_, double& min_y_, double& max_y_, double x,
                       double y) {
  min_x_ = std::min(x, min_x_);
  max_x_ = std::max(x, max_x_);
  min_y_ = std::min(y, min_y_);
  max_y_ = std::max(y, max_y_);
}
}

void MapMeta::BuildCentralLineSegmentIndex() {
  double min_x = min_x_ - 10.0;
  double min_y = min_y_ - 10.0;
  double max_x = max_x_ + 10.0;
  double max_y = max_y_ + 10.0;
  double dimension = 10;
  central_line_segment_index_ =
      std::make_unique<CentralLineSegmentIndex>(min_x, min_y, max_x, max_y, dimension);
  for (const auto& lane : map_.lane()) {
    const auto& central_line = lane.central_line();
    for (int i = 0; i + 1 < central_line.point_size(); i++) {
      math::Vec2d current_point(central_line.point(i).x(), central_line.point(i).y());
      math::Vec2d next_point(central_line.point(i + 1).x(), central_line.point(i + 1).y());
      central_line_segment_index_->GetMutable(current_point.x, current_point.y)
          ->push_back(Segment(current_point, next_point));
    }
  }
}

void MapMeta::BuildTrafficLightIndex() {
  double min_x = min_x_ - 10.0;
  double min_y = min_y_ - 10.0;
  double max_x = max_x_ + 10.0;
  double max_y = max_y_ + 10.0;
  double dimension = 10;
  traffic_light_index_ = std::make_unique<TrafficLightIndex>(min_x, min_y, max_x, max_y, dimension);
  for (const auto& traffic_light : map_.traffic_light()) {
    const auto& stop_line = traffic_light.stop_line();
    CHECK(stop_line.point_size() == 2);
    math::Vec2d start_point(stop_line.point(0).x(), stop_line.point(0).y());
    math::Vec2d end_point(stop_line.point(1).x(), stop_line.point(1).y());
    math::Vec2d mid_point =
        math::Vec2d((start_point.x + end_point.x) / 2, (start_point.y + end_point.y) / 2);
    traffic_light_index_->GetMutable(mid_point.x, mid_point.y)->push_back(traffic_light);
  }
}

void MapMeta::GetMinMaxValue() {
  for (const auto& lane : map_.lane()) {
    for (const auto& point : lane.central_line().point()) {
      UpdateMinMaxValue(min_x_, max_x_, min_y_, max_y_, point.x(), point.y());
    }
    for (const auto& point : lane.left_bound().boundary().point()) {
      UpdateMinMaxValue(min_x_, max_x_, min_y_, max_y_, point.x(), point.y());
    }
    for (const auto& point : lane.right_bound().boundary().point()) {
      UpdateMinMaxValue(min_x_, max_x_, min_y_, max_y_, point.x(), point.y());
    }
  }
}

Optional<Segment> MapMeta::GetNearestCentralLineSegment(double x, double y) {
  std::vector<const CentralLineSegmentVector*> vec_segments =
      central_line_segment_index_->GetAllInCircle(x, y, 5.0);

  Optional<Segment> result = utils::none;
  double min_dis = std::numeric_limits<double>::max();

  for (const CentralLineSegmentVector* v : vec_segments) {
    for (const pnc::map::Segment& seg : *v) {
      double dis = seg.DistanceToPoint(math::Vec2d(x, y));
      if (dis < min_dis) {
        min_dis = dis;
        result = seg;
      }
    }
  }
  return result;
}

Optional<TrafficLight> MapMeta::GetNearestTrafficLight(double x, double y) {
  std::vector<const TrafficLightVector*> vec_segments =
      traffic_light_index_->GetAllInCircle(x, y, 5.0);
  Optional<TrafficLight> result = utils::none;
  double min_dis = std::numeric_limits<double>::max();
  for (const TrafficLightVector* v : vec_segments) {
    for (const TrafficLight& traffic_light : *v) {
      const auto& stop_line = traffic_light.stop_line();
      math::Vec2d start_point(stop_line.point(0).x(), stop_line.point(0).y());
      math::Vec2d end_point(stop_line.point(1).x(), stop_line.point(1).y());
      Segment seg(start_point, end_point);
      double dis = seg.DistanceToPoint(math::Vec2d(x, y));
      if (dis < min_dis) {
        min_dis = dis;
        result = traffic_light;
      }
    }
  }
  return result;
}
}
}  // namespace pnc
