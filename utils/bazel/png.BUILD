licenses(["notice"])  # Zlib/libpng(BSD/MIT-like)

package(default_visibility = ["//visibility:private"])

cc_library(
    name = "png",
    srcs = [
        "png.c",
        "pngerror.c",
        "pngget.c",
        "pngmem.c",
        "pngpread.c",
        "pngread.c",
        "pngrio.c",
        "pngrtran.c",
        "pngrutil.c",
        "pngset.c",
        "pngtrans.c",
        "pngwio.c",
        "pngwrite.c",
        "pngwtran.c",
        "pngwutil.c",
    ],
    hdrs = [
        "png.h",
        "pngconf.h",
    ],
    includes = ["."],
    visibility = ["//visibility:public"],
    deps = [
        "@zlib//:zlib",
    ],
)
