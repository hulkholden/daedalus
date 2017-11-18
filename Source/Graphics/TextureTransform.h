#ifndef GRAPHICS_TEXTURETRANSFORM_H_
#define GRAPHICS_TEXTURETRANSFORM_H_

#include "Graphics/NativePixelFormat.h"

void ClampTexels( NativePf8888 * texels, u32 n64_width, u32 n64_height, u32 native_width, u32 native_height, u32 native_stride );

#endif // GRAPHICS_TEXTURETRANSFORM_H_
