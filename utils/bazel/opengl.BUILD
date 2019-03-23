licenses(["restricted"])  # SGI B

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "opengl",
    srcs = [
        "lib/libGL.so",
        "lib/libGLU.so",
    ],
    hdrs = glob(["include/GL/**"]),
    includes = ["include"],
)

cc_library(
    name = "egl",
    srcs = ["lib/libEGL.so"],
    hdrs = glob(["include/EGL/**"]),
    includes = ["include"],
)
