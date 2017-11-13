/*
Copyright (C) 2001 StrmnNrmn

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

#ifndef GRAPHICS_NATIVEPIXELFORMAT_H_
#define GRAPHICS_NATIVEPIXELFORMAT_H_

#include "Base/Macros.h"

struct NativePf8888
{
	union
	{
		struct
		{
			u8		R;
			u8		G;
			u8		B;
			u8		A;
		};
		u32			Bits;
	};

	static u32 Make( u8 r, u8 g, u8 b, u8 a )
	{
		return (r << ShiftR) |
			   (g << ShiftG) |
			   (b << ShiftB) |
			   (a << ShiftA);
	}

	template< typename T >
	static NativePf8888 Make( T c )
	{
		return NativePf8888( c.GetR(), c.GetG(), c.GetB(), c.GetA() );
	}

	NativePf8888()
	{
	}

	// Would like to remove this
	explicit NativePf8888( u32 bits )
		:	Bits( bits )
	{
	}

	NativePf8888( u8 r, u8 g, u8 b, u8 a )
		:	Bits( Make( r,g,b,a ) )
	{
	}

	u8	GetR() const { return R; }
	u8	GetG() const { return G; }
	u8	GetB() const { return B; }
	u8	GetA() const { return A; }

	static const u32	MaskR = 0x000000ff;
	static const u32	MaskG = 0x0000ff00;
	static const u32	MaskB = 0x00ff0000;
	static const u32	MaskA = 0xff000000;

	static const u32	ShiftR = 0;
	static const u32	ShiftG = 8;
	static const u32	ShiftB = 16;
	static const u32	ShiftA = 24;

	static const u32	BitsR = 8;
	static const u32	BitsG = 8;
	static const u32	BitsB = 8;
	static const u32	BitsA = 8;
};
DAEDALUS_STATIC_ASSERT( sizeof( NativePf8888 ) == 4 );

#endif // GRAPHICS_NATIVEPIXELFORMAT_H_
