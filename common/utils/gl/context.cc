// Copyright @2018 Pony AI Inc. All rights reserved.

#include "common/utils/gl/context.h"

#include <string>

#include "common/utils/gl/functions.h"

namespace gl {

void Context::Initialize() {
  MakeCurrent();
  functions_ = std::make_unique<Functions>(this);
  DoneCurrent();
}

void Context::CheckError() const {
  CHECK(functions_ != nullptr);
  auto gl_error = functions_->GetError();
  std::string error_string;
  switch (gl_error) {
    case GL_INVALID_ENUM:
      error_string = "GL_INVALID_ENUM";
      break;
    case GL_INVALID_VALUE:
      error_string = "GL_INVALID_VALUE";
      break;
    case GL_INVALID_OPERATION:
      error_string = "GL_INVALID_OPERATION";
      break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      error_string = "GL_INVALID_FRAMEBUFFER_OPERATION";
      break;
    case GL_OUT_OF_MEMORY:
      error_string = "GL_OUT_OF_MEMORY";
      break;
    case GL_NO_ERROR:
    default:
      break;
  }
  CHECK_EQ(gl_error, GL_NO_ERROR) << error_string;
}

}  // namespace gl
