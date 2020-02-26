// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include "common/utils/display/color.h"
#include "pnc/display/layer/simulation_system_layer_base.h"
#include "pnc/display/user_interface_data.h"

namespace utils {
namespace display {

class ObstacleLayer : public SimulationSystemLayerBase {
 public:
  ObstacleLayer(const std::string& name, const interface::simulation::SimulationSystemData& data,
                const utils::display::UserInterfaceData& user_interface_data)
      : SimulationSystemLayerBase(name, data), user_interface_data_(user_interface_data) {}
  ~ObstacleLayer() override = default;

  void Draw() const override;

 protected:
  void DrawObstacle(const interface::perception::PerceptionObstacle& obstacle, bool draw_id = false,
                    bool draw_velocity = false, bool draw_vehicle_box = false,
                    utils::display::Color text_color = utils::display::Color::Yellow(),
                    double height_offset = 0.0) const;

  const utils::display::UserInterfaceData& user_interface_data_;
};

}  // namespace display
}  // namespace utils
