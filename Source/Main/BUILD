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
    visibility = ["//visibility:public"],
    linkstatic = 1,
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
    linkopts = [
      "-framework AudioToolbox",
    ],
)
