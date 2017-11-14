cc_library(
    name = "HLEGraphics",
    hdrs = glob([
        "*.h",
        "uCodes/*.h",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "//Base",
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
    data = [
        ":shaders",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":HLEGraphics",
        "//Base",
        "//Config",
        "//Core",
        "//Debug",
        "//Graphics",
        "//Plugins",
        "//Test:BatchTest",
        "//Utility",
    ],
)

filegroup(
    name = "shaders",
    srcs = [
        "n64.psh",
    ],
)
