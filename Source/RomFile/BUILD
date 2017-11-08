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
        "//third_party/zlib",
        "//third_party/zlib/contrib/minizip",
    ],
    visibility = ["//visibility:public"],
)
