cc_library(
    name = "SysOSX",
    hdrs = glob(["**/*.h"]),
    includes = ["Include", "."],
    visibility = ["//visibility:public"],
    deps = [
        "//third_party/webby",
    ],
)

cc_library(
    name = "Main",
    srcs = glob(["**/*.cpp"]),
    visibility = ["//visibility:public"],
    linkstatic = 1,
    deps = [
        "//:stdafx",
        ":SysOSX",
        "//Core",
        "//Debug",
        "//DynaRec",
        "//HLEAudio",
        "//Interface",
        "//Plugins",
        "//System:SystemInit",
        "//Test:BatchTest",
    ],
    data = [
        ":static_content",
    ],
    linkopts = [
      "-framework AudioToolbox",
    ],
)

filegroup(
    name = "static_content",
    srcs = glob([
        "Debug/Web/**",
    ]),
)
