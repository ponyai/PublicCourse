package(default_visibility = ["//visibility:public"])

cc_library(
    name = "knn_interface",
    hdrs = ["knn_interface.h"],
    deps = [
        "@eigen",
        "@glog",
    ],
)

cc_library(
    name = "knn_nanoflann",
    hdrs = ["knn_nanoflann.h"],
    deps = [
        ":knn_interface",
        "//common/utils/common:defines",
        "@nanoflann",
    ],
)

