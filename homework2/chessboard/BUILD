package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "opencv_example",
    srcs = ["opencv_example.cc"],
    deps = [
        "@opencv//:core",
        "@opencv//:highgui",
	"@opencv//:imgcodecs",
    ],
)

cc_binary(
    name = "distortion",
    srcs = ["distortion.cc"],
    deps = [
        "@opencv//:core",
	"@opencv//:highgui",
    ],
)
