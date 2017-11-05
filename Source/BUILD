package(default_visibility=["//visibility:public"])

config_setting(
    name = "darwin",
    values = {"cpu": "darwin"},
)

config_setting(
    name = "darwin_x86_64",
    values = {"cpu": "darwin_x86_64"},
)

config_setting(
    name = "windows",
    values = {"cpu": "x64_windows"},
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
        ]
    }) + [
        "//Base",
    ],
)
