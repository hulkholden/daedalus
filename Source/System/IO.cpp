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

#include "Base/Daedalus.h"
#include "System/IO.h"

#include "absl/strings/match.h"
#include "absl/strings/str_cat.h"

namespace IO
{
namespace Directory
{
bool EnsureExists(const std::string& path)
{
	if (IsDirectory(path))
	{
		return true;
	}

	// Make sure parent exists,
	std::string path_parent = path;
	IO::Path::RemoveBackslash(&path_parent);
	if (IO::Path::RemoveFileSpec(&path_parent))
	{
		//	Recursively create parents.
		if (!EnsureExists(path_parent))
		{
			return false;
		}
	}

	return Create(path);
}

}
namespace Path
{
const char kPathSeparator = '/';
const char kPathSeparatorStr[] = "/";

#ifdef DAEDALUS_W32
const char kPathSeparatorWin = '\\';
#endif

std::string Join(absl::string_view a, absl::string_view b)
{
	if (absl::EndsWith(a, kPathSeparatorStr))
	{
		return absl::StrCat(a, b);
	}
	return absl::StrCat(a, kPathSeparatorStr, b);
}

std::string Join(absl::string_view a, absl::string_view b, absl::string_view c)
{
	return Join(Join(a, b), c);
}

void AddExtension(std::string* path, absl::string_view ext)
{
	absl::StrAppend(path, ext);
}

bool RemoveExtension(std::string* path)
{
	size_t idx = path->rfind('.');
	if (idx != std::string::npos)
	{
		path->resize(idx);
		return true;
	}
	return false;
}

bool RemoveFileSpec(std::string* path)
{
	size_t idx = path->rfind(kPathSeparator);
	if (idx != std::string::npos)
	{
		path->resize(idx);
		return true;
	}
#ifdef DAEDALUS_W32
	idx = path->rfind(kPathSeparatorWin);
	if (idx != std::string::npos)
	{
		path->resize(idx);
		return true;
	}
#endif

	return false;
}

bool RemoveBackslash(std::string* path)
{
	size_t len = path->length();
	while (!path->empty() && path->back() == kPathSeparator)
	{
		path->pop_back();
	}
#ifdef DAEDALUS_W32
	while (!path->empty() && path->back() == kPathSeparatorWin)
	{
		path->pop_back();
	}
#endif

	return len != path->length();
}


}  // namespace Path
}  // namespace IO
