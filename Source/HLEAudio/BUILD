cc_library(
    name = "HLEAudio",
    hdrs =glob(["*.h"]),
    deps = [
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "HLEAudioCpp",
    srcs = [
        "ABI1.cpp",
        "ABI2.cpp",
        "ABI3.cpp",
        "ABI3mp3.cpp",
        "AudioBuffer.cpp",
        "AudioHLEProcessor.cpp",
        "HLEMain.cpp",
    ] + select({
        "//:darwin": [
            "OSX/AudioPlugin.cpp",
        ],
        "//:windows": [
            "Windows/AudioPlugin.cpp",
        ],
    }),
    deps = [
        "//:stdafx",
        ":HLEAudio",
        "//Config",
        "//Core",
        "//Debug",
        "//Plugins",
        "//System",
        "//Utility",
    ],
    visibility = ["//visibility:public"],
)
