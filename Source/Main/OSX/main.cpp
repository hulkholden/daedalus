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

#include "Core/CPU.h"
#include "Debug/DBGConsole.h"
#include "Main/SystemInit.h"
#include "System/IO.h"
#include "System/Paths.h"
#include "Test/BatchTest.h"

#ifdef DAEDALUS_LINUX
#include <linux/limits.h>
#endif

int main(int argc, char **argv)
{
	InstallAbortHandlers();
	int result = 0;

	if (argc > 0)
	{
		char* exe_path = realpath(argv[0], nullptr);
		gDaedalusExePath = exe_path;
		free(exe_path);

		IO::Path::RemoveFileSpec(&gDaedalusExePath);
	}
	else
	{
		fprintf(stderr, "Couldn't determine executable path\n");
		return 1;
	}

	if (!System_Init())
	{
		fprintf(stderr, "System_Init failed\n");
		return 1;
	}

	if (argc > 1)
	{
		bool batch_test = false;
		const char *filename = NULL;

		for (int i = 1; i < argc; ++i)
		{
			const char *arg = argv[i];
			if (*arg == '-')
			{
				++arg;
				if (strcmp(arg, "-batch") == 0)
				{
					batch_test = true;
					break;
				}
			}
			else
			{
				filename = arg;
			}
		}

		if (batch_test)
		{
			BatchTestMain(argc, argv);
		}
		else if (filename)
		{
			if (!System_Open(filename))
			{
				fprintf(stderr, "System_Open failed\n");
			}
			CPU_Run();
			System_Close();
		}
	}
	else
	{
		fprintf(stderr, "Usage: daedalus [rom]\n");
		return 1;
	}

	System_Finalize();
	return result;
}

// FIXME: All this stuff needs tidying

void Dynarec_ClearedCPUStuffToDo() {}

void Dynarec_SetCPUStuffToDo() {}

extern "C" {
void _EnterDynaRec() { DAEDALUS_ASSERT(false, "Unimplemented"); }
}
