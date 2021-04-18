package(default_visibility = ["//visibility:public"])

cc_library(
    name = "vehicle_agent",
    hdrs = ["vehicle_agent.h"],
    deps = [
        "//common/proto:cc_agent_status_proto",
        "//common/proto:cc_control_proto",
        "//common/proto:cc_simulation_proto",
        "//common/proto:cc_vehicle_params_proto",
        "//common/utils/file",
        "//common/utils/file:path",
        "//homework6/map:map_lib",
        "@glog",
    ],
)

cc_library(
    name = "simulation_system",
    srcs = ["simulation_system.cc"],
    hdrs = ["simulation_system.h"],
    deps = [
        ":simulation_engine_base",
        ":simulation_world",
        ":single_thread_simulation_engine",
        ":system_base",
        ":vehicle_agent",
        "//common/proto:cc_simulation_proto",
        "//common/utils/common:mutex",
        "//common/utils/math",
        "//homework6/map:map_lib",
        "//homework6/proto:cc_simulation_config_proto",
        "@com_google_protobuf//:protobuf",
        "@glog",
    ],
)

cc_library(
    name = "simulation_world",
    srcs = ["simulation_world.cc"],
    hdrs = ["simulation_world.h"],
    deps = [
        "//common/proto:cc_agent_status_proto",
        "//common/proto:cc_map_proto",
        "//common/proto:cc_vehicle_params_proto",
        "//common/utils/math",
        "//common/utils/math/transform",
        "//homework6/proto:cc_simulation_config_proto",
        "//homework6/simulation/dynamic_lib:libdynamic",
        "@eigen",
        "@gflags",
        "@glog",
    ],
)

cc_library(
    name = "system_base",
    srcs = ["system_base.cc"],
    hdrs = ["system_base.h"],
    deps = [
        "//common/proto:cc_playback_proto",
        "//common/proto:cc_simulation_proto",
        "//common/utils/common:mutex",
    ],
)

cc_library(
    name = "simulation_engine_base",
    hdrs = ["simulation_engine_base.h"],
    deps = [
        "//common/proto:cc_agent_status_proto",
        "//common/proto:cc_control_proto",
        "//homework6/agents",
        "@gflags",
        "@glog",
    ],
)

cc_library(
    name = "single_thread_simulation_engine",
    hdrs = ["single_thread_simulation_engine.h"],
    deps = [
        ":simulation_engine_base",
        ":vehicle_agent",
        ":vehicle_agent_factory",
        "//homework6/proto:cc_simulation_config_proto",
        "@gflags",
        "@glog",
    ],
)

cc_library(
    name = "vehicle_agent_factory",
    hdrs = ["vehicle_agent_factory.h"],
    deps = [
        ":vehicle_agent",
        "//common/utils/containers:utils",
        "//common/utils/math",
        "@gflags",
        "@glog",
    ],
)
