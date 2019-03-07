licenses(["notice"])  # zLib(BSD/MIT-like)

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "zlib",
    srcs = [
        # Private hdrs:
        "crc32.h",
        "deflate.h",
        "gzguts.h",
        "inffast.h",
        "inffixed.h",
        "inflate.h",
        "inftrees.h",
        "trees.h",
        "zutil.h",
        # srcs:
        "adler32.c",
        "compress.c",
        "crc32.c",
        "deflate.c",
        "gzclose.c",
        "gzlib.c",
        "gzread.c",
        "gzwrite.c",
        "inflate.c",
        "infback.c",
        "inftrees.c",
        "inffast.c",
        "trees.c",
        "uncompr.c",
        "zutil.c",
    ],
    hdrs = [
        "zconf.h",
        "zlib.h",
    ],
    copts = [
        "-Wno-implicit-function-declaration",
    ],
    includes = ["."],
)
