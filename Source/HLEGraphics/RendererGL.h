#ifndef HLEGRAPHICS_RENDERERGL_H_
#define HLEGRAPHICS_RENDERERGL_H_

#include "HLEGraphics/BaseRenderer.h"

struct BufferData;

class RendererGL : public BaseRenderer
{
  public:
	virtual void RestoreRenderStates();

	virtual void RenderTriangles(DaedalusVtx* p_vertices, u32 num_vertices, bool disable_zbuffer);

	virtual void TexRect(u32 tile_idx, const v2& xy0, const v2& xy1, TexCoord st0, TexCoord st1);
	virtual void TexRectFlip(u32 tile_idx, const v2& xy0, const v2& xy1, TexCoord st0, TexCoord st1);
	virtual void FillRect(const v2& xy0, const v2& xy1, u32 color);

	virtual void Draw2DTexture(f32 x0, f32 y0, f32 x1, f32 y1, f32 u0, f32 v0, f32 u1, f32 v1,
	                           const CNativeTexture* texture);
	virtual void Draw2DTextureR(f32 x0, f32 y0, f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3, f32 s, f32 t);

  private:
	void MakeShaderConfigFromCurrentState(struct ShaderConfiguration* config) const;

	int PrepareRenderState(const Matrix4x4& mat_project, bool disable_zbuffer);

	void RenderDaedalusVtxStreams(int prim, int buffer_idx, const float* positions, const TexCoord* uvs,
	                              const u32* colours, int count);
};

bool Renderer_Initialise();
void Renderer_Finalise();

#endif  // HLEGRAPHICS_RENDERERGL_H_
