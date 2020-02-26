package(default_visibility = ["//visibility:public"])

cc_library(
    name = "map_lib",
    srcs = ["map_lib.cc"],
    hdrs = ["map_lib.h"],
    data = [
        "//pnc/map/grid3:grid3_map",
    ],
    deps = [
        "//common/proto:cc_map_proto",
        "//common/utils/file",
        "@glog",
    ],
)

cc_library(
    name = "segment",
    hdrs = ["segment.h"],
    deps = [
        "//common/utils/math",
        "@gflags",
        "@glog",
    ],
)

cc_library(
    name = "map_meta",
    srcs = ["map_meta.cc"],
    hdrs = ["map_meta.h"],
    deps = [
        ":segment",
        "//common/proto:cc_agent_status_proto",
        "//common/proto:cc_control_proto",
        "//common/proto:cc_map_proto",
        "//common/proto:cc_simulation_config_proto",
        "//common/utils/common:optional",
        "//common/utils/index:grid_index",
        "//common/utils/math",
        "@gflags",
        "@glog",
    ],
)
