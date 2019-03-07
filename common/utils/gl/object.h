// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include "common/utils/gl/functions.h"

namespace gl {

class Object {
 public:
  explicit Object(const Context* context);
  virtual ~Object() = default;

 protected:
  const Context* context() { return context_; }
  const Context::Functions& f() { return context_->functions(); }

 private:
  const Context* context_ = nullptr;
};

}  // namespace gl
