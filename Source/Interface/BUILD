cc_library(
    name = "Interface",
    hdrs = glob(["*.h"]),
    visibility = ["//visibility:public"],
    deps = [
        "//Base",
        "//Core",
        "//Debug",
        "//System",
        "//Utility",
    ],
)

cc_library(
    name = "InterfaceCpp",
    srcs = glob(["*.cpp"]),
    visibility = ["//visibility:public"],
    deps = [
        ":Interface",
        "//Base",
        "//Input",
        "//RomFile",
        "//Utility",
    ],
)
