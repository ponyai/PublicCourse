// Copyright @2018 Pony AI Inc. All rights reserved.
#include <gtest/gtest.h>

#include "homework1/unittest/car.h"

namespace homework1 {

TEST(Car, TestWheelNumber) {
  Car car;
  EXPECT_EQ(4, car.wheels_number());
}

TEST(Car, TestDefaultColor) {
  Car car;
  EXPECT_EQ(Color::kWhite, car.color());
}

TEST(Car, TestColor) {
  Car car(Color::kBlack);
  EXPECT_EQ(Color::kBlack, car.color());
}

TEST(Car, TestState) {
  Car car;
  ASSERT_EQ(Car::State::kParked, car.state());

  // Please check the state of car.
  car.Drive();
  // Your code here.

  car.Stop();
  // Your code here.

}

}  // namespace homework1
