licenses(["notice"])  # BSD 3-clause

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "core",
    srcs = ["lib/libopencv_core.so"],
    hdrs = glob([
        "include/opencv2/core/*.h",
        "include/opencv2/core/*.hpp",
    ]),
    includes = ["include"],
    linkstatic = 1,
)

cc_library(
    name = "calib3d",
    srcs = ["lib/libopencv_calib3d.so"],
    hdrs = glob([
        "include/opencv2/calib3d/*.h",
        "include/opencv2/calib3d/*.hpp",
    ]),
    includes = ["include"],
    linkstatic = 1,
)

cc_library(
    name = "contrib",
    srcs = ["lib/libopencv_contrib.so"],
    hdrs = glob([
        "include/opencv2/contrib/*.h",
        "include/opencv2/contrib/*.hpp",
    ]),
    includes = ["include"],
    linkstatic = 1,
)

cc_library(
    name = "features2d",
    srcs = ["lib/libopencv_features2d.so"],
    hdrs = glob([
        "include/opencv2/features2d/*.h",
        "include/opencv2/features2d/*.hpp",
    ]),
    includes = ["include"],
    linkstatic = 1,
)

cc_library(
    name = "highgui",
    srcs = ["lib/libopencv_highgui.so"],
    hdrs = glob([
        "include/opencv2/highgui/*.h",
        "include/opencv2/highgui/*.hpp",
    ]),
    linkstatic = 1,
)

cc_library(
    name = "imgproc",
    srcs = ["lib/libopencv_imgproc.so"],
    hdrs = glob([
        "include/opencv2/imgproc/*.h",
        "include/opencv2/imgproc/*.hpp",
    ]),
    includes = ["include"],
    linkstatic = 1,
)

cc_library(
    name = "ml",
    srcs = ["lib/libopencv_ml.so"],
    hdrs = glob([
        "include/opencv2/ml/*.h",
        "include/opencv2/ml/*.hpp",
    ]),
    includes = ["include"],
    linkstatic = 1,
)

cc_library(
    name = "objdetect",
    srcs = ["lib/libopencv_objdetect.so"],
    hdrs = glob([
        "include/opencv2/objdetect/*.h",
        "include/opencv2/objdetect/*.hpp",
    ]),
    includes = ["include"],
    linkstatic = 1,
)

## MY ADAPTIONS: imgcodecs
cc_library(
  name = "imgcodecs",
  srcs = ["lib/libopencv_imgcodecs.so"],
  hdrs = glob([
      "include/opencv4/opencv2/imgcodecs/*.h",
      "include/opencv4/opencv2/imgcodecs/*.hpp",
  ]),
  includes = ["include"],
  linkstatic = 1,
)
