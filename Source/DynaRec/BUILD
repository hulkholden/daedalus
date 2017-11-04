cc_library(
    name = "DynaRec",
    hdrs =glob(["*.h"]),
    deps = [
        "//:stdafx",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "DynaRecCpp",
    srcs =glob(["*.cpp"]),    
    deps = [
        "//:stdafx",
        "//Debug",
        ":DynaRec",
        "//Core",
        "//System",
        "//Utility",
    ],
    visibility = ["//visibility:public"],
)
