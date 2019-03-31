// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

namespace utils {
namespace display {

// For different layers of drawings. If need to add more layers, please define
// here. The layers
// should be positive and incremental.
enum GlLayers {
  kLayerRoadGraph,
  kLayerRoute,
  kHighlightedLines,
};

float GetGlLayer(GlLayers l);

}  // namespace display
}  // namespace utils
