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

#include <Shlwapi.h>
#include <io.h>


namespace IO
{
	namespace File
	{
		bool	Move( const std::string& from, const std::string& to )
		{
			return ::MoveFile( from.c_str(), to.c_str() ) ? true : false;
		}

		bool	Delete( const std::string& file )
		{
			return ::DeleteFile( file.c_str() ) ? true : false;
		}

		bool	Exists( const std::string& path )
		{
			return ::PathFileExists( path.c_str() ) ? true : false;
		}
	}
	namespace Directory
	{
		bool	Create( const std::string& path )
		{
			return ::CreateDirectory( path.c_str(), NULL ) ? true : false;
		}

		bool	IsDirectory( const std::string& path )
		{
			return ::PathIsDirectory( path.c_str() ) ? true : false;
		}
	}

	namespace Path
	{
		absl::string_view FindFileName( const std::string& path )
		{
			return ::PathFindFileName( path.c_str() );
		}

	}

	bool	FindFileOpen( const std::string& path, FindHandleT * handle, FindDataT & data )
	{
		std::string name = Path::Join(path, "*.*");
		_finddata_t	_data;
		*handle = _findfirst( name.c_str(), &_data );

		if( *handle != -1 )
		{
			data.Name = _data.name;

			// Ignore hidden files (and '.' and '..')
			if (_data.name[0] == '.')
			{
				if (!FindFileNext(*handle, data))
				{
					FindFileClose(*handle);
					*handle = -1;
					return false;
				}
			}
			return true;
		}

		return false;
	}

	bool	FindFileNext( FindHandleT handle, FindDataT & data )
	{
		DAEDALUS_ASSERT( handle != -1, "Cannot search with invalid directory handle" );

		_finddata_t	_data;
		while ( _findnext( handle, &_data ) != -1)
		{
			// Ignore hidden files (and '.' and '..')
			if (_data.name[0] == '.')
				continue;

			data.Name = _data.name;
			return true;
		}

		return false;
	}

	bool	FindFileClose( FindHandleT handle )
	{
		DAEDALUS_ASSERT( handle != -1, "Trying to close an invalid directory handle" );

		return _findclose( handle ) != -1;
	}

}
