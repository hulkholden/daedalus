cc_library(
    name = "RomFile",
    srcs = [
        "RomFile.cpp",
        "RomFileCache.cpp",
        "RomFileCompressed.cpp",
        "RomFileUncompressed.cpp",
    ],
    hdrs = [
        "RomFile.h",
        "RomFileCache.h",
        "RomFileCompressed.h",
        "RomFileUncompressed.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//:stdafx",
        "//Debug",
        "//System",
        "//Utility",
        "@com_google_absl//absl/strings",
        "@zlib//:minizip",
        "@zlib//:zlib",
    ],
)

cc_test(
    name = "RomFile_test",
    size = "small",
    srcs = [
        "RomFile_test.cpp",
    ],
    visibility = ["//visibility:private"],
    deps = [
        ":RomFile",
        "//Test:TestUtil",
        "@com_google_googletest//:gtest_main",
    ],
)
