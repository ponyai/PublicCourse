# Eigen is a C++ template library for linear algebra: vectors,
# matrices, and related algorithms.

licenses([
    # Note: Eigen is an MPL2 library that includes GPL v3 and LGPL v2.1+ code.
    #       We've taken special care to not reference any restricted code.
    "reciprocal",  # MPL2
    "notice",      # Portions BSD
])

exports_files(["COPYING.MPL2"])

cc_library(
    name = "eigen",
    hdrs = glob([
        "Eigen/*",
        "Eigen/**/*.h",
    ]),
    copts = [
        "-DEIGEN_MPL2_ONLY",
    ],
    includes = ["."],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "unsupported-eigen",
    hdrs = glob([
        "unsupported/Eigen/*",
        "unsupported/Eigen/**/*.h",
    ]),
    copts = [
        "-DEIGEN_MPL2_ONLY",
    ],
    includes = ["."],
    visibility = ["//visibility:public"],
)

