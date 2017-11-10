package(default_visibility=["//visibility:public"])

cc_library(
    name = "Debug",
    srcs = [
        "DebugConsoleImpl.cpp",
        "DebugLog.cpp",
        "Dump.cpp",
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
    ],
    deps = [
        "//:stdafx",
        "//Base",
        "//Utility",
    ],
)

cc_library(
    name = "Synchroniser",
    srcs = ["Synchroniser.cpp"],
    hdrs = ["Synchroniser.h"],
    deps = [
        ":Debug",
        "//:stdafx",
        "//Core:Core",
    ],
)
