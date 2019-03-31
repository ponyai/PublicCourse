package(default_visibility = ["//visibility:public"])

cc_library(
    name = "map_lib",
    hdrs = ["map_lib.h"],
    data = [
        "//homework4/map/grid2:grid2_map",
    ],
    deps = [
        "//common/proto:cc_map_proto",
        "//common/utils/file",
        "@gflags",
        "@glog",
    ],
)
