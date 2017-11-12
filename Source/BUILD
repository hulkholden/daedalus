package(default_visibility = ["//visibility:public"])

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

config_setting(
    name = "release",
    values = {"compilation_mode": "opt"},
)

config_setting(
    name = "debug",
    values = {"compilation_mode": "fastbuild"},
)

cc_library(
    name = "stdafx",
    srcs = ["StdAfx.cpp"],
    hdrs = [
        "stdafx.h",
    ],
    defines = select({
        ":release": ["DAEDALUS_CONFIG_RELEASE"],
        ":debug": ["DAEDALUS_CONFIG_DEV"],
    }),
    deps = [
        "//Base",
        "//Platform",
    ],
)

cc_binary(
    name = "daedalus",
    deps = [
        "//Base",
        "//Core:CoreCpp",
        "//Debug",
        "//DynaRec:DynaRecCpp",
        "//Graphics:GraphicsCpp",
        "//HLEAudio:HLEAudioCpp",
        "//HLEGraphics:HLEGraphicsCpp",
        "//Interface:InterfaceCpp",
        "//Main",
        "//OSHLE:OSHLECpp",
        "//SysGL:SysGLCpp",
        "//System",
        "//System:SystemInit",
        "//Utility",
        "//third_party/zlib",
    ],
)
