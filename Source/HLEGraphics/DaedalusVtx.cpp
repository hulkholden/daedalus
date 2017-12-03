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

#include "Base/Daedalus.h"
#include "HLEGraphics/DaedalusVtx.h"

// Bits for clipping
// 543210
// +++---
// zyxzyx
#define X_NEG  0x01	//left
#define Y_NEG  0x02	//bottom
#define Z_NEG  0x04	//far
#define X_POS  0x08	//right
#define Y_POS  0x10	//top
#define Z_POS  0x20	//near

void DaedalusVtx4::Interpolate(const DaedalusVtx4& lhs, const DaedalusVtx4& rhs, float factor)
{
	ProjectedPos   = lhs.ProjectedPos + (rhs.ProjectedPos - lhs.ProjectedPos) * factor;
	TransformedPos = lhs.TransformedPos + (rhs.TransformedPos - lhs.TransformedPos) * factor;
	Colour         = lhs.Colour + (rhs.Colour - lhs.Colour) * factor;
	Texture        = lhs.Texture + (rhs.Texture - lhs.Texture) * factor;
	ClipFlags      = 0;
}

void DaedalusVtx4::InitClipFlags()
{
	u32 clip_flags = 0;
	if		(ProjectedPos.x < -ProjectedPos.w)	clip_flags |= X_POS;
	else if (ProjectedPos.x > ProjectedPos.w)	clip_flags |= X_NEG;

	if		(ProjectedPos.y < -ProjectedPos.w)	clip_flags |= Y_POS;
	else if (ProjectedPos.y > ProjectedPos.w)	clip_flags |= Y_NEG;

	if		(ProjectedPos.z < -ProjectedPos.w)	clip_flags |= Z_POS;
	else if (ProjectedPos.z > ProjectedPos.w)	clip_flags |= Z_NEG;
	ClipFlags = clip_flags;
}

void DaedalusVtx4::SetColour(const v3& col, f32 a)
{
	Colour.x = col.x;
	Colour.y = col.y;
	Colour.z = col.z;
	Colour.w = a;
}

void DaedalusVtx4::GenerateTexCoord(const v3& norm, bool linear, bool mario_hack)
{
	f32 nx = norm.x;
	f32 ny = norm.y;

	if (linear)
	{
		Texture.x = 0.5f * ( 1.0f + nx );
		Texture.y = 0.5f * ( 1.0f + ny );
	}
	else
	{
		//Cheap way to do Acos(x)/Pi (abs() fixes star in SM64, sort of) //Corn
		if (mario_hack)
		{
			nx = fabsf(nx);
			ny = fabsf(ny);
		}
		Texture.x =  0.5f - 0.25f * nx - 0.25f * nx * nx * nx;
		Texture.y =  0.5f - 0.25f * ny - 0.25f * ny * ny * ny;
	}
}
