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

// Display stuff like registers, instructions, memory usage and so on
#include "Base/Daedalus.h"
#include "Debug/Dump.h"

#include "Debug/DBGConsole.h"
#include "System/IO.h"
#include "System/Paths.h"

// Initialise the directory where files are dumped
// Appends subdir to the global dump base and returns.
std::string Dump_GetDumpDirectory(const std::string& subdir)
{
	std::string dump_dir = IO::Path::Join(gDaedalusExePath, "Dumps", subdir);

#ifdef DAEDALUS_DEBUG_CONSOLE
	if (CDebugConsole::IsAvailable())
	{
		// DBGConsole_Msg( 0, "Dump dir: [C%s]", dump_dir.c_str() );
	}
#endif
	IO::Directory::EnsureExists(dump_dir);
	return dump_dir;
}
