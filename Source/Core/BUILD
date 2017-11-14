cc_library(
    name = "Core",
    hdrs = glob([
        "*.h",
        "*.inl",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "//Base",
        "//System",
        "//Ultra",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "CoreCpp",
    srcs = glob(["*.cpp"]),
    visibility = ["//visibility:public"],
    deps = [
        "//Base",
        "//Config",
        "//Core",
        "//Debug",
        "//DynaRec",
        "//HLEGraphics",
        "//Input",
        "//Interface",
        "//Plugins",
        "//RomFile",
        "//System:SystemInit",
        "//Test:BatchTest",
        "//Utility",
    ],
)
