licenses(["notice"])

config_setting(
    name = "darwin",
    values = {"cpu": "darwin"},
)

config_setting(
    name = "windows",
    values = {"cpu": "x64_windows"},
)

_DEFINES = [
    "_GLFW_USE_OPENGL=1",
    "_GLFW_VERSION_FULL=\"3.0.1\"",
    "_GLFW_USE_MENUBAR=1",
]

cc_library(
    name = "glfw",
    hdrs = glob(["include/GLFW/*.h"]) + glob(["src/*.h"]) + [
        #"deps/GL/glext.h",
    ],
    includes = ["include"],
    srcs = [
        "src/context.c",
        "src/egl_context.c",
        "src/init.c",
        "src/input.c",
        "src/monitor.c",
        "src/osmesa_context.c",
        "src/vulkan.c",
        "src/window.c",
    ] + select({
        ":darwin": [
            "src/cocoa_time.c",
            "src/posix_thread.c",
          ],
          ":windows": [
            "src/wgl_context.c",
            "src/win32_init.c",
            "src/win32_joystick.c",
            "src/win32_monitor.c",
            "src/win32_thread.c",
            "src/win32_time.c",
            "src/win32_window.c",
          ],
    }),
    defines = _DEFINES + select({
        ":darwin": [
            "_GLFW_COCOA=1",
            "_GLFW_NSGL=1",
        ],
        ":windows": [
            '_GLFW_WIN32=1',
        ],
    }),
    deps = select({
        ":darwin": [":osx_objc"],
        ":windows": [],
    }),
    visibility = ["//visibility:public"],
)

objc_library(
    name = "osx_objc",
    hdrs = glob(["include/GLFW/*.h"]) + glob(["src/*.h"]) + [
    ],
    includes = ["include"],
    defines = _DEFINES + [
        "_GLFW_COCOA=1",
        "_GLFW_NSGL=1",
    ],
    srcs = [
        "src/cocoa_init.m",
        "src/cocoa_joystick.m",
        "src/cocoa_monitor.m",
        "src/cocoa_window.m",
        "src/nsgl_context.m",
    ],
    copts = [
        "-fno-objc-arc",
    ],
    sdk_frameworks = [
        "Cocoa",
        "CoreFoundation",
        "CoreVideo",
        "IOKit",
        "OpenGL",
    ],
)
