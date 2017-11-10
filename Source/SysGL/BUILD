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
    visibility = ["//visibility:public"],
    deps = [
        "//:stdafx",
        ":SysGL",
        "//Core",
        "//Debug",
        "//Graphics",
        "//HLEGraphics",
        "//Input",
        "//Interface",
        "//Plugins",
        "//third_party/libpng",
    ],
    data = [
        ":shaders",
    ],
)

filegroup(
    name = "shaders",
    srcs = [
        "HLEGraphics/n64.psh",
    ],
)
