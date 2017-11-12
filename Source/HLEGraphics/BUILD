cc_library(
    name = "HLEGraphics",
    hdrs = glob([
        "*.h",
        "uCodes/*.h",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "//:stdafx",
        "//Interface",
        "//Math",
        "//Utility",
        "@glfw//:glfw",
    ],
)

cc_library(
    name = "HLEGraphicsCpp",
    srcs = glob([
        "*.cpp",
        "uCodes/*.cpp",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        ":HLEGraphics",
        "//:stdafx",
        "//Config",
        "//Core",
        "//Debug",
        "//Graphics",
        "//Plugins",
        "//Test:BatchTest",
        "//Utility",
    ],
)
