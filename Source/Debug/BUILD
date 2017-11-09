
cc_library(
    name = "Debug",
    hdrs = [
        "DBGConsole.h",
        "DebugConsoleImpl.h",
        "DebugLog.h",
        "Dump.h",
        "Synchroniser.h",
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
    visibility = ["//visibility:public"],
)

cc_library(
    name = "DebugCpp",
    srcs = glob(["*.cpp"]),
    deps = [
        "//:stdafx",
        "//Debug",
        "//Core",
        "//Config",
        "//OSHLE",
    ],
    visibility = ["//visibility:public"],
)

