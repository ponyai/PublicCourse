// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&) = delete;      \
  void operator=(const TypeName&) = delete

#define DISALLOW_COPY_MOVE_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&) = delete;           \
  void operator=(const TypeName&) = delete;     \
  TypeName(TypeName&&) = delete;                \
  void operator=(TypeName&&) = delete

// Compiler attributes
#if (defined(__GNUC__) || defined(__APPLE__)) && !defined(SWIG)
// Compiler supports GCC-style attributes
#define MUST_USE_RESULT __attribute__((warn_unused_result))
#else
// Non-GCC equivalents
#define MUST_USE_RESULT
#endif

#ifndef PREDICT_TRUE
#ifdef __GNUC__
// Provided at least since GCC 3.0.
#define PREDICT_TRUE(x) (__builtin_expect(!!(x), 1))
#else
#define PREDICT_TRUE(x) (x)
#endif
#endif

#ifndef PREDICT_FALSE
#ifdef __GNUC__
// Provided at least since GCC 3.0.
#define PREDICT_FALSE(x) (__builtin_expect(x, 0))
#else
#define PREDICT_FALSE(x) (x)
#endif
#endif
