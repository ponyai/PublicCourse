package(default_visibility = ["//visibility:public"])

cc_library(
    name = "format",
    srcs = [
        "format.cc",
    ],
    hdrs = [
        "format.h",
    ],
    deps = [
        ":utils",
        "@fmt",
    ],
)

cc_library(
    name = "string_view",
    srcs = [
        "string_view.cc",
    ],
    hdrs = [
        "string_view.h",
    ],
    deps = [
        "@glog//:glog",
    ],
)

cc_library(
    name = "utils",
    srcs = [
        "utils.cc",
    ],
    hdrs = [
        "utils.h",
    ],
    deps = [
        ":string_view",
    ],
)

