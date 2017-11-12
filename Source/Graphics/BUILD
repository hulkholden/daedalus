cc_library(
    name = "Graphics",
    hdrs = glob(["*.h"]),
    visibility = ["//visibility:public"],
    deps = [
        "//:stdafx",
        "//SysGL",
        "//Utility",
    ],
)

cc_library(
    name = "GraphicsCpp",
    srcs = glob(["*.cpp"]),
    visibility = ["//visibility:public"],
    deps = [
        "//:stdafx",
        "//Graphics",
        "//Math",
        "//third_party/glfw",
        "//third_party/libpng",
    ],
)
