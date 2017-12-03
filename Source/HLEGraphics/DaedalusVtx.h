/*
Copyright (C) 2001,2006 StrmnNrmn

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#ifndef HLEGRAPHICS_DAEDALUSVTX_H_
#define HLEGRAPHICS_DAEDALUSVTX_H_

#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

struct DaedalusVtx4
{
	v4 TransformedPos;
	v4 ProjectedPos;
	v4 Colour;
	v2 Texture;

	u32 ClipFlags;

	void Interpolate(const DaedalusVtx4& lhs, const DaedalusVtx4& rhs, float factor);

	void InitClipFlags();
	void SetColour(const v3& col, f32 a);

	void GenerateTexCoord(const v3& norm, bool linear, bool mario_hack);
};

struct TexCoord
{
	s16 s;
	s16 t;

	TexCoord()
	{
	}
	TexCoord(s16 s_, s16 t_) : s(s_), t(t_)
	{
	}
};

#endif  // HLEGRAPHICS_DAEDALUSVTX_H_
