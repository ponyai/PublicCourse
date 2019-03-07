package(default_visibility = ["//visibility:public"])

load("@bazel_skylib//lib:selects.bzl", "selects")
load(":cc_toolchain_config.bzl", "cc_toolchain_config")

# Adapted from the toolchain BUILD file shipped with bazel.

# This is the entry point for --crosstool_top.  Toolchains are found
# by lopping off the name of --crosstool_top and searching for
# "cc-compiler-${CPU}" in this BUILD file, where CPU is the target CPU
# specified in --cpu.
#
# This file group should include
#   * all cc_toolchain targets supported
#   * all file groups that said cc_toolchain might refer to,
# including the default_grte_top setting in the CROSSTOOL
# protobuf.
alias(
    name = "toolchain",
    actual = "//external:cc_toolchain",
)

config_setting(
    name = "dbg_enabled",
    values = {
        "compilation_mode": "dbg",
    },
)

config_setting(
    name = "fastbuild_enabled",
    values = {
        "compilation_mode": "fastbuild",
    },
)

config_setting(
    name = "opt_enabled",
    values = {
        "compilation_mode": "opt",
    },
)

config_setting(
    name = "asan_enabled",
    values = {
        "features": "asan",
    },
)

config_setting(
    name = "tsan_enabled",
    values = {
        "features": "tsan",
    },
)

config_setting(
    name = "msan_enabled",
    values = {
        "features": "msan",
    },
)

config_setting(
    name = "ubsan_enabled",
    values = {
        "features": "ubsan",
    },
)

config_setting(
    name = "cuda_enabled",
    define_values = {
        "enable_cuda": "true",
    },
)

# Hardcoded toolchain, legacy behaviour.
cc_toolchain_suite(
    name = "default",
    toolchains = {
        # Bazel 0.18.1 and earlier versions use the following line.
        "k8|compiler": ":cc-compiler-local",
        # Bazel 0.23.2 requires the following line.
        "k8": ":cc-compiler-local",
    },
)

alias(
    name = "malloc",
    actual = select({
        "//conditions:default": "@bazel_tools//tools/cpp:malloc",
    }),
)

cc_toolchain_config(
    name = "local_linux",
    cpu = "k8",
)

cc_toolchain(
    name = "cc-compiler-local",
    all_files = ":empty",
    compiler_files = ":empty",
    dwp_files = ":empty",
    linker_files = ":empty",
    objcopy_files = ":empty",
    strip_files = ":empty",
    supports_param_files = 1,
    toolchain_config = ":local_linux",
    toolchain_identifier = "local_linux",
)

filegroup(name = "toolchain_category")

filegroup(
    name = "empty",
    srcs = [],
)
