cc_library(
    name = "Platform",
    hdrs = select({
        "//:darwin": [
            "OSX/Platform.h"
        ],
        "//:windows": [
            "Windows/Platform.h",
        ],
    }),
    includes = select({
        "//:darwin": [
            "OSX",
        ],
        "//:windows": [
            "Windows",
        ],
    }),
    visibility = ["//visibility:public"],
)
