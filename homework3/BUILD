package(default_visibility = ["//visibility:public"])

cc_library(
    name = "cubic_spline",
    srcs = ["cubic_spline.cc"],
    hdrs = ["cubic_spline.h"],
)

cc_binary(
    name = "interpolate_main",
    srcs = ["interpolate_main.cc"],
    deps = [
        ":cubic_spline",
        "@gflags",
        "@glog",
    ],
)
