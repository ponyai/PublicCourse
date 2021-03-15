package(default_visibility = ["//visibility:public"])

cc_library(
    name = "obstacle",
    hdrs = ["obstacle.h"],
    deps = [
        "//common/utils/math",
    ]
)

cc_library(
    name = "single_frame_detector",
    srcs = ["single_frame_detector.cc"],
    hdrs = ["single_frame_detector.h"],
    deps = [
	":obstacle",
        "//common/utils/math",
        "//common/utils/math/transform",
        "//homework2:pointcloud",
        "@eigen//:eigen",
        "@glog//:glog",
    ],
)

cc_library(
    name = "pointcloud_viewer",
    srcs = ["pointcloud_viewer.cc"],
    hdrs = ["pointcloud_viewer.h"],
    deps = [
	":obstacle",
        ":single_frame_detector",
        "//common/proto:cc_object_labeling_3d_proto",
        "//common/utils/display:painter_widget_base",
        "//common/utils/display:qt_macro",
        "//common/utils/file:path",
        "//common/utils/gl/qt:context",
        "//common/utils/strings:format",
        "@glog//:glog",
        "@qt//:qt_core",
        "@qt//:qt_widgets",
    ],
)

cc_binary(
    name = "single_frame_detector_main",
    srcs = ["single_frame_detector_main.cc"],
    deps = [
        ":pointcloud_viewer",
        ":single_frame_detector",
    ],
)
