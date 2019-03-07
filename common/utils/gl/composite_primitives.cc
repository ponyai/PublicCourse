// Copyright @2018 Pony AI Inc. All rights reserved.

#include "common/utils/gl/composite_primitives.h"

namespace gl {

Surface::Surface(const Context* context) : Polygon(context) {}

void Surface::SetRenderingFlag(bool show_vertex, bool show_edge, bool show_surface) {
  show_vertex_ = show_vertex;
  show_edge_ = show_edge;
  show_surface_ = show_surface;
}

void Surface::DrawData() const {
  if (show_surface_) {
    Triangles::DrawData(context_, Triangles::DrawMode::kTriangles, rendering_state_,
                        vertices_count_);
  }
  if (show_edge_) {
    Lines::DrawData(context_, Lines::DrawMode::kLineLoop, rendering_state_, vertices_count_);
  }
  if (show_vertex_) {
    Points::DrawData(context_, rendering_state_, vertices_count_);
  }
}

}  // namespace gl
