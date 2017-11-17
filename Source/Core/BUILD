cc_library(
    name = "Core",
    hdrs = glob([
        "*.h",
        "*.inl",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "//Base",
        "//System",
        "//Ultra",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "CoreCpp",
    srcs = [
        "CPU.cpp",
        "Cheats.cpp",
        "DMA.cpp",
        "Disassemble.cpp",
        "Dynamo.cpp",
        "FlashMem.cpp",
        "FramerateLimiter.cpp",
        "Interpret.cpp",
        "Interrupt.cpp",
        "JpegTask.cpp",
        "Memory.cpp",
        "PIF.cpp",
        "PrintOpCode.cpp",
        "R4300.cpp",
        "ROM.cpp",
        "ROMBuffer.cpp",
        "ROMImage.cpp",
        "RSP_HLE.cpp",
        "Registers.cpp",
        "RomSettings.cpp",
        "Save.cpp",
        "SaveState.cpp",
        "TLB.cpp",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//Base",
        "//Config",
        "//Core",
        "//Debug",
        "//DynaRec",
        "//Input",
        "//Interface",
        "//RomFile",
        "//System:SystemInit",
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
        "//HLEAudio:HLEAudioCpp",
        "//HLEGraphics:HLEGraphicsCpp",
        "//Interface:InterfaceCpp",
        "//Test:TestUtil",
        "@com_google_googletest//:gtest_main",
    ],
)
