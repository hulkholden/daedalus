cc_library(
    name = "HLEGraphics",
    hdrs =glob(["*.h", "uCodes/*.h"]),
    deps = [
        "//:stdafx",
        "//Interface",
        "//Math",
        "//third_party/glfw",
        "//Utility",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "HLEGraphicsCpp",
    srcs = glob(["*.cpp", "uCodes/*.cpp"]),
    deps = [
        "//:stdafx",
        ":HLEGraphics",
        "//Config",
        "//Core",
        "//Debug",
        "//Graphics",
        "//Plugins",
        "//Test",
        "//Utility",
    ],
    visibility = ["//visibility:public"],
)
