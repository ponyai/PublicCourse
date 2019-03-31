// Copyright @2018 Pony AI Inc. All rights reserved.

#include "homework2/rotation/rotation.h"
#include <cmath>
#include <limits>

#define MATH_PI 3.14159265358979323846264338327950419984

namespace homework2 {

Eigen::Vector3d ToRollPitchYaw(Eigen::Matrix3d rotation) {
  double yaw = std::atan(rotation(1,0) / rotation(0,0));
  double pitch = std::atan(-rotation(2,0) / std::sqrt(rotation(2,1) * rotation(2,1) + rotation(2,2) * rotation(2,2)));
  double roll = std::atan(rotation(2,1) / rotation(2,2));
  return Eigen::Vector3d(yaw, pitch, roll);
}

Eigen::AngleAxisd ToAngleAxis(Eigen::Matrix3d rotation) {
  double eps = std::numeric_limits<double>::epsilon();
  Eigen::Vector3d axis;
  if( std::abs(rotation(0,1) - rotation(1,0)) < eps
    && std::abs(rotation(0,2) - rotation(2,0)) < eps
    && std::abs(rotation(1,2) - rotation(2,1)) < eps ) {
    // Rotation Matrix is Singular
    if( std::abs(rotation(0,1) + rotation(1,0)) < eps
      && std::abs(rotation(0,2) + rotation(2,0)) < eps
      && std::abs(rotation(1,2) + rotation(2,1)) < eps
      && std::abs(rotation(0,0) - 1.0) < eps
      && std::abs(rotation(1,1) - 1.0) < eps
      && std::abs(rotation(2,2) - 1.0) < eps ) {
        // Rotation Matrix is Indentity
	return Eigen::AngleAxisd(0, Eigen::Vector3d::UnitX());
	}
    else {
      // Rotation Angle is PI
      double xx = (rotation(0,0) + 1)/2;
      double yy = (rotation(1,1) + 1)/2;
      double zz = (rotation(2,2) + 1)/2;
      double xy = (rotation(0,1) + rotation(1,0))/4;
      double xz = (rotation(0,2) + rotation(2,0))/4;
      double yz = (rotation(1,2) + rotation(2,1))/4;
      if ((xx > yy) && (xx > zz)) {
        if (xx < eps) 
	  axis = Eigen::Vector3d(0, std::sqrt(2), std::sqrt(2));
        else {
	  double x = std::sqrt(xx);
	  axis = Eigen::Vector3d(x, xy/x, xz/x);
	}
      }
      else if (yy > zz) {
        if (yy < eps)
	  axis = Eigen::Vector3d(std::sqrt(2), 0, std::sqrt(2));
	else {
	  double y = std::sqrt(yy);
	  axis = Eigen::Vector3d(xy/y, y, yz/y);
	}
      }
      else {
        if (zz < eps)
	  axis = Eigen::Vector3d(std::sqrt(2), std::sqrt(2), 0);
	else {
	  double z = std::sqrt(zz);
	  axis = Eigen::Vector3d(xz/z, yz/z, z);
	}
      }
      return Eigen::AngleAxisd(MATH_PI, axis);
    }
  }
  // Now there is no sigular situration
  double normalizer = std::sqrt((rotation(2,1) - rotation(1,2)) * (rotation(2,1) - rotation(1,2))
    + (rotation(0,1) - rotation(1,0)) * (rotation(0,1) - rotation(1,0))
    + (rotation(0,2) - rotation(2,0)) * (rotation(0,2) - rotation(2,0)));
  double angle = std::acos((rotation(0,0) + rotation(1,1) + rotation(2,2) - 1) / 2);
  double x = (rotation(2,1) - rotation(1,2)) / normalizer;
  double y = (rotation(0,2) - rotation(2,0)) / normalizer;
  double z = (rotation(1,0) - rotation(0,1)) / normalizer;
  axis = Eigen::Vector3d(x, y, z); 
  return Eigen::AngleAxisd(angle, axis);
}
}  // namespace homework2
