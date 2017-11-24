cc_library(
    name = "Core",
    srcs = [
        "Cheats.cpp",
        "Disassemble.cpp",
        "DMA.cpp",
        "FlashMem.cpp",
        "FramerateLimiter.cpp",
        "Interrupt.cpp",
        "JpegTask.cpp",
        "R4300.cpp",
        "ROMBuffer.cpp",
        "ROMImage.cpp",
        "Registers.cpp",
        "TLB.cpp",
    ],
    hdrs = glob([
        "*.h",
        "*.inl",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "//Base",
        "//Config",
        "//Debug",
        "//RomFile",
        "//System",
        "//DynaRec",
        "//Ultra",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "CoreCpp",
    srcs = [
        "CPU.cpp",
        "Dynamo.cpp",
        "Interpret.cpp",
        "Memory.cpp",
        "PrintOpCode.cpp",
        "PIF.cpp",
        "ROM.cpp",
        "RSP_HLE.cpp",
        "RomSettings.cpp",
        "Save.cpp",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//Base",
        "//Config",
        "//Core",
        "//Debug:Synchroniser",
        "//Input",
        "//Interface",
        "//OSHLE",
        "//Utility",
    ],
)

cc_test(
    name = "R4300_test",
    size = "small",
    srcs = [
        "R4300_test.cpp",
    ],
    visibility = ["//visibility:private"],
    deps = [
        ":CoreCpp",
        # TODO(strmnnrn): Fix this mess of dependencies.
        "//HLEAudio",
        "//Graphics:GL",
        "//Test:TestUtil",
        "@com_google_googletest//:gtest_main",
    ],
)
