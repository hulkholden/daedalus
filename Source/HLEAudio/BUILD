cc_library(
    name = "HLEAudio",
    hdrs =glob(["*.h"]),
    deps = [
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "HLEAudioCpp",
    srcs = glob(["*.cpp"]),
    deps = [
        "//:stdafx",
        ":HLEAudio",
        "//Core",
        "//Debug:DebugConsole",
    ],
    visibility = ["//visibility:public"],
)
