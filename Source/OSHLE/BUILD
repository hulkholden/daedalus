cc_library(
    name = "OSHLE",
    hdrs = glob([
        "*.h",
        "*.inl",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "//Base",
        "//Utility",
    ],
)

cc_library(
    name = "OSHLECpp",
    srcs = glob(["*.cpp"]),
    visibility = ["//visibility:public"],
    deps = [
        "//Base",
        "//Config",
        "//Core",
        "//Debug",
        "//DynaRec",
        "//Math",
        "//OSHLE",
        "//Plugins",
        "//Utility",
    ],
)
