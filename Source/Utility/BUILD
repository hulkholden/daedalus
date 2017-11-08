package(
    default_visibility = ["//visibility:public"],
)

cc_library(
    name = "Utility",
    hdrs = [
        "AtomicPrimitives.h",
        "CRC.h",
        "Cond.h",
        "DataSink.h",
        "Endian.h",
        "FastMemcpy.h",
        "Hash.h",
        "IniFile.h",
        "Mutex.h",
        "Profiler.h",
        "ROMFile.h",
        "ROMFileCache.h",
        "ROMFileCompressed.h",
        "ROMFileUncompressed.h",
        "RefCounted.h",
        "SpinLock.h",
        "Stream.h",
        "Thread.h",
        "Timer.h",
        "Timing.h",
    ],
    deps = [
        "//:stdafx",
        "//Config",
        "//System",
    ],
)

cc_library(
    name = "UtilityCpp",
    srcs = [
        "CRC.cpp",
        "DataSink.cpp",
        "FastMemcpy.cpp",
        "Hash.cpp",
        "IniFile.cpp",
        "Profiler.cpp",
        "ROMFile.cpp",
        "ROMFileCache.cpp",
        "ROMFileCompressed.cpp",
        "ROMFileUncompressed.cpp",
        "Timer.cpp",
    ],
    deps = [
        ":Utility",
        "//:stdafx",
        "//Base",
        "//Config",
        "//Debug",
        "//Input",
        "//Interface",
        "//Math",
        "//OSHLE",
        "//System",
        "//third_party/gtest",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "FastMemcpy_test",
    size = "small",
    srcs = [
        "FastMemcpy_test.cpp",
    ],
    visibility = ["//visibility:private"],
    deps = [
        ":UtilityCpp",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "IniFile_test",
    size = "small",
    srcs = [
        "IniFile_test.cpp",
    ],
    visibility = ["//visibility:private"],
    deps = [
        ":UtilityCpp",
        "@com_google_googletest//:gtest_main",
    ],
)

