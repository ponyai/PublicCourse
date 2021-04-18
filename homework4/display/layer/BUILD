package(default_visibility = ["//visibility:public"])

cc_library(
    name = "layers",
    srcs = [
        "intensity_map_layer.cc",
        "lane_layer.cc",
        "road_graph_layer.cc",
        "route_layer.cc",
    ],
    hdrs = [
        "intensity_map_layer.h",
        "lane_layer.h",
        "layers_common.h",
        "road_graph_layer.h",
        "road_graph_layer_base.h",
        "route_layer.h",
    ],
    deps = [
        "//common/proto:cc_map_lane_proto",
        "//common/proto:cc_map_proto",
        "//common/utils/containers:utils",
        "//common/utils/display:layer",
        "//common/utils/file",
        "//common/utils/gl:primitives",
        "//homework4/display:map_gl_painter",
        "//homework4/display:user_interface_data",
        "//homework4/map:map_lib",
    ],
)
