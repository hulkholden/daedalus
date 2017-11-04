cc_library(
    name = "Core",
    hdrs =glob(["*.h", "*.inl"]),
    deps = [
        "//:stdafx",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "CoreCpp",
    srcs =glob(["*.cpp"]),
    deps = [
        "//:stdafx",
        "//Core",
        "//Config",
        "//Debug",
        "//DynaRec",
        "//Interface",
        "//Input",
        "//OSHLE",
        "//Plugins",
        "//System",
        "//Test",
        "//Utility",
    ],
    visibility = ["//visibility:public"],
)