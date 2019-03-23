licenses(["restricted"])  # GNU LGPL v3, portion GNU GPL v3

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "qt_core",
    srcs = ["lib/libQt5Core.so"],
    hdrs = glob(["include/qt/QtCore/**"]),
    includes = ["include/qt"],
)

cc_library(
    name = "qt_gui",
    srcs = ["lib/libQt5Gui.so"],
    hdrs = glob(["include/qt/QtGui/**"]),
    includes = ["include/qt"],
    deps = [
        ":qt_core",
    ],
)

cc_library(
    name = "qt_widgets",
    srcs = ["lib/libQt5Widgets.so"],
    hdrs = glob(["include/qt/QtWidgets/**"]),
    includes = ["include/qt"],
    deps = [
        ":qt_core",
        ":qt_gui",
    ],
)

cc_library(
    name = "qt_opengl",
    srcs = ["lib/libQt5OpenGL.so"],
    hdrs = glob(["include/qt/QtOpenGL/**"]),
    includes = ["include/qt"],
    deps = [
        ":qt_gui",
        ":qt_widgets",
    ],
)
