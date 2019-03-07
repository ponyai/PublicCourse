// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

// We suppose everyone who includes this header wants OpenGL context.
#define PONY_USE_GL

#ifdef PONY_USE_GL
// Uses OpenGL. Our code base currently uses OpenGL in PPOV and labeling tools.
#include <GL/gl.h>
#include <GL/glext.h>
#endif  // PONY_USE_GL
