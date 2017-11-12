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
        "AtomicPrimitives.h",
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
    visibility = ["//visibility:public"],
    deps = [
        "//:stdafx",
        "@com_google_absl//absl/strings",
        "@zlib//:minizip",
        "@zlib//:zlib",
    ],
)

cc_library(
    name = "SystemInit",
    srcs = ["SystemInit.cpp"],
    hdrs = ["SystemInit.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":System",
        "//:stdafx",
        "//Core",
        "//Debug",
        "//Debug:Synchroniser",
        "//Graphics",
        "//HLEGraphics",
        "//Input",
        "//Interface",
        "//OSHLE",
        "//Plugins",
        "//SysGL",
        "//Utility",
    ],
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
