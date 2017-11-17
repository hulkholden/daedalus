/*
Copyright (C) 2006,2007 StrmnNrmn

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

#ifndef CORE_ROMSETTINGS_H_
#define CORE_ROMSETTINGS_H_

#include <string>

#include "Base/Singleton.h"
#include "Config/RomSettings.h"

class RomID;

class CRomSettingsDB : public CSingleton<CRomSettingsDB>
{
   public:
	virtual ~CRomSettingsDB() {}

	virtual bool OpenSettingsFile(const std::string& filename) = 0;
	virtual void Commit() = 0;

	virtual bool GetSettings(const RomID& id, RomSettings* settings) const = 0;
	virtual void SetSettings(const RomID& id, const RomSettings& settings) = 0;
};

const char* ROM_GetExpansionPakUsageName(EExpansionPakUsage pak_usage);
const char* ROM_GetSaveTypeName(ESaveType save_type);

#endif  // CORE_ROMSETTINGS_H_
