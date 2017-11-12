cc_library(
    name = "Core",
    hdrs = glob([
        "*.h",
        "*.inl",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "//:stdafx",
        "//OSHLE",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "CoreCpp",
    srcs = glob(["*.cpp"]),
    visibility = ["//visibility:public"],
    deps = [
        "//:stdafx",
        "//Config",
        "//Core",
        "//Debug",
        "//DynaRec",
        "//Input",
        "//Interface",
        "//OSHLE",
        "//Plugins",
        "//RomFile",
        "//System:SystemInit",
        "//Test:BatchTest",
        "//Utility",
    ],
)
