// Copyright @2018 Pony AI Inc. All rights reserved.
#include "common/utils/display/primitive_painter.h"

#include <GL/gl.h>

#include <atomic>
#include "glog/logging.h"

#include "common/utils/display/color.h"
#include "common/utils/gl/basic_primitives.h"
#include "common/utils/gl/context.h"
#include "common/utils/gl/functions.h"
#include "common/utils/gl/texture.h"

namespace utils {
namespace display {

PrimitivePainter::PrimitivePainter(gl::Context* context) : context_(context) {}

template <>
void PrimitivePainter::DrawQuadsWithTexcoord<math::Vec3d>(
    const utils::ConstArrayView<math::Vec3d>& points,
    const utils::ConstArrayView<math::Vec2d>& texcoords) const {
  DCHECK_EQ(points.size(), texcoords.size());
  DCHECK_EQ(points.size() % 4, 0u);

  const auto& f = context_->functions();
  f.EnableClientState(GL_VERTEX_ARRAY);
  f.EnableClientState(GL_TEXTURE_COORD_ARRAY);
  f.Enable(GL_TEXTURE_2D);

  f.VertexPointer(3, GL_DOUBLE, 0, points.data());
  f.TexCoordPointer(2, GL_DOUBLE, 0, texcoords.data());
  for (int64_t i = 0; i < points.size(); i += 4) {
    f.DrawArrays(GL_TRIANGLE_FAN, i, 4);
  }

  f.DisableClientState(GL_VERTEX_ARRAY);
  f.DisableClientState(GL_TEXTURE_COORD_ARRAY);
}

}  // namespace display
}  // namespace utils
