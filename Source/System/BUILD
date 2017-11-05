cc_library(
    name = "System",
    hdrs = glob(["*.h"]),
    deps = [
        "//:stdafx",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "SystemCpp",
    srcs = glob(["*.cpp"]),
    deps = [
        "//:stdafx",
        ":System",
        "//Core",
        "//Debug",
        "//Graphics",
        "//HLEGraphics",
        "//Interface",
        "//Input",
        "//OSHLE",
        "//Plugins",
        "//SysGL",
        "//Utility",
    ],
    visibility = ["//visibility:public"],
)
