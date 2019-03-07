package(default_visibility = ["//visibility:public"])

# Note that you only need a cc_proto_library for the protos
# you directly use in your code - a cc_proto_library converts
# an entire proto graph.
cc_proto_library(
    name = "cc_geometry_proto",
    deps = [":geometry_proto"],
)

# Conventions:
# 1. One proto_library rule per .proto file.
# 2. A file named foo.proto will be in a rule named foo_proto.
proto_library(
    name = "geometry_proto",
    srcs = ["geometry.proto"],
)

