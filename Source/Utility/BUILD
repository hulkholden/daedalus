package(
    default_visibility = ["//visibility:public"],
)

cc_library(
    name = "Utility",
    hdrs = [
        "AtomicPrimitives.h",
        "CRC.h",
        "FastMemcpy.h",
        "Hash.h",
        "IniFile.h",
        "Profiler.h",
        "RefCounted.h",
        "Stream.h",
        "Timer.h",
    ],
    srcs = [
        "CRC.cpp",
        "FastMemcpy.cpp",
        "Hash.cpp",
        "IniFile.cpp",
        "Profiler.cpp",
        "Timer.cpp",
    ],
    deps = [
        "//:stdafx",
        "//Base",
        "//Config",
        "//Debug",
        "//Input",
        "//Math",
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
        ":Utility",
        "//:stdafx",
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
        ":Utility",
        "//:stdafx",
        "//Test:TestUtil",
        "@com_google_googletest//:gtest_main",
    ],
)

