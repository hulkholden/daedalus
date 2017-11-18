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

#include <limits.h>
#include <stdlib.h>

#include <string>

#include "System/IO.h"

std::string GetExePath(const char* argv0)
{
	char* exe_path = realpath(argv0, nullptr);
	std::string path = exe_path;
	free(exe_path);
	IO::Path::RemoveFileSpec(&path);
	return path;
}

std::string MakeRomPath(const char* filename)
{
	return filename;
}

// FIXME: All this stuff needs tidying

void Dynarec_ClearedCPUStuffToDo() {}

void Dynarec_SetCPUStuffToDo() {}

extern "C" {
void _EnterDynaRec() { DAEDALUS_ASSERT(false, "Unimplemented"); }
}
