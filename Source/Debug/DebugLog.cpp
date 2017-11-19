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
#include "Debug/DebugLog.h"

#include <stdio.h>

#include "gflags/gflags.h"

#include "Debug/DBGConsole.h"
#include "Debug/Dump.h"
#include "System/IO.h"

#ifdef DAEDALUS_LOG

static FILE* g_hOutputLog = nullptr;

DEFINE_string(log_filename, "", "The debug log filename. If unset, log outut is sent to stderr.");

bool Debug_InitLogging()
{
	std::string log_filename = FLAGS_log_filename;
	if (log_filename.empty())
	{
		g_hOutputLog = stderr;
	}
	else
	{
		g_hOutputLog = fopen(log_filename.c_str(), "w");
		if (!g_hOutputLog)
		{
			Console_Print("Can't open %s", log_filename.c_str());
		}
	}
	return g_hOutputLog != nullptr;
}

void Debug_FinishLogging()
{
	if (g_hOutputLog && g_hOutputLog != stderr)
	{
		fclose(g_hOutputLog);
	}
	g_hOutputLog = nullptr;
}

void Debug_Print(const char* format, ...)
{
	if (!g_hOutputLog || format == nullptr)
	{
		return;
	}
	char buffer[1024 + 1];
	char* p = buffer;
	va_list va;
	va_start(va, format);
	vsprintf(p, format, va);
	va_end(va);
	fprintf(g_hOutputLog, "%s\n", p);
}

#endif  // DAEDALUS_LOG
