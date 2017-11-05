cc_library(
    name = "SysPosix",
    hdrs = glob(["**/*.h"]),
    visibility = ["//visibility:public"],
)

cc_library(
    name = "SysPosixCpp",
    srcs = glob(["**/*.cpp"]),
    visibility = ["//visibility:public"],
    linkstatic = 1,
    deps = [
        "//:stdafx",
        ":SysPosix",
        "//Utility",
    ],
)
