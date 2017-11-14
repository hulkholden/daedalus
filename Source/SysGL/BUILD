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
        ":SysGL",
        "//Base",
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
