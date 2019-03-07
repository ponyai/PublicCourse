package(default_visibility = ["//visibility:public"])

# required
cc_library(
    name = "base",
    hdrs = ["base.h"],
    deps = [
        "@opengl",
    ],
)

cc_library(
    name = "binding_interface",
    hdrs = ["binding_interface.h"],
)

# required
cc_library(
    name = "context_and_functions",
    srcs = ["context.cc"],
    hdrs = [
        "context.h",
        "functions.h",
        "rendering_state.h",
    ],
    deps = [
        ":base",
        "@glog",
        "@opengl",
    ],
)

cc_library(
    name = "object",
    srcs = ["object.cc"],
    hdrs = ["object.h"],
    deps = [
        ":context_and_functions",
        "//common/utils/common:defines",
        "@glog",
        "@opengl",
    ],
)

cc_library(
    name = "primitives",
    srcs = [
        "basic_primitives.cc",
        "composite_primitives.cc",
        "primitive.cc",
    ],
    hdrs = [
        "basic_primitives.h",
        "composite_primitives.h",
        "primitive.h",
    ],
    deps = [
        ":context_and_functions",
        ":texture",
        ":vertexbuffer",
        "//common/utils/common:defines",
        "//common/utils/display:color",
        "//common/utils/image:image_header",
        "@glm",
        "@glog",
        "@opengl",
    ],
)

# required
cc_library(
    name = "texture",
    srcs = ["texture.cc"],
    hdrs = ["texture.h"],
    deps = [
        ":context_and_functions",
        ":object",
        "//common/utils/common:defines",
        "//common/utils/image:image_header",
        "//common/utils/image/codec",
        "//common/utils/image/codec:jpeg",
        "//common/utils/image/codec:netpbm",
        "//common/utils/image/codec:png",
        "@glog",
    ],
)

cc_library(
    name = "geometry_utils",
    srcs = ["geometry_utils.cc"],
    hdrs = ["geometry_utils.h"],
    deps = [
        ":context_and_functions",
        "//common/utils/common:defines",
        "//common/utils/containers:array_view",
        "//common/utils/containers:fixed_array",
        "//common/utils/display:color",
        "//common/utils/math:dimension",
        "@glm",
        "@glog",
        "@opengl",
    ],
)

cc_library(
    name = "vertexbuffer",
    srcs = ["vertexbuffer.cc"],
    hdrs = ["vertexbuffer.h"],
    deps = [
        ":binding_interface",
        ":context_and_functions",
        ":geometry_utils",
        ":object",
        ":texture",
        "//common/utils/common:defines",
        "//common/utils/display:color",
        "@glog",
        "@googletest//:gtest",
        "@opengl",
    ],
)
