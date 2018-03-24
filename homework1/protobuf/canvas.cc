// Copyright @2018 Pony AI Inc. All rights reserved.

#include "homework1/protobuf/canvas.h"

#include <iostream>
#include <glog/logging.h>

namespace homework1 {

using homework1::geometry::Point3D;

void Canvas::Draw() const {
  for (const auto& p : polygon_.point()) {
    std::cout << "Point:" << p.x() << ", " << p.y() << ", " << p.z() << std::endl;
  }
}

void Canvas::AddPoint(double x, double y, double z) {
  Point3D point;
  point.set_x(x);
  point.set_y(y);
  point.set_z(z);
  AddPoint(point);
}

void Canvas::AddPoint(const Point3D& p) {
  auto* point = polygon_.add_point();
  point->CopyFrom(p);
}

const Point3D& Canvas::GetPoint(int index) const {
  return polygon_.point(index);
}

void Canvas::ParseFromString(const std::string& serialzation) {
  polygon_.ParseFromString(serialzation);
}

const std::string Canvas::SerializeToString() const {
  std::string serialzation;
  CHECK(polygon_.SerializeToString(&serialzation)) << "Canvas serialization failed.";
  return serialzation;
}

void Canvas::AddLinePoint(double x, double y, double z) {
  Point3D point;
  point.set_x(x);
  point.set_y(y);
  point.set_z(z);
  AddLinePoint(point);
}

void Canvas::AddLinePoint(const Point3D& p) {
  auto* point = polyline_.add_point();
  point->CopyFrom(p);
}

double Canvas::LineLength() {
  double line_length = 0.0;
  int line_size = polyline_.point_size();
  for (int i=1; i < line_size; i++) {
    const auto& p1 = polyline_.point(i-1);
    const auto& p2 = polyline_.point(i);
    line_length += sqrt(pow(p1.x() - p2.x(),2) + pow(p1.y() - p2.y(), 2) + pow(p1.z() - p2.z(), 2));
  }
  return line_length;
}

}  // namespace homework1
