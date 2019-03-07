// Copyright @2018 Pony AI Inc. All rights reserved.

#include <glog/logging.h>

#include "common/utils/containers/utils.h"
#include "common/utils/display/layer_manager.h"

namespace utils {
namespace display {

LayerManager::LayerManager(gl::Context* context, OpenglPainter* gl_painter,
                           FontRenderer* font_renderer)
    : context_(CHECK_NOTNULL(context)),
      gl_painter_(CHECK_NOTNULL(gl_painter)),
      font_renderer_(CHECK_NOTNULL(font_renderer)) {}

void LayerManager::AddLayer(std::unique_ptr<Layer> layer, int level, bool always_enabled) {
  const std::string& name = layer->GetName();
  auto& layer_in_map = layers_name_map_[name];
  if (layer_in_map == nullptr) {
    layer_in_map = std::move(layer);
    layer_in_map->InitializeContext(context_, gl_painter_, font_renderer_);
    layers_level_map_[level].push_back(layer_in_map.get());
    if (always_enabled) {
      default_layer_names_.insert(name);
    }
    layer_in_map->Enable();
  } else {
    LOG(ERROR) << "Layer with name '" << name << "' has already been added!";
  }
}

void LayerManager::EnableLayer(const std::string& layer_name) {
  auto layer = utils::FindOrNull(layers_name_map_, layer_name);
  if (layer == nullptr) {
    LOG(WARNING) << "Failed to enable layer '" << layer_name << "', not found!";
  } else {
    (*layer)->Enable();
  }
}

void LayerManager::DisableLayer(const std::string& layer_name) {
  auto layer = utils::FindOrNull(layers_name_map_, layer_name);
  if (layer == nullptr) {
    LOG(WARNING) << "Failed to disable layer '" << layer_name << "', not found!";
  } else {
    if (default_layer_names_.count(layer_name) != 0) {
      LOG(WARNING) << "Failed to disable layer '" << layer_name
                   << "', which is enabled by default!";
    } else {
      (*layer)->Disable();
    }
  }
}

void LayerManager::Resize(int width, int height) {
  for (auto const& layer_pair : layers_name_map_) {
    layer_pair.second->Resize(width, height);
  }
}

void LayerManager::DrawLayers() const {
  // Draw the layers from the lowest level to the highest.
  for (auto const& level_pair : layers_level_map_) {
    for (Layer* layer : level_pair.second) {
      const std::string& name = layer->GetName();
      if (default_layer_names_.count(name) != 0 || layer->IsEnabled()) {
        layer->Draw();
      }
    }
    glClear(GL_DEPTH_BUFFER_BIT);
  }
}

}  // namespace display
}  // namespace utils
