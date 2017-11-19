package(default_visibility = ["//visibility:public"])

cc_library(
    name = "Debug",
    srcs = [
        "DebugConsole.cpp",
        "DebugLog.cpp",
        "Dump.cpp",
        "WebDebug.cpp",
        "WebDebugTemplate.cpp",
    ],
    hdrs = [
        "Console.h",
        "DebugLog.h",
        "Dump.h",
        "WebDebug.h",
        "WebDebugTemplate.h",
    ],
    data = [
        ":static_content",
    ],
    deps = [
        "//Base",
        "//System",
        "//external:gflags",
        "@webby//:webby",
    ],
)

cc_library(
    name = "Synchroniser",
    srcs = ["Synchroniser.cpp"],
    hdrs = ["Synchroniser.h"],
    deps = [
        ":Debug",
        "//Base",
        "//Core",
    ],
)

filegroup(
    name = "static_content",
    srcs = glob([
        "Web/**",
    ]),
)
