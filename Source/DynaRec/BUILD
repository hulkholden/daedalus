cc_library(
    name = "DynaRec",
    hdrs = glob(["*.h"]),
    deps = [
        "//:stdafx",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "DynaRecCpp",
    srcs = glob(["*.cpp"]),
    deps = [
        ":DynaRec",
        "//:stdafx",
        "//Debug",
        "//Debug:Synchroniser",
        "//Core",
        "//System",
        "//Utility",
    ],
    visibility = ["//visibility:public"],
)
