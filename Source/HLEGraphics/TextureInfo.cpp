/*
Copyright (C) 2001,2007 StrmnNrmn

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
#include "TextureInfo.h"

#include "Config/ConfigOptions.h"
#include "Core/Memory.h"
#include "Core/ROM.h"
#include "Ultra/ultra_gbi.h"
#include "Utility/Hash.h"
#include "Utility/Profiler.h"

static const char * const	gImageFormatNames[8] = {"RGBA", "YUV", "CI", "IA", "I", "?1", "?2", "?3"};
static const u32			gImageSizesInBits[4] = {4, 8, 16, 32};

const char * TextureInfo::GetFormatName() const
{
	return gImageFormatNames[ Format ];
}

u32 TextureInfo::GetSizeInBits() const
{
	return gImageSizesInBits[ Size ];
}


