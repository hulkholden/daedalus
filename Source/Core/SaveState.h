/*

  Copyright (C) 2001 Lkb

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

#ifndef CORE_SAVESTATE_H_
#define CORE_SAVESTATE_H_

#include <string>

class RomID;

bool SaveState_LoadFromFile(const std::string& filename);
bool SaveState_SaveToFile(const std::string& filename);
RomID SaveState_GetRomID(const std::string& filename);
const char* SaveState_GetRom(const std::string& filename);

#endif // CORE_SAVESTATE_H_
