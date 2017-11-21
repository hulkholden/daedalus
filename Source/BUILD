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

cc_binary(
    name = "daedalus",
    deps = [
        "//Base",
        "//Core:CoreCpp",
        "//Debug",
        "//DynaRec:DynaRecCpp",
        "//Graphics",
        "//HLEAudio",
        "//HLEGraphics",
        "//Interface:InterfaceCpp",
        "//Main",
        "//Main:SystemInit",
        "//OSHLE",
        "//System",
        "//Utility",
        "@zlib//:zlib",
    ],
)
