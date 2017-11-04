cc_library(
    name = "OSHLE",
    hdrs =glob(["*.h"]),
    deps = [
        "//:stdafx",
        "//Utility",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "CoreDeps",
    srcs =glob(["*.cpp"]),    
    deps = [
        "//:stdafx",
        "//Core",
        "//OSHLE",
        "//Utility",
    ],
    visibility = ["//visibility:public"],
)
