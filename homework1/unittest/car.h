// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

namespace homework1 {

enum class Color {
  kWhite = 0,
  kRed = 1,
  kGreen = 2,
  kBlack = 3
};

class Car {
 public:
  enum class State {
    kParked = 0,
    kRunning = 1
  };

  Car() = default;
  Car(Color color) : color_(color) {}

  int wheels_number() const { return 4; }
  Color color() const { return color_; }
  State state() const { return state_; }

  void Drive() {
    state_ = State::kRunning;
  }

  void Stop() {
    state_ = State::kParked;
  }

 private:
   State state_ = State::kParked;
   Color color_ = Color::kWhite;
};

}  // namesapce homework1
