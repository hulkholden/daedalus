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

#include "Base/Daedalus.h"
#include "Core/Disassemble.h"

#include <ctype.h>
#include <stdio.h>

#include "Core/CPU.h"
#include "Core/Memory.h"
#include "Core/PrintOpCode.h"
#include "Core/ROMBuffer.h"
#include "Debug/Console.h"
#include "Debug/Dump.h"
#include "System/IO.h"
#include "System/Paths.h"

static void Dump_DisassembleMIPSRange(FILE* fh, u32 address_offset, const OpCode* b, const OpCode* e)
{
	u32 address(address_offset);
	const OpCode* p(b);
	while (p < e)
	{
		char opinfo[400];

		OpCode op(GetCorrectOp(*p));
#if 0
		if (translate_patches)
		{
			///TODO: We need a way to know this
			if (IsJumpTarget( op ))
			{
				fprintf(fp, "\n");
				fprintf(fp, "\n");
				fprintf(fp, "// %s():\n", Patch_GetJumpAddressName(current_pc));
			}
		}
#endif

		SprintOpCodeInfo(opinfo, address, op);
		fprintf(fh, "0x%08x: <0x%08x> %s\n", address, op._u32, opinfo);

		address += 4;
		++p;
	}
}

//	N.B. This assumbes that b/e are 4 byte aligned (otherwise endianness is broken)
static void Dump_MemoryRange(FILE* fh, u32 address_offset, const u32* b, const u32* e)
{
	u32 address(address_offset);
	const u32* p(b);
	while (p < e)
	{
		fprintf(fh, "0x%08x: %08x %08x %08x %08x ", address, p[0], p[1], p[2], p[3]);

		const u8* p8(reinterpret_cast<const u8*>(p));
		for (u32 i = 0; i < 16; i++)
		{
			u8 c(p8[i ^ U8_TWIDDLE]);
			if (c >= 32 && c < 128)
				fprintf(fh, "%c", c);
			else
				fprintf(fh, ".");

			if ((i % 4) == 3) fprintf(fh, " ");
		}
		fprintf(fh, "\n");

		address += 16;
		p += 4;
	}
}

static void Dump_DisassembleRSPRange(FILE* fh, u32 address_offset, const OpCode* b, const OpCode* e)
{
	u32 address(address_offset);
	const OpCode* p(b);
	while (p < e)
	{
		char opinfo[400];
		SprintRSPOpCodeInfo(opinfo, address, *p);
		fprintf(fh, "0x%08x: <0x%08x> %s\n", address, p->_u32, opinfo);

		address += 4;
		++p;
	}
}

static std::string GetDumpPath(const char* filename, const char* default_filename)
{
	if (filename == NULL || strlen(filename) == 0)
	{
		return IO::Path::Join(Dump_GetDumpDirectory(""), default_filename);
	}
	return filename;
}

void Dump_Disassemble(u32 start, u32 end, const char* p_file_name)
{
	std::string file_path = GetDumpPath(p_file_name, "dis.txt");

	// Cute hack - if the end of the range is less than the start,
	// assume it is a length to disassemble
	if (end < start)
	{
		end = start + end;
	}

	const void * vstart;
	if (!Memory_GetInternalReadAddress(start, &vstart))
	{
		Console_Print("[Ydis: Invalid start 0x%08x]", start);
		return;
	}
	const void * vend;
	if (!Memory_GetInternalReadAddress(end, &vend))
	{
		Console_Print("[Ydis: Invalid end 0x%08x]", end);
		return;
	}

	FILE* fp = fopen(file_path.c_str(), "w");
	if (!fp)
	{
		return;
	}

	Console_Print("Disassembling from 0x%08x to 0x%08x ([C%s])", start, end, file_path.c_str());

	Dump_DisassembleMIPSRange(fp, start, static_cast<const OpCode*>(vstart), static_cast<const OpCode*>(vend));

	fclose(fp);
}

void Dump_RSPDisassemble(const char* p_file_name)
{
	std::string file_path = GetDumpPath(p_file_name, "rdis.txt");

	u32 dstart = 0xa4000000;
	u32 istart = 0xa4001000;
	u32 iend = 0xa4002000;
	const void* dstart_ptr;
	const void* istart_ptr;
	const void* iend_ptr;
	if (!Memory_GetInternalReadAddress(dstart, &dstart_ptr))
	{
		Console_Print("[Yrdis: Invalid dstart 0x%08x]", dstart);
		return;
	}
	if (!Memory_GetInternalReadAddress(istart, &istart_ptr))
	{
		Console_Print("[Yrdis: Invalid istart 0x%08x]", istart);
		return;
	}
	if (!Memory_GetInternalReadAddress(iend, &iend_ptr))
	{
		Console_Print("[Yrdis: Invalid iend 0x%08x]", iend);
		return;
	}

	Console_Print("Disassembling from 0x%08x to 0x%08x ([C%s])", dstart, iend, file_path.c_str());

	FILE* fp = fopen(file_path.c_str(), "w");
	if (!fp)
	{
		return;
	}

	Dump_MemoryRange(fp, dstart,
		static_cast<const u32*>(dstart_ptr),
		static_cast<const u32*>(istart_ptr));
	Dump_DisassembleRSPRange(fp, istart,
		static_cast<const OpCode*>(istart_ptr),
		static_cast<const OpCode*>(iend_ptr));

	fclose(fp);
}

void Dump_Strings(const char* p_file_name)
{
	std::string file_path = GetDumpPath(p_file_name, "strings.txt");

	static const u32 MIN_LENGTH = 5;

	Console_Print("Dumping strings in rom ([C%s])", file_path.c_str());

	FILE* fp = fopen(file_path.c_str(), "w");
	if (!fp)
	{
		return;
	}

	// Memory dump
	u32 ascii_start = 0;
	u32 ascii_count = 0;
	for (u32 i = 0; i < RomBuffer::GetRomSize(); i++)
	{
		if (RomBuffer::ReadValueRaw<u8>(i ^ 0x3) >= ' ' && RomBuffer::ReadValueRaw<u8>(i ^ 0x3) < 200)
		{
			if (ascii_count == 0)
			{
				ascii_start = i;
			}
			ascii_count++;
		}
		else
		{
			if (ascii_count >= MIN_LENGTH)
			{
				fprintf(fp, "0x%08x: ", ascii_start);

				for (u32 j = 0; j < ascii_count; j++)
				{
					fprintf(fp, "%c", RomBuffer::ReadValueRaw<u8>((ascii_start + j) ^ 0x3));
				}

				fprintf(fp, "\n");
			}

			ascii_count = 0;
		}
	}
	fclose(fp);
}
