cc_library(
    name = "Base",
    srcs = glob(["*.cpp"]),
    hdrs = glob(["*.h"] + [
        "Dev/BuildConfig.h",
        "Profile/BuildConfig.h",
        "Release/BuildConfig.h",
    ]),
    defines = select({
        "//:release": ["DAEDALUS_CONFIG_RELEASE"],
        "//:debug": ["DAEDALUS_CONFIG_DEV"],
    }),
    visibility = ["//visibility:public"],
    deps = [
        "//Platform",
    ],
)
