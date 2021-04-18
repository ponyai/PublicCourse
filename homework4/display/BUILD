package(default_visibility = ["//visibility:public"])

load("//utils/bazel:qt.bzl", "cc_qt_library")

cc_qt_library(
    name = "main_window",
    hdrs = ["main_window.h"],
    deps = [
        ":map_lane_dock_widget",
        ":map_painter_widget",
        "@qt//:qt_core",
        "@qt//:qt_widgets",
    ],
)

cc_qt_library(
    name = "map_painter_widget",
    srcs = ["map_painter_widget.cc"],
    hdrs = ["map_painter_widget.h"],
    deps = [
        ":map_gl_painter",
        ":user_interface_data",
        "//common/utils/display:camera",
        "//common/utils/display:font_renderer",
        "//common/utils/display:layer_manager",
        "//common/utils/display:painter_widget_base",
        "//common/utils/display:painter_widget_controller",
        "//common/utils/display:qt_macro",
        "//common/utils/gl/qt:context",
        "//homework4/display/layer:layers",
        "@qt//:qt_core",
        "@qt//:qt_gui",
        "@qt//:qt_widgets",
    ],
)

cc_library(
    name = "map_gl_painter",
    srcs = ["map_gl_painter.cc"],
    hdrs = ["map_gl_painter.h"],
    deps = [
        ":gl_layers",
        ":user_interface_data",
        "//common/proto:cc_map_proto",
        "//common/proto:cc_route_proto",
        "//common/utils/display:color",
        "//common/utils/display:font_renderer",
        "//common/utils/display:gl_painter",
        "//common/utils/display:primitive_painter",
        "//common/utils/gl:context_and_functions",
        "//common/utils/gl:primitives",
        "//common/utils/gl:texture",
        "//common/utils/gl_support:base",
        "//common/utils/gl_support:mesh_model",
        "//common/utils/math",
        "//homework4/map:map_lib",
        "@glm",
        "@qt//:qt_widgets",
    ],
)

cc_library(
    name = "user_interface_data",
    hdrs = [
        "user_interface_data.h",
    ],
    deps = [
        "//common/utils/display:rendering_controllers",
        "//common/utils/math",
    ],
)

cc_library(
    name = "gl_layers",
    srcs = ["gl_layers.cc"],
    hdrs = ["gl_layers.h"],
)

cc_qt_library(
    name = "map_lane_dock_widget",
    srcs = ["map_lane_dock_widget.cc"],
    hdrs = ["map_lane_dock_widget.h"],
    deps = [
        "//common/proto:cc_display_proto",
        "//common/proto:cc_map_proto",
        "//common/utils/display:qt_macro",
        "//common/utils/math",
        "@gflags",
        "@qt//:qt_widgets",
    ],
)
