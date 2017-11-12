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
    ] + select({
        "//:darwin": [
        ],
        "//:windows": [
            "Windows/x86/AssemblyWriterX86.h",
            "Windows/x86/DynarecTargetX86.h",
        ],
    }),
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
            "Windows/DISASM86.cpp",
            "Windows/x86/AssemblyUtilsX86.cpp",
            "Windows/x86/AssemblyWriterX86.cpp",
            "Windows/x86/CodeBufferManagerX86.cpp",
            "Windows/x86/CodeGeneratorX86.cpp",
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
