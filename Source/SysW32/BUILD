cc_library(
    name = "SysW32",
    hdrs = glob(["**/*.h"]),
    includes = ["Include"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "Main",
    srcs = glob(["**/*.cpp"]),
    visibility = ["//visibility:public"],
    linkstatic = 1,
    deps = [
        "//:stdafx",
        ":SysW32",
        "//Config",
        "//Core",
        "//DynaRec",
        "//Debug",
        "//HLEAudio",
        "//HLEGraphics",
        "//Interface",
        "//System",
        "//Test:BatchTest",
        "//Utility",
    ],
)
