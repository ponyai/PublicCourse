// Copyright @2018 Pony AI Inc. All rights reserved.
//
#include <gtest/gtest.h>
#include "homework1/protobuf/canvas.h"

namespace homework1 {

TEST(Canvas, Serialization) {
  Canvas canvas;
  canvas.AddPoint(1.0, 2.0, 3.0);

  const std::string serialization = canvas.SerializeToString();

  Canvas other_canvas;
  other_canvas.ParseFromString(serialization);
  ASSERT_EQ(1, other_canvas.point_size());
  EXPECT_NEAR(1.0, other_canvas.GetPoint(0).x(), 1e-6);
  EXPECT_NEAR(2.0, other_canvas.GetPoint(0).y(), 1e-6);
  EXPECT_NEAR(3.0, other_canvas.GetPoint(0).z(), 1e-6);
}

}  // namespace homework1
