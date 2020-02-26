package(default_visibility = ["//visibility:public"])

cc_library(
    name = "layers",
    srcs = [
        "intensity_map_layer.cc",
        "obstacle_layer.cc",
        "road_graph_layer.cc",
        "vehicle_layer.cc",
    ],
    hdrs = [
        "intensity_map_layer.h",
        "layers_common.h",
        "obstacle_layer.h",
        "road_graph_layer.h",
        "road_graph_layer_base.h",
        "simulation_system_layer_base.h",
        "vehicle_layer.h",
    ],
    deps = [
        "//common/proto:cc_agent_status_proto",
        "//common/proto:cc_map_lane_proto",
        "//common/proto:cc_map_proto",
        "//common/proto:cc_simulation_proto",
        "//common/proto:cc_vehicle_params_proto",
        "//common/utils/containers:utils",
        "//common/utils/display:layer",
        "//common/utils/file",
        "//common/utils/gl:primitives",
        "//pnc/display:pnc_gl_painter",
        "//pnc/display:user_interface_data",
        "//pnc/map:map_lib",
    ],
)
