cc_library(
    name = "SysGL",
    hdrs = glob(["**/*.h"]),
    visibility = ["//visibility:public"],
    deps = [
        "//third_party/glew",
    ],
)

cc_library(
    name = "SysGLCpp",
    srcs = glob(["**/*.cpp"]),
    data = [
        ":shaders",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":SysGL",
        "//:stdafx",
        "//Core",
        "//Debug",
        "//Graphics",
        "//HLEGraphics",
        "//Input",
        "//Interface",
        "//Plugins",
        "@libpng//:libpng",
    ],
)

filegroup(
    name = "shaders",
    srcs = [
        "HLEGraphics/n64.psh",
    ],
)
