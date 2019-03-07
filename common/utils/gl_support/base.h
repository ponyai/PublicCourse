// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>

#define ASSERT_GL_OK()                                           \
  do {                                                           \
    auto gl_error = glGetError();                                \
    CHECK_EQ(gl_error, GL_NO_ERROR) << gluErrorString(gl_error); \
  } while (false);
