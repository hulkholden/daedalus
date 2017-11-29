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

#pragma once

#ifndef DEBUG_CONSOLE_H_
#define DEBUG_CONSOLE_H_

#include "Base/Macros.h"

// TODO: always compile these in, but provide macros to strip calls out.

#ifdef DAEDALUS_DEBUG_CONSOLE

void DAEDALUS_VARARG_CALL_TYPE Console_Print(const char* format, ...);

void Console_OverwriteStart();
void DAEDALUS_VARARG_CALL_TYPE Console_Overwrite(const char* format, ...);
void Console_OverwriteEnd();
void Console_Flush();

#else

#define Console_Print(...) DAEDALUS_USE(__VA_ARGS__)
#define Console_OverwriteStart() (void)0
#define Console_Overwrite(...) DAEDALUS_USE(__VA_ARGS__)
#define Console_OverwriteEnd() (void)0
#define Console_Flush() (void)0

#endif  // DAEDALUS_DEBUG_CONSOLE

#endif  // DEBUG_CONSOLE_H_
