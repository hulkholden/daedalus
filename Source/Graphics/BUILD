cc_library(
    name = "Graphics",
    hdrs =glob(["*.h"]),
    deps = [
        "//:stdafx",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "GraphicsCpp",
    srcs = glob(["*.cpp"]),
    deps = [
        "//:stdafx",
        "//third_party/libpng",
        "//Graphics",
        "//Math",
    ],
    visibility = ["//visibility:public"],
)
