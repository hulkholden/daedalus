cc_library(
    name = "Graphics",
    hdrs = glob(["*.h"]),
    visibility = ["//visibility:public"],
    deps = [
        "//Base",
        "//SysGL",
        "//Utility",
    ],
)

cc_library(
    name = "GraphicsCpp",
    srcs = glob(["*.cpp"]),
    visibility = ["//visibility:public"],
    deps = [
        "//Base",
        "//Graphics",
        "//Math",
        "@glfw//:glfw",
        "@libpng//:libpng",
    ],
)
