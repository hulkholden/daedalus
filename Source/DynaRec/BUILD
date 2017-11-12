cc_library(
    name = "DynaRec",
    hdrs = [
        "AssemblyBuffer.h",
        "AssemblyUtils.h",
        "BranchType.h",
        "CodeBufferManager.h",
        "CodeGenerator.h",
        "DynaRecProfile.h",
        "Fragment.h",
        "FragmentCache.h",
        "IndirectExitMap.h",
        "RegisterSpan.h",
        "StaticAnalysis.h",
        "Trace.h",
        "TraceRecorder.h",
    ],
    deps = [
        "//:stdafx",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "DynaRecCpp",
    srcs = [
        "BranchType.cpp",
        "DynaRecProfile.cpp",
        "Fragment.cpp",
        "FragmentCache.cpp",
        "IndirectExitMap.cpp",
        "StaticAnalysis.cpp",
        "TraceRecorder.cpp",
    ] + select({
        "//:darwin": [
            "OSX/CodebufferManager.cpp",
        ],
        "//:windows": [
        ],
    }),
    deps = [
        ":DynaRec",
        "//:stdafx",
        "//Debug",
        "//Debug:Synchroniser",
        "//Core",
        "//System",
        "//Utility",
    ],
    visibility = ["//visibility:public"],
)
