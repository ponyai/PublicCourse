// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

namespace gl {

struct RenderingState {
  // https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glPointSize.xml
  float point_size = 1.0;
  // https://www.khronos.org/registry/OpenGL-Refpages/es2.0/xhtml/glLineWidth.xml
  float line_width = 1.0;
  // https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glLineStipple.xml
  bool line_dashed = false;
  bool line_stipple = false;
  int line_stipple_factor = 1;
  int line_stipple_pattern = 0x00FF;

  void Reset();
};

}  // namespace gl
