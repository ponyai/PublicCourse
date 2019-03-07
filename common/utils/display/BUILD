package(default_visibility = ["//visibility:public"])

load("//utils/bazel:qt.bzl", "cc_qt_library")

cc_qt_library(
    name = "painter_widget_base",
    srcs = ["painter_widget_base.cc"],
    hdrs = ["painter_widget_base.h"],
    deps = [
        ":gl_painter",
        ":layer_manager",
        "//common/utils/display:camera",
        "//common/utils/display:font_renderer",
        "//common/utils/display:painter_widget_controller",
        "//common/utils/display:qt_macro",
        "//common/utils/gl/qt:context",
        "@qt//:qt_core",
        "@qt//:qt_gui",
        "@qt//:qt_widgets",
    ],
)

cc_library(
    name = "gl_painter",
    srcs = ["gl_painter.cc"],
    hdrs = ["gl_painter.h"],
    deps = [
        "//common/utils/display:color",
        "//common/utils/display:font_renderer",
        "//common/utils/display:primitive_painter",
        "//common/utils/gl:context_and_functions",
        "//common/utils/gl:primitives",
        "//common/utils/gl:texture",
        "//common/utils/gl_support:base",
        "//common/utils/gl_support:mesh_model",
        "//common/utils/math",
        "@glm",
        "@qt//:qt_widgets",
    ],
)

cc_library(
    name = "layer",
    hdrs = [
        "layer.h",
    ],
    deps = [
        ":gl_painter",
        "//common/utils/display:font_renderer",
        "//common/utils/gl:context_and_functions",
    ],
)

cc_library(
    name = "layer_manager",
    srcs = ["layer_manager.cc"],
    hdrs = ["layer_manager.h"],
    deps = [
        ":layer",
        "//common/utils/containers:utils",
    ],
)

cc_library(
    name = "camera",
    srcs = ["camera.cc"],
    hdrs = ["camera.h"],
    deps = [
        "//common/utils/math",
        "@glm",
        "@glog",
        "@opengl",
    ],
)

cc_library(
    name = "color",
    srcs = ["color.cc"],
    hdrs = ["color.h"],
    deps = [
        "//common/proto:cc_display_proto",
        "//common/utils/math:dimension",
        "@eigen",
    ],
)

cc_library(
    name = "font_renderer",
    srcs = ["font_renderer.cc"],
    hdrs = ["font_renderer.h"],
    deps = [
        ":color",
        "//common/utils/common:optional",
        "//common/utils/math",
        "@opengl",
        "@qt//:qt_gui",
        "@qt//:qt_widgets",
    ],
)

cc_library(
    name = "rendering_controllers",
    srcs = ["rendering_controllers.cc"],
    hdrs = ["rendering_controllers.h"],
    deps = [
        ":camera",
        "//common/utils/common:defines",
        "@qt//:qt_opengl",
        "@qt//:qt_widgets",
    ],
)

cc_library(
    name = "painter_widget_controller",
    srcs = ["painter_widget_controller.cc"],
    hdrs = ["painter_widget_controller.h"],
    deps = [
        ":camera",
        ":rendering_controllers",
        "@qt//:qt_opengl",
        "@qt//:qt_widgets",
    ],
)

cc_library(
    name = "primitive_painter",
    srcs = ["primitive_painter.cc"],
    hdrs = [
        "primitive_painter.h",
        "primitive_painter-inl.h",
    ],
    deps = [
        ":color",
        "//common/utils/common:defines",
        "//common/utils/containers:array_view",
        "//common/utils/containers:fixed_array",
        "//common/utils/gl:context_and_functions",
        "//common/utils/gl:primitives",
        "//common/utils/gl:texture",
        "//common/utils/gl:vertexbuffer",
        "//common/utils/math",
        "//common/utils/math:dimension",
        "//common/utils/math/transform",
        "@glog",
        "@opengl",
    ],
)

cc_library(
    name = "qt_macro",
    hdrs = ["qt_macro.h"],
)
