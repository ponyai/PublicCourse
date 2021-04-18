package(default_visibility = ["//visibility:public"])

cc_proto_library(
    name = "cc_simulation_config_proto",
    deps = [
        ":simulation_config_proto",
    ],
)

proto_library(
    name = "simulation_config_proto",
    srcs = ["simulation_config.proto"],
    deps = [
        "//common/proto:route_proto",
    ],
)
