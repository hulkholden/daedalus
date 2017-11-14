package(
    default_visibility = ["//visibility:public"],
)

cc_library(
    name = "Utility",
    srcs = [
        "CRC.cpp",
        "FastMemcpy.cpp",
        "Hash.cpp",
        "IniFile.cpp",
        "Profiler.cpp",
        "Timer.cpp",
    ],
    hdrs = [
        "CRC.h",
        "FastMemcpy.h",
        "Hash.h",
        "IniFile.h",
        "Profiler.h",
        "RefCounted.h",
        "Stream.h",
        "Timer.h",
    ],
    deps = [
        "//Base",
        "//Config",
        "//Debug",
        "//Math",
        "//System",
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
        "//Base",
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
        "//Base",
        "//Test:TestUtil",
        "@com_google_googletest//:gtest_main",
    ],
)
