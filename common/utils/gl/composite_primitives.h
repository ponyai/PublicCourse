// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include "common/utils/gl/basic_primitives.h"
#include "common/utils/gl/context.h"

namespace gl {

class Surface : public Polygon {
 public:
  explicit Surface(const Context* context);
  ~Surface() override = default;
  void SetRenderingFlag(bool show_vertex = true, bool show_edge = true, bool show_surface = true);

 protected:
  void DrawData() const override;

 private:
  bool show_surface_ = true;
  bool show_edge_ = true;
  bool show_vertex_ = true;
};

}  // namespace gl
