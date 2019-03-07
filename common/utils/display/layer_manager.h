// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <map>
#include <memory>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include "common/utils/display/gl_painter.h"
#include "common/utils/display/layer.h"
#include "common/utils/gl/context.h"

namespace utils {
namespace display {

class LayerManager {
 public:
  LayerManager(gl::Context* context, OpenglPainter* gl_painter, FontRenderer* font_renderer);

  // Add the newly allocated layer, please note that the original copy of layer_ptr would be
  // invalid after this function is called.
  // Layers may have different levels(default is 0). Layers with higher levels would be drawn on top
  // of the lower ones.
  void AddLayer(std::unique_ptr<Layer> layer_ptr, int level, bool always_enabled = false);

  void EnableLayer(const std::string& layer_name);

  void DisableLayer(const std::string& layer_name);

  void Resize(int width, int height);

  // Only those layers that set enabled or enabled by default would actually work.
  void DrawLayers() const;

 private:
  std::unordered_set<std::string> default_layer_names_;
  // 'layers_name_map_' own layers. If you want to remove layers from the map, please make sure you
  // also make the necessary change on layers_level_map_ in the mean time.
  std::unordered_map<std::string, std::unique_ptr<Layer>> layers_name_map_;
  // layers_level_map_ doesn't own any layers.
  std::map<int, std::vector<Layer*>> layers_level_map_;
  gl::Context* context_ = nullptr;         // Not owned.
  OpenglPainter* gl_painter_ = nullptr;    // Not owned.
  FontRenderer* font_renderer_ = nullptr;  // Not owned.

  DISALLOW_COPY_MOVE_AND_ASSIGN(LayerManager);
};

}  // namespace display
}  // namespace utils
