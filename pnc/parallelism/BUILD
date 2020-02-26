package(default_visibility = ["//visibility:public"])

cc_library(
    name = "multi_process",
    srcs = ["multi_process.cc"],
    hdrs = ["multi_process.h"],
    deps = [
        "//common/utils/containers:fixed_array",
        "@glog//:glog",
    ],
)

cc_library(
    name = "thread_pool",
    srcs = [
        "thread_pool.cc",
    ],
    hdrs = [
        "thread_pool.h",
    ],
    deps = [
        "//common/utils/common:defines",
        "//common/utils/common:mutex",
        "@glog//:glog",
    ],
)
