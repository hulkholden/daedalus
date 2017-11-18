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

#include "System/IO.h"

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "ws2_32.lib")

std::string GetExePath(const char* argv0)
{
	HMODULE hModule = GetModuleHandle(nullptr);
	if (hModule == nullptr)
	{
		return "";
	}

	char exe_path[256];
	GetModuleFileName(hModule, exe_path, ARRAYSIZE(exe_path));
	std::string path = exe_path;
	IO::Path::RemoveFileSpec(&path);
	return path;
}

std::string MakeRomPath(const char* filename)
{
	// Need absolute path when loading from Visual Studio
	// This is ok when loading from console too, since arg0 will be empty, it'll just load file name (arg1)
	std::string rom_path = argv[1];// IO::Path::Join(gDaedalusExePath, argv[1]);
	return rom_path;
}
