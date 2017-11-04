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
        "//Interface",
        "//Input",
        "//OSHLE",
        "//Plugins",
        "//Utility",
    ],
    visibility = ["//visibility:public"],
)
