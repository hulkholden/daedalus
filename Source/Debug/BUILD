
cc_library(
    name = "Debug",
    hdrs = glob(["*.h"]),
    deps = [
        "//:stdafx",
        "//Utility",
        "//third_party/zlib/contrib/minizip",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "DebugCpp",
    srcs = glob(["*.cpp"]),
    deps = [
        "//:stdafx",
        "//Debug",
        "//Core",
        "//Config",
        "//OSHLE",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "DebugConsole",
    hdrs = [
        "DBGConsole.h",
    ],
    deps = [
        "//:stdafx",
    ],
    visibility = ["//visibility:public"],
)
