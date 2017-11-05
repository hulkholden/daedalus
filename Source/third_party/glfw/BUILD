licenses(["notice"])

config_setting(
    name = "darwin",
    values = {"cpu": "darwin"},
)

config_setting(
    name = "darwin_x86_64",
    values = {"cpu": "darwin_x86_64"},
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
        "deps/GL/glext.h",
    ],
    includes = ["include"],
    srcs = [
        "src/clipboard.c",
        "src/context.c",
        "src/gamma.c",
        "src/init.c",
        "src/input.c",
        "src/joystick.c",
        "src/monitor.c",
        "src/time.c",
        "src/window.c",
    ] + select({
        ":darwin": [
            "src/cocoa_gamma.c",
            "src/cocoa_time.c",
          ],
    }),
    defines = _DEFINES + select({
        ":darwin": [
            "_GLFW_COCOA=1",
            "_GLFW_NSGL=1",
        ]
    }),
    deps = select({
        ":darwin": [":osx_objc"],
    }),
    visibility = ["//visibility:public"],
)


# build with --apple_platform_type=macos

objc_library(
    name = "osx_objc",
    hdrs = glob(["include/GLFW/*.h"]) + glob(["src/*.h"]) + [
        "deps/GL/glext.h",
    ],
    includes = ["include"],
    defines = _DEFINES + [
        "_GLFW_COCOA=1",
        "_GLFW_NSGL=1",
    ],
    srcs = [
        "src/nsgl_context.m",
        "src/cocoa_clipboard.m",
        "src/cocoa_init.m",
        "src/cocoa_joystick.m",
        "src/cocoa_monitor.m",
        "src/cocoa_window.m",
    ],
    sdk_frameworks = [
        "OpenGL",
    ],
)
