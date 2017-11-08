package(default_visibility=["//visibility:public"])

config_setting(
    name = "darwin",
    values = {"cpu": "darwin"},
    visibility = [":__subpackages__"],
)

config_setting(
    name = "windows",
    values = {"cpu": "x64_windows"},
    visibility = [":__subpackages__"],
)

cc_library(
    name = "stdafx",
    srcs = ["StdAfx.cpp"],
    hdrs = [
        "stdafx.h",
        "BuildOptions.h",
    ],
    deps = select({
        ":darwin": [
           "//SysOSX",
        ],
        ":windows": [
            "//SysW32",
        ],
    }) + [
        "//Base",
    ],
)

cc_binary(
    name = "daedalus",
    deps = select({
        ":darwin": [
           "//SysOSX:Main",
           "//SysPosix:SysPosixCpp",
        ],
    }) + [
        "//Base",
        "//Core:CoreCpp",
        "//Debug:DebugCpp",
        "//Graphics:GraphicsCpp",
        "//HLEAudio:HLEAudioCpp",
        "//HLEGraphics:HLEGraphicsCpp",
        "//Interface:InterfaceCpp",
        "//OSHLE:OSHLECpp",
        "//SysGL:SysGLCpp",
        "//System",
        "//System:SystemInit",
        "//third_party/zlib",
        "//Utility:UtilityCpp",
    ],
)
