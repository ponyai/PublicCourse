package(default_visibility = ["//visibility:public"])

cc_library(
    name = "defines",
    hdrs = [
        "defines.h",
    ],
)

cc_library(
    name = "mutex",
    hdrs = [
        "mutex.h",
    ],
    deps = [
        "@glog//:glog",
    ],
)

cc_library(
    name = "optional",
    hdrs = [
        "optional.h",
    ],
    deps = [
        "@boost//:optional",
    ],
)

cc_library(
    name = "proto_utils",
    srcs = [
        "proto_utils.cc",
    ],
    hdrs = [
        "proto_utils.h",
    ],
    deps = [
        "@com_google_protobuf//:protobuf",
        "@glog//:glog",
    ],
)
