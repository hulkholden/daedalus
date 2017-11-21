cc_library(
    name = "HLEGraphics",
    srcs = glob([
        "*.cpp",
        "uCodes/*.cpp",
    ]),
    hdrs = glob([
        "*.h",
        "uCodes/*.h",
    ]),
    data = [
        ":shaders",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//Base",
        "//Config",
        "//Core",
        "//Debug",
        "//Graphics",
        "//Interface",
        "//Math",
        "//Utility",
        "@glfw//:glfw",
    ],
)

filegroup(
    name = "shaders",
    srcs = [
        "n64.psh",
    ],
)
