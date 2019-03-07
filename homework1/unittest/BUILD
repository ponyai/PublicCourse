package(default_visibility = ["//visibility:public"])

cc_library(
    name = "car_lib",
    srcs = ["car.cc"],
    hdrs = ["car.h"],
)

cc_test(
    name = "car_test",
    srcs = ["car_test.cc"],
    deps = [
        ":car_lib",
        "//utils/testing:test_main",
    ],
)
