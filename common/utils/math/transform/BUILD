package(default_visibility = ["//visibility:public"])

cc_library(
    name = "transform",
    srcs = ["transform.cc"],
    hdrs = [
        "transform.h",
    ],
    deps = [
        ":rigid_transform",
        "//common/proto:cc_geometry_proto",
        "//common/proto:cc_transform_proto",
        "//common/utils/math",
        "@eigen//:eigen",
        "@glog//:glog",
    ],
)

cc_library(
    name = "rigid_transform",
    hdrs = [
        "rigid_transform.h",
    ],
    deps = [
        "@eigen//:eigen",
        "@glm",
        "@glog//:glog",
    ],
)


