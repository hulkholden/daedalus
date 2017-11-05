cc_library(
    name = "OSHLE",
    hdrs =glob(["*.h", "*.inl"]),
    deps = [
        "//:stdafx",
        "//Utility",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "OSHLECpp",
    srcs = glob(["*.cpp"]),
    deps = [
        "//:stdafx",
        "//Config",
        "//Core",
        "//Debug",
        "//DynaRec",
        "//OSHLE",
        "//Plugins",
        "//Utility",
    ],
    visibility = ["//visibility:public"],
)
