licenses(["notice"])  # BSD 2-clause

cc_library(
    name = "fmt",
    srcs = glob(["fmt/*.cc"]),
    hdrs = glob(["fmt/*.h"]),
    includes = ["."],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "test_utils",
    srcs = [
        "test/gtest-extra.cc",
        "test/util.cc",
    ],
    hdrs = [
        "fmt/format.cc",
        "fmt/posix.cc",
        "fmt/printf.cc",
        "test/gtest-extra.h",
        "test/mock-allocator.h",
        "test/posix-mock.h",
        "test/test-assert.h",
        "test/util.h",
    ],
    copts = [
        "-DFMT_USE_FILE_DESCRIPTORS=1",
    ],
    deps = [
        ":fmt",
        "@googletest//:gtest",
    ],
)

[cc_test(
    name = test_file.split("/")[-1].replace(".cc", ""),
    srcs = [test_file],
    copts = [
        "-Wno-unused-variable",
        "-DFMT_USE_FILE_DESCRIPTORS=1",
    ],
    deps = [
        ":fmt",
        ":test_utils",
        "@googletest//:gtest_main",
    ],
) for test_file in glob(["test/*-test.cc"])]
