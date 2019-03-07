// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#ifdef __clang__

#define DISABLE_MISSING_OVERRIDE_WARNING_BEGIN \
  _Pragma("clang diagnostic push")             \
      _Pragma("clang diagnostic ignored \"-Winconsistent-missing-override\"")

#define DISABLE_MISSING_OVERRIDE_WARNING_END _Pragma("clang diagnostic pop")

#else

#define DISABLE_MISSING_OVERRIDE_WARNING_BEGIN
#define DISABLE_MISSING_OVERRIDE_WARNING_END

#endif  // __clang
