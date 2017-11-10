package(default_visibility=["//visibility:public"])

cc_library(
    name = "Debug",
    hdrs = [
        "DBGConsole.h",
        "DebugConsoleImpl.h",
        "DebugLog.h",
        "Dump.h",
    ],
    srcs = select({
        "//:darwin": [
            "Posix/Assert.cpp",
        ],
        "//:windows": [
            "Windows/Assert.cpp",
        ],
    }),
    deps = [
        "//:stdafx",
        "//Base",
        "//Utility",
    ],
)

cc_library(
    name = "DebugCpp",
    srcs = [
        "DebugConsoleImpl.cpp",
        "DebugLog.cpp",
        "Dump.cpp",
    ],
    deps = [
        "//:stdafx",
        "//Debug",
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
