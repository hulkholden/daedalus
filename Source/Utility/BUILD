cc_library(
    name = "Utility",
    hdrs = glob(["*.h"]),
    deps = [
        "//:stdafx",
        "//third_party/zlib",
        "//third_party/zlib/contrib/minizip",
        "//Debug:DebugConsole",
        "//Math",
        "//System",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "UtilityCpp",
    srcs = glob(["*.cpp"], exclude=["FastMemcpy_test.cpp"]),
    deps = [
        ":Utility",
        "//third_party/zlib/contrib/minizip",        
        "//third_party/gtest",        
        "//:stdafx",
        "//Config",
        "//Core",
        "//Input",
        "//Interface",
        "//Math",
        "//OSHLE",
    ],
    visibility = ["//visibility:public"],
)
