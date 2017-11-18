/*
Copyright (C) 2012 StrmnNrmn

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

#include "Core/CPU.h"
#include "Debug/DBGConsole.h"
#include "Main/SystemInit.h"
#include "System/IO.h"
#include "System/Paths.h"
#include "Test/BatchTest.h"

DEFINE_bool(batch, false, "Run in batch testing mode.");
DEFINE_string(roms, "", "The roms directory.");

// Platform-specific.
std::string GetExePath(const char* argv0);
std::string MakeRomPath(const char* filename);

int DAEDALUS_VARARG_CALL_TYPE main(int argc, char **argv)
{
#if defined(DAEDALUS_OSX)
	InstallAbortHandlers();
#endif
	gflags::ParseCommandLineFlags(&argc, &argv, true);

	if (argc == 0)
	{
		fprintf(stderr, "No argv[0]?\n");
		return 1;
	}

	gDaedalusExePath = GetExePath(argv[0]);
	if (gDaedalusExePath.empty())
	{
		fprintf(stderr, "Couldn't determine executable path\n");
		return 1;
	}

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

		std::string rom_path = MakeRomPath(argv[1]);
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
	return 0;
}
