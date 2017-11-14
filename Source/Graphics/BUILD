cc_library(
    name = "Graphics",
    srcs = [
        "ColourValue.cpp",
        "GraphicsContext.cpp",
        "NativeTexture.cpp",
        "PngUtil.cpp",
        "TextureTransform.cpp",
    ],
    hdrs = [
        "ColourValue.h",
        "GraphicsContext.h",
        "NativePixelFormat.h",
        "NativeTexture.h",
        "PngUtil.h",
        "TextureTransform.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":GL",
        "//Base",
        "//Math",
        "//Utility",
        "@libpng//:libpng",
    ],
)

cc_library(
    name = "GL",
    srcs = ["GL.cpp"],
    hdrs = ["GL.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//Base",
        "//third_party/glew",
        "@glfw//:glfw",
    ],
)
