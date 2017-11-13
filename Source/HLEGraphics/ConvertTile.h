#ifndef HLEGRAPHICS_CONVERTTILE_H_
#define HLEGRAPHICS_CONVERTTILE_H_

struct NativePf8888;
struct TextureInfo;

bool ConvertTile(const TextureInfo & ti, NativePf8888 * texels, u32 pitch);

#endif // HLEGRAPHICS_CONVERTTILE_H_
