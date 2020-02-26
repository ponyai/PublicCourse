package(default_visibility = ["//visibility:public"])

cc_library(
    name = "sample_agent",
    hdrs = ["sample_agent.h"],
    deps = [
        "//pnc/simulation:vehicle_agent",
        "//pnc/simulation:vehicle_agent_factory",
        "//common/proto:cc_agent_status_proto",
        "//common/proto:cc_control_proto",
        "//common/proto:cc_simulation_proto",
        "@glog",
    ],
)
