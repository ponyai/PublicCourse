licenses(["restricted"])  # SGI B

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "opengl",
    srcs = [
        "lib/x86_64-linux-gnu/libGL.so",
        "lib/x86_64-linux-gnu/libGLU.so",
    ],
    hdrs = glob(["include/GL/**"]),
    includes = ["include"],
)

cc_library(
    name = "egl",
    srcs = ["lib/x86_64-linux-gnu/libEGL.so"],
    hdrs = glob(["include/EGL/**"]),
    includes = ["include"],
)
