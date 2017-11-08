cc_library(
    name = "Core",
    hdrs =glob(["*.h", "*.inl"]),
    deps = [
        "//:stdafx",
        "//OSHLE",
        "@com_google_absl//absl/strings",
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
        "//RomFile",
        "//System",
        "//Test",
        "//Utility",
    ],
    visibility = ["//visibility:public"],
)