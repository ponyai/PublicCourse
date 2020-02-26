package(default_visibility = ["//visibility:public"])

cc_library(
    name = "vehicle_agent",
    hdrs = ["vehicle_agent.h"],
    deps = [
        "//common/proto:cc_agent_status_proto",
        "//common/proto:cc_control_proto",
        "//common/proto:cc_display_proto",
        "//common/proto:cc_simulation_proto",
        "//common/proto:cc_vehicle_params_proto",
        "//common/utils/file",
        "//pnc/display:variable_view",
        "//pnc/map:map_lib",
        "@glog",
    ],
)

cc_library(
    name = "metric_collector",
    srcs = ["metric_collector.cc"],
    hdrs = ["metric_collector.h"],
    deps = [
        "//common/proto:cc_agent_metric_proto",
        "//common/proto:cc_agent_status_proto",
        "//common/utils/math",
        "//common/utils/math/transform",
        "//pnc/utils:vehicle_status_helper",
        "@eigen",
    ],
)

cc_library(
    name = "simulation_system",
    srcs = ["simulation_system.cc"],
    hdrs = ["simulation_system.h"],
    deps = [
        ":manual_test_agent",
        ":metric_collector",
        ":simulation_engine_base",
        ":simulation_world",
        ":single_thread_simulation_engine",
        ":system_base",
        ":types",
        ":vehicle_agent",
        "//common/proto:cc_agent_metric_proto",
        "//common/proto:cc_simulation_proto",
        "//common/utils/common:mutex",
        "//common/utils/math",
        "//pnc/map:map_lib",
        "//pnc/simulation/multi_process:multi_process_simulation_engine",
        "//pnc/utils:vehicle",
        "@com_google_protobuf//:protobuf",
        "@glog",
    ],
)

cc_library(
    name = "replay_system",
    srcs = ["replay_system.cc"],
    hdrs = ["replay_system.h"],
    deps = [
        ":system_base",
        ":vehicle_agent",
        "//common/proto:cc_simulation_proto",
        "//common/utils/common:mutex",
        "//common/utils/math",
        "//pnc/map:map_lib",
        "//pnc/utils:vehicle",
        "//pnc/utils:index",
        "@com_google_protobuf//:protobuf",
        "@glog",
    ],
)

cc_library(
    name = "simulation_world",
    srcs = ["simulation_world.cc"],
    hdrs = ["simulation_world.h"],
    deps = [
        ":mocked_obstacle_agent",
        ":simulation_evaluator",
        "//common/proto:cc_agent_status_proto",
        "//common/proto:cc_map_proto",
        "//common/proto:cc_perception_proto",
        "//common/proto:cc_vehicle_params_proto",
        "//common/utils/index:grid_index",
        "//common/utils/math",
        "//common/utils/math/transform",
        "//pnc/map:map_meta",
        "//pnc/map:segment",
        "//pnc/simulation:traffic_light_agent",
        "//pnc/simulation/dynamic_lib:libdynamic",
        "//pnc/utils:vehicle",
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
        "@glog",
    ],
)

cc_library(
    name = "manual_test_agent",
    hdrs = ["manual_test_agent.h"],
    deps = [
        ":vehicle_agent",
        "//common/utils/common:mutex",
    ],
)

cc_library(
    name = "traffic_light_agent",
    srcs = ["traffic_light_agent.cc"],
    hdrs = ["traffic_light_agent.h"],
    deps = [
        "//common/utils/math:math",
        "//common/proto:cc_agent_status_proto",
        "//common/proto:cc_map_id_proto",
        "//common/proto:cc_map_proto",
        "@glog",
    ],
)

cc_library(
    name = "simulation_engine_base",
    hdrs = ["simulation_engine_base.h"],
    deps = [
        ":types",
        "//common/proto:cc_agent_status_proto",
        "//common/proto:cc_control_proto",
        "//common/proto:cc_simulation_config_proto",
        "//pnc/agents",
        "@gflags",
        "@glog",
    ],
)

cc_library(
    name = "simulation_evaluator",
    srcs = ["simulation_evaluator.cc"],
    hdrs = ["simulation_evaluator.h"],
    deps = [
        ":metric_collector",
        ":types",
        "//common/proto:cc_agent_status_proto",
        "//common/proto:cc_control_proto",
        "//common/proto:cc_map_proto",
        "//common/proto:cc_simulation_config_proto",
        "//common/proto:cc_simulation_proto",
        "//common/utils/index:grid_index",
        "//pnc/map:map_meta",
        "//pnc/map:segment",
        "//pnc/utils:vehicle",
        "@gflags",
        "@glog",
    ],
)

cc_library(
    name = "single_thread_simulation_engine",
    srcs = ["single_thread_simulation_engine.cc"],
    hdrs = ["single_thread_simulation_engine.h"],
    deps = [
        ":simulation_engine_base",
        ":vehicle_agent",
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

cc_library(
    name = "types",
    hdrs = ["types.h"],
    deps = [
        "//common/proto:cc_agent_response_proto",
        "//common/proto:cc_agent_status_proto",
        "//common/proto:cc_control_proto",
    ],
)

cc_library(
    name = "mocked_obstacle_agent",
    srcs = ["mocked_obstacle_agent.cc"],
    hdrs = ["mocked_obstacle_agent.h"],
    deps = [
        ":types",
        "//common/proto:cc_agent_status_proto",
        "//common/proto:cc_map_id_proto",
        "//common/proto:cc_map_proto",
        "//common/proto:cc_perception_proto",
        "//common/proto:cc_simulation_config_proto",
        "//common/proto:cc_vehicle_params_proto",
        "//common/utils/file:path",
        "//pnc/map:map_meta",
        "//pnc/utils:vehicle",
        "@glog",
    ],
)
