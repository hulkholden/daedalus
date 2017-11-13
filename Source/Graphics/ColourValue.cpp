/*
Copyright (C) 2006 StrmnNrmn

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
#include "ColourValue.h"

#include "Base/MathUtil.h"
#include "Math/Vector4.h"

inline u32 Vector2ColourClamped( const v4 & colour )
{
	u8 r = u8( Clamp<s32>( s32(colour.x * 255.0f), 0, 255 ) );
	u8 g = u8( Clamp<s32>( s32(colour.y * 255.0f), 0, 255 ) );
	u8 b = u8( Clamp<s32>( s32(colour.z * 255.0f), 0, 255 ) );
	u8 a = u8( Clamp<s32>( s32(colour.w * 255.0f), 0, 255 ) );

	return c32::Make( r, g, b, a );
}

c32::c32( const v4 & colour )
:	mColour( Vector2ColourClamped( colour ) )
{
}

v4	c32::GetColourV4() const
{
	return v4( GetR() / 255.0f, GetG() / 255.0f, GetB() / 255.0f, GetA() / 255.0f );
}

