package(default_visibility = ["//visibility:public"])

cc_library(
    name = "base",
    hdrs = ["base.h"],
    deps = [
        "@opengl//:opengl",
    ],
)

cc_library(
    name = "material",
    srcs = ["material.cc"],
    hdrs = ["material.h"],
    deps = [
        "//common/utils/strings:utils",
        "@glog//:glog",
        "@opengl//:opengl",
    ],
)

cc_library(
    name = "mesh_model",
    srcs = ["mesh_model.cc"],
    hdrs = ["mesh_model.h"],
    deps = [
        ":material",
        "//common/utils/common:defines",
        "//common/utils/file:path",
        "//common/utils/math",
        "//common/utils/strings:utils",
        "@glog//:glog",
        "@opengl//:opengl",
    ],
)


