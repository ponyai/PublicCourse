package(default_visibility = ["//visibility:public"])

cc_library(
    name = "server_client_defines",
    hdrs = ["server_client_defines.h"],
)

cc_library(
    name = "multi_process_simulation_engine",
    srcs = ["multi_process_simulation_engine.cc"],
    hdrs = ["multi_process_simulation_engine.h"],
    deps = [
        ":server_client_defines",
        ":tcp_agent_client",
        ":tcp_simulation_server",
        "//common/proto:cc_agent_response_proto",
        "//common/proto:cc_agent_comms_proto",
        "//common/proto:cc_agent_status_proto",
        "//common/proto:cc_control_proto",
        "//pnc/parallelism:multi_process",
        "//pnc/simulation:simulation_engine_base",
    ],
)

cc_library(
    name = "net_client_interface",
    hdrs = ["net_client_interface.h"],
)

cc_library(
    name = "net_server_interface",
    hdrs = ["net_server_interface.h"],
)

cc_library(
    name = "tcp_agent_client",
    srcs = ["tcp_agent_client.cc"],
    hdrs = ["tcp_agent_client.h"],
    deps = [
        ":net_client_interface",
        ":server_client_defines",
        ":tcp_connection",
        "//common/proto:cc_agent_comms_proto",
        "//common/proto:cc_agent_status_proto",
        "//common/proto:cc_control_proto",
        "//common/proto:cc_simulation_config_proto",
        "//common/utils/containers:array_view",
        "//common/utils/status",
        "//pnc/simulation:vehicle_agent",
        "//pnc/simulation:vehicle_agent_factory",
        "@gflags",
    ],
)

cc_library(
    name = "tcp_simulation_server",
    srcs = ["tcp_simulation_server.cc"],
    hdrs = ["tcp_simulation_server.h"],
    deps = [
        ":message_receiver",
        ":net_server_interface",
        ":server_client_defines",
        ":tcp_agent_client",
        ":tcp_connection",
        "//common/proto:cc_agent_response_proto",
        "//common/proto:cc_agent_comms_proto",
        "//common/proto:cc_agent_status_proto",
        "//common/proto:cc_control_proto",
        "//common/utils/containers:array_view",
        "//common/utils/status",
        "//pnc/parallelism:thread_pool",
        "//pnc/simulation:manual_test_agent",
        "//pnc/simulation:types",
        "@gflags",
    ],
)

cc_library(
    name = "tcp_connection",
    srcs = ["tcp_connection.cc"],
    hdrs = ["tcp_connection.h"],
    deps = [
        ":net_client_interface",
        ":server_client_defines",
        "//common/utils/containers:array_view",
        "//common/utils/status",
        "@gflags",
    ],
)

cc_library(
    name = "message_receiver",
    hdrs = ["message_receiver.h"],
    srcs = ["message_receiver.cc"],
    deps = [
        ":server_client_defines",
        ":tcp_connection",
        "//common/proto:cc_agent_comms_proto",
        "//common/utils/common:optional",
        "//common/utils/containers:array_view",
        "//common/utils/status",
        "//common/utils/common:mutex",
        "@gflags",
    ],
)
