licenses(["notice"])  # BSD 2-clause

COPTS = ["-Wno-unused-parameter"]

cc_library(
    name = "nanoflann",
    hdrs = [
        "examples/KDTreeVectorOfVectorsAdaptor.h",
        "include/nanoflann.hpp",
    ],
    copts = COPTS,
    includes = [
        "examples",
        "include",
    ],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "nanoflann_test",
    srcs = ["tests/test_main.cpp"],
    copts = COPTS,
    deps = [
        ":nanoflann",
        "@googletest//:gtest",
    ],
)
