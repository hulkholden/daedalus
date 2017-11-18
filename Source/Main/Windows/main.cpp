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

#include <gflags/gflags.h>

#include "Config/ConfigOptions.h"
#include "Core/CPU.h"
#include "Core/ROM.h"
#include "Core/RomSettings.h"
#include "Debug/DBGConsole.h"
#include "Debug/DebugLog.h"
#include "Interface/Preferences.h"
#include "Interface/RomDB.h"
#include "Main/SystemInit.h"
#include "System/IO.h"
#include "System/Paths.h"
#include "Test/BatchTest.h"
#include "Utility/Profiler.h"

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "ws2_32.lib")

DEFINE_bool(batch, false, "Run in batch testing mode.");
DEFINE_string(roms, "", "The roms directory.");

int __cdecl main(int argc, char **argv)
{
	gflags::ParseCommandLineFlags(&argc, &argv, true);

	HMODULE hModule = GetModuleHandle(NULL);
	if (hModule != NULL)
	{
		char exe_path[256];
		GetModuleFileName(hModule, exe_path, ARRAYSIZE(exe_path));
		gDaedalusExePath = exe_path;
		IO::Path::RemoveFileSpec(&gDaedalusExePath);
	}
	else
	{
		fprintf(stderr, "Couldn't determine executable path\n");
		return 1;
	}

	int result = 0;

	if (!System_Init())
	{
		fprintf(stderr, "System_Init failed\n");
		return 1;
	}

	if (FLAGS_batch)
	{
		BatchTestMain();
	}
	else
	{
		if (argc != 2)
		{
			fprintf(stderr, "Usage: daedalus [rom]\n");
			return 1;
		}

		// Need absolute path when loading from Visual Studio
		// This is ok when loading from console too, since arg0 will be empty, it'll just load file name (arg1)
		std::string rom_path = argv[1];// IO::Path::Join(gDaedalusExePath, argv[1]);
		fprintf(stderr, "Loading %s\n", rom_path.c_str());
		if (!System_Open(rom_path))
		{
			fprintf(stderr, "System_Open failed\n");
			return 1;
		}
		CPU_Run();
		System_Close();
	}

	System_Finalize();
	return result;
}
