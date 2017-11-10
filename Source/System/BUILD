cc_library(
    name = "System",
    srcs = [
        "CompressedStream.cpp",
        "DataSink.cpp",
        "Paths.cpp",
    ] + select({
        "//:darwin": [
            "Posix/IO.cpp",
        ],
        "//:windows": [
            "Windows/IO.cpp",
        ],
    }),
    hdrs = [
        "CompressedStream.h",
        "DataSink.h",
        "IO.h",
        "Paths.h",
    ],
    deps = [
        "//:stdafx",
        "//third_party/zlib",
        "//third_party/zlib/contrib/minizip",
        "@com_google_absl//absl/strings",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "SystemInit",
    srcs = ["SystemInit.cpp"],
    hdrs = ["SystemInit.h"],
    deps = [
        "//:stdafx",
        ":System",
        "//Core",
        "//Debug",
        "//Debug:Synchroniser",
        "//Graphics",
        "//HLEGraphics",
        "//Interface",
        "//Input",
        "//OSHLE",
        "//Plugins",
        "//SysGL",
        "//Utility",
    ],
    visibility = ["//visibility:public"],
)
