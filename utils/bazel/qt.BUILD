licenses(["restricted"])  # GNU LGPL v3, portion GNU GPL v3

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "qt_core",
    srcs = ["lib/x86_64-linux-gnu/libQt5Core.so"],
    hdrs = glob(["include/x86_64-linux-gnu/qt5/QtCore/**"]),
    includes = ["include/x86_64-linux-gnu/qt5"],
)

cc_library(
    name = "qt_gui",
    srcs = ["lib/x86_64-linux-gnu/libQt5Gui.so"],
    hdrs = glob(["include/x86_64-linux-gnu/qt5/QtGui/**"]),
    includes = ["include/x86_64-linux-gnu/qt5"],
    deps = [
        ":qt_core",
    ],
)

cc_library(
    name = "qt_widgets",
    srcs = ["lib/x86_64-linux-gnu/libQt5Widgets.so"],
    hdrs = glob(["include/x86_64-linux-gnu/qt5/QtWidgets/**"]),
    includes = ["include/x86_64-linux-gnu/qt5"],
    deps = [
        ":qt_core",
        ":qt_gui",
    ],
)

cc_library(
    name = "qt_opengl",
    srcs = ["lib/x86_64-linux-gnu/libQt5OpenGL.so"],
    hdrs = glob(["include/x86_64-linux-gnu/qt5/QtOpenGL/**"]),
    includes = ["include/x86_64-linux-gnu/qt5"],
    deps = [
        ":qt_gui",
        ":qt_widgets",
    ],
)
