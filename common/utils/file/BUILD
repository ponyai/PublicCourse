package(default_visibility = ["//visibility:public"])

cc_library(
    name = "file_header",
    hdrs = [
        "file.h",
    ],
    deps = [
        "//common/utils/common:proto_utils",
        "//common/utils/status",
        "//common/utils/strings:utils",
        "@boost//:algorithm",
        "@boost//:filesystem",
        "@boost//:lexical_cast",
    ],
)

cc_library(
    name = "path",
    srcs = [
        "path.cc",
    ],
    hdrs = [
        "path.h",
    ],
    deps = [
        "//common/utils/common:optional",
        "//common/utils/strings:utils",
        "@boost//:filesystem",
    ],
)

cc_library(
    name = "file",
    srcs = [
        "file.cc",
    ],
    deps = [
        ":gzip_file",
        ":path",
        "//common/utils/common:defines",
        "//common/utils/common:proto_utils",
        "//common/utils/strings:format",
        "@boost//:algorithm",
        "@boost//:filesystem",
        "@glog//:glog",
    ],
)

cc_library(
    name = "gzip_file_header",
    hdrs = [
        "gzip_file.h",
    ],
    deps = [
        ":file_header",
    ],
)

cc_library(
    name = "gzip_file",
    srcs = [
        "gzip_file.cc",
    ],
    deps = [
        ":gzip_file_header",
        "//common/utils/status",
        "//common/utils/strings:utils",
        "@zlib//:zlib",
    ],
)

cc_library(
    name = "mmap_file",
    hdrs = [
        "mmap_file.h",
    ],
    deps = [
        ":file_header",
        "//common/utils/containers:array_view",
    ],
)

