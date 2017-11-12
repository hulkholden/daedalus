/*
Copyright (C) 2008 StrmnNrmn

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

#pragma once

#ifndef PLATFORM_WINDOWS_PLATFORM_H_
#define PLATFORM_WINDOWS_PLATFORM_H_

#define __PRETTY_FUNCTION__ __FUNCTION__
#define _DO_NOT_DECLARE_INTERLOCKED_INTRINSICS_IN_MEMORY
#define __has_feature(x) 0

#undef DAEDALUS_BREAKPOINTS_ENABLED

#define DAEDALUS_ENDIAN_MODE DAEDALUS_ENDIAN_LITTLE

#endif // PLATFORM_WINDOWS_PLATFORM_H_
