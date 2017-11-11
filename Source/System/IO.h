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

#ifndef SYSTEM_IO_H_
#define SYSTEM_IO_H_

#include "Base/Types.h"

#include <string.h>

#include "absl/strings/string_view.h"

namespace IO
{
namespace File
{
bool Move(const std::string& from, const std::string& to);
bool Delete(const std::string& file);
bool Exists(const std::string& path);
}
namespace Directory
{
bool Create(const std::string& path);
bool EnsureExists(const std::string& p_path);
bool IsDirectory(const std::string& path);
}

namespace Path
{
std::string Join(absl::string_view a, absl::string_view b);
std::string Join(absl::string_view a, absl::string_view b, absl::string_view c);

absl::string_view FindFileName(const std::string& path);
bool RemoveBackslash(std::string* path);
bool RemoveFileSpec(std::string* path);
bool RemoveExtension(std::string* path);
void AddExtension(std::string* path, absl::string_view ext);

inline void SetExtension(std::string* path, absl::string_view ext)
{
	RemoveExtension(path);
	AddExtension(path, ext);
}
}

struct FindDataT
{
	std::string Name;
};

#if defined(DAEDALUS_W32)
typedef intptr_t FindHandleT;
#elif defined(DAEDALUS_OSX) || defined(DAEDALUS_LINUX)
typedef void* FindHandleT;
#else
#error Need to define FindHandleT for this platform
#endif

bool FindFileOpen(const std::string& path, FindHandleT* handle, FindDataT& data);
bool FindFileNext(FindHandleT handle, FindDataT& data);
bool FindFileClose(FindHandleT handle);
}

#endif  // SYSTEM_IO_H_
