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
#include "Core/ROMBuffer.h"

#include "Base/MathUtil.h"
#include "Core/DMA.h"
#include "Core/ROM.h"
#include "Debug/Console.h"
#include "RomFile/RomFile.h"
#include "System/IO.h"
#include "Utility/Stream.h"

namespace
{
bool gRomLoaded = false;
u8* gRomData = nullptr;
u32 gRomSize = 0;
}

bool RomBuffer::IsRomLoaded() { return gRomLoaded; }

u32 RomBuffer::GetRomSize() { return gRomSize; }

bool RomBuffer::Open()
{
	CNullOutputStream messages;
	std::string filename = g_ROM.FileName;
	ROMFile* rom = ROMFile::Create(filename);
	if (rom == nullptr)
	{
		Console_Print("Failed to create [C%s]\n", filename.c_str());
		return false;
	}

	if (!rom->Open(messages))
	{
		Console_Print("Failed to open [C%s]\n", filename.c_str());
		delete rom;
		return false;
	}

	gRomSize = rom->GetRomSize();

	// Now, allocate memory for rom - round up to a 4 byte boundry
	u32 size_aligned = AlignPow2(gRomSize, 4);
	u8* data = (u8*)malloc(size_aligned);

	if (!rom->LoadData(gRomSize, data, messages))
	{
		Console_Print("Failed to load [C%s]\n", filename.c_str());
		free(data);
		delete rom;
		return false;
	}
	gRomData = data;

	delete rom;

	Console_Print("Opened [C%s]\n", filename.c_str());
	gRomLoaded = true;
	return true;
}

void RomBuffer::Close()
{
	if (gRomData)
	{
		free(gRomData);
		gRomData = nullptr;
	}

	gRomSize = 0;
	gRomLoaded = false;
}

void RomBuffer::GetRomBytesRaw(void* dst, u32 rom_start, u32 length) { memcpy(dst, gRomData + rom_start, length); }

void* RomBuffer::GetAddressRaw(u32 rom_start)
{
	if (rom_start < gRomSize)
	{
		return gRomData + rom_start;
	}

	return nullptr;
}

void RomBuffer::CopyToRam(u8* dst, u32 dst_offset, u32 dst_size, u32 src_offset, u32 length)
{
	DMA_HandleTransfer(dst, dst_offset, dst_size, gRomData, src_offset, gRomSize, length);
}

const void* RomBuffer::GetFixedRomBaseAddress()
{
	DAEDALUS_ASSERT(IsRomLoaded(), "The rom isn't loaded");
	return gRomData;
}
