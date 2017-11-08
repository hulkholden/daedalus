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
        "//Debug:DebugConsole",
        "//Graphics",
        "//HLEGraphics",
        "//Input",
        "//Interface",
        "//Plugins",
        "//third_party/libpng",
    ],
)