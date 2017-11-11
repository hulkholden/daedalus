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

#include "stdafx.h"
#include "System/IO.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>

#include "absl/strings/match.h"
#include "absl/strings/str_cat.h"

namespace IO
{
const char kPathSeparator = '/';

namespace File
{
bool Move(const std::string& from, const std::string& to)
{
	return rename(from.c_str(), to.c_str()) >= 0;
}

bool Delete(const std::string& file)
{
	return remove(file.c_str()) == 0;
}

bool Exists(const std::string& path)
{
	// TODO(strmnnrmn): Just stat?
	FILE* fh = fopen(path.c_str(), "rb");
	if (fh)
	{
		fclose(fh);
		return true;
	}
	return false;
}
}
namespace Directory
{
bool Create(const std::string& path)
{
	return mkdir(path.c_str(), 0777) == 0;
}

bool IsDirectory(const std::string& path)
{
	struct stat s;

	if (stat(path.c_str(), &s) == 0)
	{
		if (s.st_mode & S_IFDIR)
		{
			return true;
		}
	}

	return false;
}
}  // namespace File

namespace Path
{
char* Combine(char* p_dest, const char* p_dir, const char* p_file)
{
	strcpy(p_dest, p_dir);
	Append(p_dest, p_file);
	return p_dest;
}

bool Append(char* p_path, const char* p_more)
{
	u32 len(strlen(p_path));

	if (len > 0)
	{
		if (p_path[len - 1] != kPathSeparator)
		{
			p_path[len] = kPathSeparator;
			p_path[len + 1] = '\0';
			len++;
		}
	}
	strcat(p_path, p_more);
	return true;
}

const char* FindExtension(const char* p_path) { return strrchr(p_path, '.'); }

const char* FindFileName(const char* p_path)
{
	const char* p_last_slash = strrchr(p_path, kPathSeparator);
	if (p_last_slash)
	{
		return p_last_slash + 1;
	}
	else
	{
		return nullptr;
	}
}

}  // namespace Path

bool FindFileOpen(const std::string& path, FindHandleT* handle, FindDataT& data)
{
	DIR* d = opendir(path.c_str());
	if (d != nullptr)
	{
		// To support findfirstfile() API we must return the first result immediately
		if (FindFileNext(d, data))
		{
			*handle = d;
			return true;
		}

		closedir(d);
	}

	return false;
}

bool FindFileNext(FindHandleT handle, FindDataT& data)
{
	DAEDALUS_ASSERT(handle != nullptr, "Cannot search with invalid directory handle");

	while (dirent* ep = readdir(static_cast<DIR*>(handle)))
	{
		// Ignore hidden files (and '.' and '..')
		if (ep->d_name[0] == '.') continue;

		IO::Path::Assign(data.Name, ep->d_name);
		return true;
	}

	return false;
}

bool FindFileClose(FindHandleT handle)
{
	DAEDALUS_ASSERT(handle != nullptr, "Trying to close an invalid directory handle");

	return closedir(static_cast<DIR*>(handle)) >= 0;
}
}  // namespace IO
