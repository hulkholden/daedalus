cc_library(
    name = "Platform",
    hdrs = [
        "BuildOptions.h",
    ] + select({
        "//:darwin": [
            "OSX/Platform.h",
        ],
        "//:windows": [
            "Windows/Platform.h",
        ],
    }),
    defines = select({
        "//:darwin": [
            "DAEDALUS_OSX",
            "DAEDALUS_ACCURATE_TMEM",
            "DAEDALUS_ENABLE_OS_HOOKS",
            "DAEDALUS_GL",
        ],
        "//:windows": [
            "DAEDALUS_W32",
            "DAEDALUS_ACCURATE_TMEM",
            "DAEDALUS_ENABLE_OS_HOOKS",
            "DAEDALUS_GL",
            # TODO(strmnnrmn): Fix dynarec.
            #"DAEDALUS_ENABLE_DYNAREC",
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
