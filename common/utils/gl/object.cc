// Copyright @2018 Pony AI Inc. All rights reserved.

#include "common/utils/gl/object.h"

namespace gl {

Object::Object(const Context* context) : context_(CHECK_NOTNULL(context)) {}

}  // namespace gl
