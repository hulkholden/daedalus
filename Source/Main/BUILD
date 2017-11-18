cc_library(
    name = "Main",
    srcs = [
        "Main.cpp",
    ] + select({
        "//:darwin": [
            "OSX/main.cpp",
        ],
        "//:windows": [
            "Windows/main.cpp",
        ],
    }),
    linkopts = select({
        "//:darwin": [
            "-framework AudioToolbox",
        ],
        "//conditions:default": [],
    }),
    linkstatic = 1,
    visibility = ["//visibility:public"],
    deps = [
        ":SystemInit",
        "//Base",
        "//Core",
        "//Debug",
        "//DynaRec",
        "//HLEAudio",
        "//Interface",
        "//Test:BatchTest",
        "//external:gflags",
    ],
)

cc_library(
    name = "SystemInit",
    srcs = ["SystemInit.cpp"],
    hdrs = ["SystemInit.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//Base",
        "//Core",
        "//Debug",
        "//Debug:Synchroniser",
        "//Graphics",
        "//HLEAudio",
        "//HLEGraphics",
        "//Input",
        "//Interface",
        "//OSHLE",
        "//Utility",
    ],
)
