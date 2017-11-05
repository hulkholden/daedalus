licenses(["notice"])  #  BSD/MIT-like license

cc_library(
    name = "minizip",
    srcs = [
        "ioapi.c",
        "unzip.c",
        "zip.c",
    ],
    hdrs = glob(["*.h"]),
    visibility = ["//visibility:public"],
    deps = [
        "//third_party/zlib",
    ],
)
