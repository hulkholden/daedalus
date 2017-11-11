cc_library(
    name = "System",
    srcs = [
        "CompressedStream.cpp",
        "DataSink.cpp",
        "IO.cpp",
        "Paths.cpp",
    ] + select({
        "//:darwin": [
            "Posix/Condition.cpp",
            "Posix/IO.cpp",
            "Posix/Thread.cpp",
            "Posix/Timing.cpp",
        ],
        "//:windows": [
            "Windows/Condition.cpp",
            "Windows/IO.cpp",
            "Windows/Thread.cpp",
            "Windows/Timing.cpp",
        ],
    }),
    hdrs = [
        "CompressedStream.h",
        "Condition.h",
        "DataSink.h",
        "Endian.h",
        "IO.h",
        "Mutex.h",
        "Paths.h",
        "SpinLock.h",
        "Thread.h",
        "Timing.h",
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

cc_test(
    name = "IO_test",
    size = "small",
    srcs = [
        "IO_test.cpp",
    ],
    visibility = ["//visibility:private"],
    deps = [
        ":System",
        # Shouldn't need this.
        "//Debug",
        "@com_google_googletest//:gtest_main",
    ],
)
