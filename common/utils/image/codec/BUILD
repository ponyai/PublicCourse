package(default_visibility = ["//visibility:public"])

cc_library(
    name = "codec",
    hdrs = ["codec.h"],
    deps = [
        "//common/utils/file",
        "//common/utils/image:image_header",
    ],
)

cc_library(
    name = "jpeg",
    srcs = ["jpeg.cc"],
    hdrs = ["jpeg.h"],
    deps = [
        ":codec",
        "@glog",
        "@jpeg_turbo//:jpeg",
    ],
)

cc_library(
    name = "netpbm",
    srcs = ["netpbm.cc"],
    hdrs = ["netpbm.h"],
    deps = [
        ":codec",
        "@glog",
    ],
)

cc_library(
    name = "png",
    srcs = ["png.cc"],
    hdrs = ["png.h"],
    deps = [
        ":codec",
        "@glog",
        "@png",
    ],
)
