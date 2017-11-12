cc_library(
    name = "Main",
    srcs = select({
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
        "//:stdafx",
        "//Core",
        "//Debug",
        "//DynaRec",
        "//HLEAudio",
        "//Interface",
        "//Plugins",
        "//System:SystemInit",
        "//Test:BatchTest",
    ],
)
