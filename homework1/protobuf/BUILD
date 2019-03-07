package(default_visibility = ["//visibility:public"])

cc_library(
    name = "canvas_lib",
    srcs = ["canvas.cc"],
    hdrs = ["canvas.h"],
    deps = [
        "//homework1/protobuf/proto:cc_geometry_proto",
        "@glog",
    ],
)

cc_test(
    name = "canvas_test",
    srcs = ["canvas_test.cc"],
    deps = [
        ":canvas_lib",
        "//utils/testing:test_main",
    ],
)

