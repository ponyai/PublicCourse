// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <memory>

#include <glog/logging.h>

#include "common/utils/gl/base.h"

namespace gl {

// An interface for double-buffer OpenGL contexts.
// Reference:
//   https://www.khronos.org/opengl/wiki/OpenGL_Context
class Context {
 public:
  class Functions;
  using FuncPointer = void (*)();

  Context() = default;
  virtual ~Context() = default;

  // For complete decoupling from the specific OpenGL implementations, callers should call functions
  // in 'Functions' class instead of directly calling the 'glFooBar' functions.
  const Functions& functions() const { return *functions_; }

  void Initialize();
  void CheckError() const;

  // OpenGL operations should be enclosed by a pair of MakeCurrent() and DoneCurrent().
  virtual void MakeCurrent() = 0;
  virtual void DoneCurrent() = 0;

  // Flushes the pending OpenGL operations, and actually show them on screen.
  virtual void SwapBuffers() = 0;

  virtual FuncPointer GetProcAddress(const char* proc_name) const = 0;

 protected:
  std::unique_ptr<Functions> functions_;
};

}  // namespace gl
