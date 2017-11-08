cc_library(
    name = "Interface",
    hdrs =glob(["*.h"]),
    deps = [
        "//:stdafx",
        "//Core",
        "//Debug:DebugConsole",
        "//System",
        "//Utility",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "InterfaceCpp",
    srcs =glob(["*.cpp"]),
    deps = [
        ":Interface",
        "//:stdafx",
        "//Input",
        "//RomFile",
        "//Utility",
    ],
    visibility = ["//visibility:public"],
)