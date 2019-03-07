package(default_visibility = ["//visibility:public"])

cc_library(
    name = "math",
    srcs = [
        "math_utils.cc",
        "box2d.cc",
        "vec2d.cc",
        "vec3d.cc",
    ],
    hdrs = [
        "math_utils.h",
        "box2d.h",
        "ray.h",
        "vec.h",
        "vec2d.h",
        "vec3d.h",
    ],
    deps = [
        ":common",
        "//common/proto:cc_geometry_proto",
        "//common/utils/containers:array_view",
        "//common/utils/containers:fixed_array",
        "@eigen//:eigen",
        "@glm",
        "@glog//:glog",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "common",
    hdrs = [
        "common.h",
    ],
)

cc_library(
    name = "dimension",
    hdrs = [
        "dimension.h",
    ],
    deps = [
        ":math",
        "@eigen",
        "@glm",
    ],
)
