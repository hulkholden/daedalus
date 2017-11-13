cc_library(
    name = "Platform",
    hdrs = select({
        "//:windows": [
            "Windows/DaedalusW32.h",
        ],
        "//conditions:default": [],
    }),
    defines = select({
        "//:darwin": [
            "DAEDALUS_OSX",
            "DAEDALUS_ENABLE_OS_HOOKS",
            "DAEDALUS_ENDIAN_MODE=DAEDALUS_ENDIAN_LITTLE",
        ],
        "//:windows": [
            "DAEDALUS_W32",
            "DAEDALUS_ENABLE_OS_HOOKS",
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
