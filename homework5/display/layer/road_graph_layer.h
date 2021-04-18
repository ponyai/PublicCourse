// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include "common/proto/agent_status.pb.h"
#include "common/proto/map.pb.h"
#include "common/proto/simulation.pb.h"
#include "homework6/display/layer/road_graph_layer_base.h"
#include "homework6/display/user_interface_data.h"
#include "homework6/map/map_lib.h"

namespace utils {
namespace display {

class RoadGraphLayer : public RoadGraphLayerBase {
 public:
  RoadGraphLayer(const std::string& name, const homework6::map::MapLib* map_lib,
                 const interface::simulation::SimulationSystemData& simulation_system_data,
                 const UserInterfaceData& user_interface_data);
  ~RoadGraphLayer() override = default;

  void Draw() const override;

 private:
  void DrawTrafficLightModel(
      const interface::map::Map& road_graph,
      const interface::simulation::SimulationSystemData& simulation_system_data) const;

  utils::gl_support::MeshModel traffic_light_model_;

  // UserInterfaceData is owned by the UI main-window and keeps the data produced by the UI widgets
  // or mouse/keyboard. The layer will be destructed before user_interface_data.
  const UserInterfaceData& user_interface_data_;

  const interface::simulation::SimulationSystemData& simulation_system_data_;

  DISALLOW_COPY_MOVE_AND_ASSIGN(RoadGraphLayer);
};

}  // namespace display
}  // namespace utils
