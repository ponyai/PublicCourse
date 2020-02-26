package(default_visibility = ["//visibility:public"])

load("//utils/bazel:qt.bzl", "cc_qt_library")

cc_qt_library(
    name = "main_window",
    hdrs = ["main_window.h"],
    deps = [
        ":interactive_slider_widget",
        ":pnc_painter_widget",
        ":q_variable_dock_widget",
        ":variable_view",
        "//pnc/simulation:simulation_system",
        "@qt//:qt_core",
        "@qt//:qt_widgets",
    ],
)

cc_qt_library(
    name = "simulation_main_window",
    hdrs = ["simulation_main_window.h"],
    deps = [
        ":main_window",
        ":pnc_painter_widget",
        ":q_variable_dock_widget",
        ":variable_view",
        "//common/proto:cc_simulation_config_proto",
        "//pnc/simulation:simulation_system",
        "@qt//:qt_core",
        "@qt//:qt_widgets",
    ],
)

cc_qt_library(
    name = "replay_main_window",
    hdrs = ["replay_main_window.h"],
    deps = [
        ":main_window",
        ":pnc_painter_widget",
        ":q_variable_dock_widget",
        ":variable_view",
        "//pnc/simulation:replay_system",
        "//pnc/utils:index",
        "@qt//:qt_core",
        "@qt//:qt_widgets",
    ],
)

cc_qt_library(
    name = "pnc_painter_widget",
    srcs = ["pnc_painter_widget.cc"],
    hdrs = ["pnc_painter_widget.h"],
    deps = [
        ":pnc_gl_painter",
        ":pnc_painter_widget_controller",
        ":user_interface_data",
        "//common/proto:cc_simulation_proto",
        "//common/proto:cc_vehicle_params_proto",
        "//common/utils/display:camera",
        "//common/utils/display:font_renderer",
        "//common/utils/display:layer_manager",
        "//common/utils/display:painter_widget_base",
        "//common/utils/display:painter_widget_controller",
        "//common/utils/display:qt_macro",
        "//common/utils/gl/qt:context",
        "//pnc/display/layer:layers",
        "//pnc/utils:vehicle_status_helper",
        "@qt//:qt_core",
        "@qt//:qt_gui",
        "@qt//:qt_widgets",
    ],
)

cc_library(
    name = "pnc_gl_painter",
    srcs = ["pnc_gl_painter.cc"],
    hdrs = ["pnc_gl_painter.h"],
    deps = [
        ":gl_layers",
        "//common/proto:cc_map_proto",
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
        "//pnc/map:map_lib",
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
    name = "pnc_painter_widget_controller",
    srcs = [
        "pnc_painter_widget_controller.cc",
    ],
    hdrs = [
        "pnc_painter_widget_controller.h",
    ],
    deps = [
        "//common/utils/display:painter_widget_controller",
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
    name = "q_variable_dock_widget",
    srcs = ["q_variable_dock_widget.cc"],
    hdrs = ["q_variable_dock_widget.h"],
    deps = [
        "//common/proto:cc_display_proto",
        "//common/utils/display:qt_macro",
        "//common/utils/math",
        "@gflags",
        "@qt//:qt_widgets",
    ],
)

cc_library(
    name = "variable_view",
    srcs = ["variable_view.cc"],
    hdrs = ["variable_view.h"],
    deps = [
        "//common/proto:cc_display_proto",
        "//common/proto:cc_transform_proto",
        "//common/utils/display:color",
        "//common/utils/math/transform",
    ],
)

cc_qt_library(
    name = "flat_slider_widget",
    srcs = ["flat_slider_widget.cc"],
    hdrs = ["flat_slider_widget.h"],
    deps = [
        "//common/utils/display:camera",
        "//common/utils/display:qt_macro",
        "@qt//:qt_core",
        "@qt//:qt_widgets",
    ],
)


cc_qt_library(
    name = "interactive_slider_widget",
    srcs = ["interactive_slider_widget.cc"],
    hdrs = ["interactive_slider_widget.h"],
    deps = [
        "//pnc/display:flat_slider_widget",
        "//common/utils/file",
        "//common/utils/file:path",
        "@glog",
        "@qt//:qt_core",
        "@qt//:qt_widgets",
    ],
)
