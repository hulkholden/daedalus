package(default_visibility = ["//visibility:public"])

cc_library(
    name = "Debug",
    srcs = [
        "DebugConsole.cpp",
        "DebugConsoleImpl.cpp",
        "DebugLog.cpp",
        "Dump.cpp",
        "WebDebug.cpp",
        "WebDebugTemplate.cpp",
    ] + select({
        "//:darwin": [
            "Posix/Assert.cpp",
        ],
        "//:windows": [
            "Windows/Assert.cpp",
        ],
    }),
    hdrs = [
        "DBGConsole.h",
        "DebugConsoleImpl.h",
        "DebugLog.h",
        "Dump.h",
        "WebDebug.h",
        "WebDebugTemplate.h",
    ],
    data = [
        ":static_content",
    ],
    deps = [
        "//:stdafx",
        "//Base",
        "//System",
        "@webby//:webby",
    ],
)

cc_library(
    name = "Synchroniser",
    srcs = ["Synchroniser.cpp"],
    hdrs = ["Synchroniser.h"],
    deps = [
        ":Debug",
        "//:stdafx",
        "//Core",
    ],
)

filegroup(
    name = "static_content",
    srcs = glob([
        "Web/**",
    ]),
)
