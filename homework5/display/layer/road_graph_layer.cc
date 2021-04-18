// Copyright @2018 Pony AI Inc. All rights reserved.

#include "homework6/display/layer/road_graph_layer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "common/utils/containers/utils.h"
#include "common/utils/file/path.h"
#include "common/utils/gl/base.h"
#include "common/utils/gl/basic_primitives.h"
#include "gflags/gflags.h"
#include "homework6/display/gl_layers.h"
#include "homework6/display/layer/layers_common.h"
#include "homework6/display/pnc_gl_painter.h"

namespace utils {
namespace display {

using interface::geometry::Point3D;

RoadGraphLayer::RoadGraphLayer(
    const std::string& name, const homework6::map::MapLib* map_lib,
    const interface::simulation::SimulationSystemData& simulation_system_data,
    const UserInterfaceData& user_interface_data)
    : RoadGraphLayerBase(name, map_lib),
      user_interface_data_(user_interface_data),
      simulation_system_data_(simulation_system_data) {
  std::string model_dir = utils::path::Get3dModelsDirectory();
  traffic_light_model_.LoadFromFile(file::path::Join(model_dir, "traffic_light.obj"));
}

void RoadGraphLayer::Draw() const {
  static_cast<PncOpenglPainter*>(gl_painter_)->DrawRoadGraph(map_data(), true, true);
}

}  // namespace display
}  // namespace utils
