package(default_visibility = ["//visibility:public"])

cc_library(
    name = "vehicle",
    srcs = ["vehicle.cc"],
    hdrs = ["vehicle.h"],
    deps = [
        ":vehicle_status_helper",
        "//common/proto:cc_perception_proto",
        "//common/proto:cc_vehicle_params_proto",
        "//common/utils/math",
        "//common/utils/math/transform",
        "//pnc/simulation:vehicle_agent",
    ],
)

cc_library(
    name = "vehicle_status_helper",
    srcs = ["vehicle_status_helper.cc"],
    hdrs = ["vehicle_status_helper.h"],
    deps = [
        "//common/proto:cc_agent_status_proto",
        "//common/utils/math",
        "//common/utils/math/transform",
        "@eigen",
    ],
)

cc_library(
    name = "index",
    hdrs = ["index.h"],
    deps = [
        "//common/utils/file:file",
    ],
)
