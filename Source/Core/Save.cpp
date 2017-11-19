/*
Copyright (C) 2008-2009 Howard Su (howard0su@gmail.com)

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

#include "Core/Save.h"

#include "Config/ConfigOptions.h"
#include "Core/Memory.h"
#include "Core/ROM.h"
#include "Debug/Console.h"
#include "Debug/Dump.h"
#include "System/IO.h"

static void InitMempackContent();

static std::string		gSaveFileName;
static bool				gSaveDirty;
static u32				gSaveSize;

static std::string		gMempackFileName;
static bool				gMempackDirty;

std::string Save_GetDirectory(const char* extension)
{
	std::string rom_filename = g_ROM.FileName;
	// If the Save path has not yet been set up, prompt user
	if (g_DaedalusConfig.SaveDir.empty())
	{
		// FIXME: missing prompt here!

		// User may have cancelled
		if (g_DaedalusConfig.SaveDir.empty())
		{
			// Default to rom path
			std::string save_dir = rom_filename;
			IO::Path::RemoveFileSpec(&save_dir);
			// FIXME(strmnnrmn): for OSX I generate savegames in a subdir Save, to make it easier to clean up.
			g_DaedalusConfig.SaveDir = IO::Path::Join(save_dir, "Save");

#ifdef DAEDALUS_DEBUG_CONSOLE
			if (CDebugConsole::IsAvailable())
			{
				Console_Print("SaveDir is still empty - defaulting to [C%s]", g_DaedalusConfig.SaveDir.c_str());
			}
#endif
		}
	}

	IO::Directory::EnsureExists(g_DaedalusConfig.SaveDir);

	// Form the filename from the file spec (i.e. strip path and replace the extension)
	absl::string_view fn_view = IO::Path::FindFileName(rom_filename);
	std::string filename(fn_view.begin(), fn_view.end());
	IO::Path::SetExtension(&filename, extension);
	return IO::Path::Join(g_DaedalusConfig.SaveDir, filename);
}

bool Save_Reset()
{
	const char * ext;
	switch (g_ROM.settings.SaveType)
	{
	case SAVE_TYPE_EEP4K:
		ext = ".sav";
		gSaveSize = 4 * 1024;
		break;
	case SAVE_TYPE_EEP16K:
		ext = ".sav";
		gSaveSize = 16 * 1024;
		break;
	case SAVE_TYPE_SRAM:
		ext = ".sra";
		gSaveSize = 32 * 1024;
		break;
	case SAVE_TYPE_FLASH:
		ext = ".fla";
		gSaveSize = 128 * 1024;
		break;
	default:
		ext = "";
		gSaveSize = 0;
		break;
	}

	DAEDALUS_ASSERT( gSaveSize <= gMemBufferSizes[MEM_SAVE], "Save size is larger than allocated memory");
	gSaveDirty = false;
	if (gSaveSize > 0)
	{
		gSaveFileName = Save_GetDirectory(ext);

		FILE * fp = fopen(gSaveFileName.c_str(), "rb");
		if (fp != NULL)
		{
			Console_Print("Loading save from [C%s]", gSaveFileName.c_str());

			u8 buffer[2048];
			u8 * dst = (u8*)gMemBuffers[MEM_SAVE];

			for (u32 d = 0; d < gSaveSize; d += sizeof(buffer))
			{
				fread(buffer, sizeof(buffer), 1, fp);

				for (u32 i = 0; i < sizeof(buffer); i++)
				{
					dst[d+i] = buffer[i^U8_TWIDDLE];
				}
			}
			fclose(fp);
		}
		else
		{
			Console_Print("Save File [C%s] cannot be found.", gSaveFileName.c_str());
		}
	}

	// init mempack
	{
		gMempackFileName = Save_GetDirectory(".mpk");
		FILE * fp = fopen(gMempackFileName.c_str(), "rb");
		if (fp != NULL)
		{
			Console_Print("Loading MemPack from [C%s]", gMempackFileName.c_str());
			fread(gMemBuffers[MEM_MEMPACK], gMemBufferSizes[MEM_MEMPACK], 1, fp);
			fclose(fp);
			gMempackDirty = false;
		}
		else
		{
			Console_Print("MemPack File [C%s] cannot be found.", gMempackFileName.c_str());
			InitMempackContent();
			gMempackDirty = true;
		}
	}


	return true;
}

void Save_Fini()
{
	Save_Flush(true);
}

void Save_MarkSaveDirty()
{
	gSaveDirty = true;
}

void Save_MarkMempackDirty()
{
	gMempackDirty = true;
}

void Save_Flush(bool force)
{
	if ((gSaveDirty || force) && g_ROM.settings.SaveType != SAVE_TYPE_UNKNOWN)
	{
		Console_Print("Saving to [C%s]", gSaveFileName.c_str());

		FILE * fp = fopen(gSaveFileName.c_str(), "wb");
		if (fp != NULL)
		{
			u8 buffer[2048];
			u8 * src = (u8*)gMemBuffers[MEM_SAVE];

			for (u32 d = 0; d < gSaveSize; d += sizeof(buffer))
			{
				for (u32 i = 0; i < sizeof(buffer); i++)
				{
					buffer[i^U8_TWIDDLE] = src[d+i];
				}
				fwrite(buffer, 1, sizeof(buffer), fp);
			}
			fclose(fp);
		}
		gSaveDirty = false;
	}

	if (gMempackDirty || force)
	{
		Console_Print("Saving MemPack to [C%s]", gMempackFileName.c_str());

		FILE * fp = fopen(gMempackFileName.c_str(), "wb");
		if (fp != NULL)
		{
			fwrite(gMemBuffers[MEM_MEMPACK], gMemBufferSizes[MEM_MEMPACK], 1, fp);
			fclose(fp);
		}
		gMempackDirty = false;
	}
}

// Mempack Stuffs

//
// Initialisation values taken from PJ64
//
static const u8 gMempackInitialize[] =
{
	0x81,0x01,0x02,0x03, 0x04,0x05,0x06,0x07, 0x08,0x09,0x0a,0x0b, 0x0C,0x0D,0x0E,0x0F,
	0x10,0x11,0x12,0x13, 0x14,0x15,0x16,0x17, 0x18,0x19,0x1A,0x1B, 0x1C,0x1D,0x1E,0x1F,
	0xFF,0xFF,0xFF,0xFF, 0x05,0x1A,0x5F,0x13, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
	0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0x01,0xFF, 0x66,0x25,0x99,0xCD,
	0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
	0xFF,0xFF,0xFF,0xFF, 0x05,0x1A,0x5F,0x13, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
	0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0x01,0xFF, 0x66,0x25,0x99,0xCD,
	0xFF,0xFF,0xFF,0xFF, 0x05,0x1A,0x5F,0x13, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
	0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0x01,0xFF, 0x66,0x25,0x99,0xCD,
	0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
	0xFF,0xFF,0xFF,0xFF, 0x05,0x1A,0x5F,0x13, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
	0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0x01,0xFF, 0x66,0x25,0x99,0xCD,
	0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
	0x00,0x71,0x00,0x03, 0x00,0x03,0x00,0x03, 0x00,0x03,0x00,0x03, 0x00,0x03,0x00,0x03,
};

static void InitMempackContent()
{
	for (size_t dst_off = 0; dst_off < gMemBufferSizes[MEM_MEMPACK]; dst_off += 32 * 1024)
	{
		u8 * mempack = (u8*)gMemBuffers[MEM_MEMPACK] + dst_off;
		for (u32 i = 0; i < 0x8000; i += 2)
		{
			mempack[i + 0] = 0x00;
			mempack[i + 1] = 0x03;
		}
		memcpy(mempack, gMempackInitialize, sizeof(gMempackInitialize));

		DAEDALUS_ASSERT(dst_off + 0x8000 <= gMemBufferSizes[MEM_MEMPACK], "Buffer overflow");
		DAEDALUS_ASSERT(dst_off + sizeof(gMempackInitialize) <= gMemBufferSizes[MEM_MEMPACK], "Buffer overflow");
	}
}
