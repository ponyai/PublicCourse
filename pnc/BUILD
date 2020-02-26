package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "simulation_main",
    srcs = ["simulation_main.cc"],
    deps = [
        "//pnc/display:simulation_main_window",
        "//pnc/map:map_lib",
        "@gflags//:gflags",
        "@glog",
    ],
)

cc_binary(
    name = "replay_main",
    srcs = ["replay_main.cc"],
    deps = [
        "//pnc/display:replay_main_window",
        "//pnc/map:map_lib",
        "@gflags//:gflags",
        "@glog",
    ],
)
