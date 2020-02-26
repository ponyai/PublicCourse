// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <string>
#include <vector>

#include "common/utils/display/layer.h"
#include "pnc/display/pnc_gl_painter.h"
#include "pnc/map/map_lib.h"

namespace utils {
namespace display {

class IntensityMapLayer : public utils::display::Layer {
 public:
  explicit IntensityMapLayer(const std::string& name);
  ~IntensityMapLayer() override = default;

  void Draw() const override;

 protected:
  void InitializeOnContextCreated() override;

 private:
  void ShowIntensityMap(const std::vector<utils::display::IntensityMapImage>& images,
                        double intensity_map_height) const;

  std::vector<utils::display::IntensityMapImage> intensity_map_images_;
};

}  // namespace display
}  // namespace utils
