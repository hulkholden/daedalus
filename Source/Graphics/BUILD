cc_library(
    name = "Graphics",
    hdrs =glob(["*.h"]),
    deps = [
        "//:stdafx",
        "//Utility",
        "//SysGL",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "GraphicsCpp",
    srcs = glob(["*.cpp"]),
    deps = [
        "//:stdafx",
        "//third_party/libpng",
        "//third_party/glfw",
        "//Graphics",
        "//Math",
    ],
    visibility = ["//visibility:public"],
)
