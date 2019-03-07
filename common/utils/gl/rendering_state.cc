// Copyright @2018 Pony AI Inc. All rights reserved.

#include "utils/gl/rendering_state"

namespace gl {

void RenderingState::Reset() {
  point_size = 1.0;
  line_width = 1.0;
  line_dashed = false;
  line_stipple = false;
  line_stipple_factor = 1;
  line_stipple_pattern = 0x00FF;
}

}  // namespace gl
