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
            "Windows/x86/CodeGeneratorX86.h",
            "Windows/x86/DynarecTargetX86.h",
        ],
    }),
    visibility = ["//visibility:public"],
    deps = [
        "//Base",
    ],
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
            "Windows/x86/DISASM86.cpp",
            "Windows/x86/AssemblyUtilsX86.cpp",
            "Windows/x86/AssemblyWriterX86.cpp",
            "Windows/x86/CodeBufferManagerX86.cpp",
            "Windows/x86/CodeGeneratorX86.cpp",
        ],
    }),
    visibility = ["//visibility:public"],
    deps = [
        ":DynaRec",
        "//Base",
        "//Core",
        "//Debug",
        "//Debug:Synchroniser",
        "//System",
        "//Utility",
    ],
)
