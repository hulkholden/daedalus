cc_library(
    name = "HLEGraphics",
    hdrs =glob(["*.h"]),
    deps = [
        "//:stdafx",
        "//Utility",
        "//third_party/glfw",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "HLEGraphicsCpp",
    srcs = glob(["*.cpp"]),
    deps = [
        "//:stdafx",
        ":HLEGraphics",
        "//Config",
        "//Core",
        "//Graphics",
    ],
    visibility = ["//visibility:public"],
)
