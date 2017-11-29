cc_library(
    name = "HLEAudio",
    srcs = [
        "ABI1.cpp",
        "ABI2.cpp",
        "ABI3.cpp",
        "ABI3mp3.cpp",
        "AudioBuffer.cpp",
        "HLEAudioState.cpp",
        "HLEMain.cpp",
    ] + select({
        "//:darwin": [
            "OSX/HLEAudio.cpp",
        ],
        "//:windows": [
            "Windows/HLEAudio.cpp",
        ],
    }),
    hdrs = glob(["*.h"]),
    visibility = ["//visibility:public"],
    deps = [
        "//Base",
        "//Config",
        "//Core",
        "//Debug",
        "//System",
        "//Utility",
    ],
)
