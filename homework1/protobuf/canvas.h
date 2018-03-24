// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <string>
#include "homework1/protobuf/proto/geometry.pb.h"

namespace homework1 {

class Canvas {
 public:
  Canvas() = default;
  ~Canvas() = default;

  int point_size() const { return polygon_.point_size(); }
  void Draw() const;

  void AddPoint(double x, double y, double z);
  void AddPoint(const homework1::geometry::Point3D& p);
  const homework1::geometry::Point3D& GetPoint(int index) const;

  void ParseFromString(const std::string& serialzation);
  const std::string SerializeToString() const;

  void AddLinePoint(double x, double y, double z);
  void AddLinePoint(const homework1::geometry::Point3D& p);
  double LineLength();

 private:
  homework1::geometry::Polygon polygon_;
  homework1::geometry::Polyline polyline_;
};

}  // namespace homework1

