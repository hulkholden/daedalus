cc_library(
    name = "Platform",
    hdrs = [
        "BuildOptions.h",
    ],
    defines = select({
        "//:darwin": [
            "DAEDALUS_OSX",
            "DAEDALUS_ACCURATE_TMEM",
            "DAEDALUS_ENABLE_OS_HOOKS",
            "DAEDALUS_GL",
            "DAEDALUS_ENDIAN_MODE=DAEDALUS_ENDIAN_LITTLE",
        ],
        "//:windows": [
            "DAEDALUS_W32",
            "DAEDALUS_ACCURATE_TMEM",
            "DAEDALUS_ENABLE_OS_HOOKS",
            "DAEDALUS_GL",
            # TODO(strmnnrmn): Fix dynarec.
            #"DAEDALUS_ENABLE_DYNAREC",
            "DAEDALUS_ENDIAN_MODE=DAEDALUS_ENDIAN_LITTLE",
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
