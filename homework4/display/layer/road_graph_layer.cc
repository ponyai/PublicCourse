// Copyright @2018 Pony AI Inc. All rights reserved.

#include "homework4/display/layer/road_graph_layer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "common/utils/containers/utils.h"
#include "common/utils/file/path.h"
#include "common/utils/gl/base.h"
#include "common/utils/gl/basic_primitives.h"
#include "homework4/display/layer/layers_common.h"
#include "homework4/display/map_gl_painter.h"

namespace utils {
namespace display {

using interface::geometry::Point3D;

RoadGraphLayer::RoadGraphLayer(const std::string& name, const homework4::map::MapLib* map_lib)
    : RoadGraphLayerBase(name, map_lib) {}

void RoadGraphLayer::Draw() const {
  static_cast<MapOpenglPainter*>(gl_painter_)->DrawRoadGraph(map_data(), true, true);
}

}  // namespace display
}  // namespace utils
