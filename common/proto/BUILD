package(default_visibility = ["//visibility:public"])

cc_proto_library(
    name = "cc_geometry_proto",
    deps = [
        ":geometry_proto",
    ],
)

proto_library(
    name = "geometry_proto",
    srcs = ["geometry.proto"],
)

cc_proto_library(
    name = "cc_transform_proto",
    deps = [
        ":transform_proto",
    ],
)

proto_library(
    name = "transform_proto",
    srcs = ["transform.proto"],
)

cc_proto_library(
    name = "cc_display_proto",
    deps = [
        ":display_proto",
    ],
)

proto_library(
    name = "display_proto",
    srcs = ["display.proto"],
    deps = [
        ":geometry_proto",
    ],
)

cc_proto_library(
    name = "cc_status_proto",
    deps = [
        ":status_proto",
    ],
)

proto_library(
    name = "status_proto",
    srcs = ["status.proto"],
)

cc_proto_library(
    name = "cc_object_labeling_3d_proto",
    deps = [
        ":object_labeling_3d_proto",
    ],
)

proto_library(
    name = "object_labeling_3d_proto",
    srcs = ["object_labeling_3d.proto"],
    deps = [
        ":geometry_proto",
        ":perception_proto",
    ],
)

cc_proto_library(
    name = "cc_perception_proto",
    deps = [
        ":perception_proto",
    ],
)

proto_library(
    name = "perception_proto",
    srcs = ["perception.proto"],
    deps = [
        ":geometry_proto",
        ":map_id_proto",
        ":map_traffic_light_proto",
    ],
)

cc_proto_library(
    name = "cc_perception_evaluation_proto",
    deps = [
        ":perception_evaluation_proto",
    ],
)

proto_library(
    name = "perception_evaluation_proto",
    srcs = ["perception_evaluation.proto"],
    deps = [
        ":perception_proto",
        ":geometry_proto",
    ],
)

cc_proto_library(
    name = "cc_map_proto",
    deps = [
        ":map_proto",
    ],
)

proto_library(
    name = "map_proto",
    srcs = ["map.proto"],
    deps = [
        ":map_lane_proto",
        ":map_traffic_light_proto",
    ],
)

cc_proto_library(
    name = "cc_map_id_proto",
    deps = [
        "map_id_proto",
    ],
)

proto_library(
    name = "map_id_proto",
    srcs = ["map_id.proto"],
)

cc_proto_library(
    name = "cc_map_lane_proto",
    deps = [
        ":map_lane_proto",
    ],
)

proto_library(
    name = "map_lane_proto",
    srcs = ["map_lane.proto"],
    deps = [
        ":geometry_proto",
        ":map_id_proto",
    ],
)

cc_proto_library(
    name = "cc_map_traffic_light_proto",
    deps = [
        ":map_traffic_light_proto",
    ],
)

proto_library(
    name = "map_traffic_light_proto",
    srcs = ["map_traffic_light.proto"],
    deps = [
        ":geometry_proto",
        ":map_id_proto",
    ],
)

cc_proto_library(
    name = "cc_route_proto",
    deps = [
        ":route_proto",
    ],
)

proto_library(
    name = "route_proto",
    srcs = ["route.proto"],
    deps = [
        ":geometry_proto",
        ":transform_proto",
    ],
)

cc_proto_library(
    name = "cc_control_proto",
    deps = [
        ":control_proto",
    ],
)

proto_library(
    name = "control_proto",
    srcs = ["control.proto"],
)

cc_proto_library(
    name = "cc_vehicle_status_proto",
    deps = [
        ":vehicle_status_proto",
    ],
)

proto_library(
    name = "vehicle_status_proto",
    srcs = ["vehicle_status.proto"],
    deps = [
        ":geometry_proto",
        ":transform_proto",
    ],
)

cc_proto_library(
    name = "cc_playback_proto",
    deps = [
        ":playback_proto",
    ],
)

proto_library(
    name = "playback_proto",
    srcs = ["playback.proto"],
)

cc_proto_library(
    name = "cc_agent_status_proto",
    deps = [
        ":agent_status_proto",
    ],
)

proto_library(
    name = "agent_status_proto",
    srcs = ["agent_status.proto"],
    deps = [
        ":control_proto",
        ":geometry_proto",
        ":map_id_proto",
        ":map_traffic_light_proto",
        ":perception_proto",
        ":transform_proto",
        ":vehicle_status_proto",
    ],
)

cc_proto_library(
    name = "cc_vehicle_params_proto",
    deps = [
        ":vehicle_params_proto",
    ],
)

proto_library(
    name = "vehicle_params_proto",
    srcs = ["vehicle_params.proto"],
)

cc_proto_library(
    name = "cc_simulation_proto",
    deps = [
        ":simulation_proto",
    ],
)

proto_library(
    name = "simulation_proto",
    srcs = ["simulation.proto"],
    deps = [
        ":agent_metric_proto",
        ":agent_status_proto",
        ":display_proto",
        ":perception_proto",
    ],
)

cc_proto_library(
    name = "cc_agent_metric_proto",
    deps = [
        ":agent_metric_proto",
    ],
)

proto_library(
    name = "agent_metric_proto",
    srcs = ["agent_metric.proto"],
)

cc_proto_library(
    name = "cc_agent_debug_data_proto",
    deps = [
        ":agent_debug_data_proto",
    ],
)

proto_library(
    name = "agent_debug_data_proto",
    srcs = ["agent_debug_data.proto"],
    deps = [
        ":display_proto",
    ],
)

cc_proto_library(
    name = "cc_simulation_config_proto",
    deps = [
        ":simulation_config_proto",
    ],
)

proto_library(
    name = "simulation_config_proto",
    srcs = ["simulation_config.proto"],
)

cc_proto_library(
    name = "cc_agent_comms_proto",
    deps = [
        ":agent_comms_proto",
    ],
)

proto_library(
    name = "agent_comms_proto",
    srcs = ["agent_comms.proto"],
    deps = [
        ":agent_status_proto",
        ":control_proto",
        ":display_proto",
        ":vehicle_status_proto",
    ],
)

cc_proto_library(
    name = "cc_agent_response_proto",
    deps = [
        ":agent_response_proto",
    ],
)

proto_library(
    name = "agent_response_proto",
    srcs = ["agent_response.proto"],
    deps = [
        ":agent_debug_data_proto",
        ":control_proto",
        ":display_proto",
    ],
)
