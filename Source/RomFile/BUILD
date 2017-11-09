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
    deps = [
        "//:stdafx",
        "//Debug",
        "//System",
        "//Utility",
        "//third_party/zlib",
        "//third_party/zlib/contrib/minizip",
        "@com_google_absl//absl/strings",
    ],
    visibility = ["//visibility:public"],
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
