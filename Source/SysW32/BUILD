cc_library(
    name = "SysW32",
    hdrs = [
        "Include/Platform.h",
    ],
    includes = ["Include"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "Main",
    srcs = glob(["**/*.cpp"]),
    visibility = ["//visibility:public"],
    linkstatic = 1,
    deps = [
        "//:stdafx",
        ":SysW32",
    ],
)
