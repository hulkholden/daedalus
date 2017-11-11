/*
 * Copyright (C) 2001 CyRUS64 (http://www.boob.co.uk)
 * Copyright (C) 2006 StrmnNrmn
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#pragma once

#ifndef UTILITY_INIFILE_H_
#define UTILITY_INIFILE_H_

#include <string>
#include <map>
#include <vector>

#include "absl/strings/string_view.h"

#include "Base/Types.h"

class IniFileSection
{
   public:
	IniFileSection(absl::string_view name) : mName(name) {}

	const std::string& name() const { return mName; }

	void AddProperty(const std::string& key, const std::string& value) { mProperties[key] = value; }

	bool GetProperty(const std::string& name, std::string* value) const;
	bool GetProperty(const std::string& name, bool* value) const;
	bool GetProperty(const std::string& name, int* value) const;
	bool GetProperty(const std::string& name, u32* value) const;
	bool GetProperty(const std::string& name, float* value) const;

   private:
	std::string mName;
	std::map<std::string, std::string> mProperties;
};

class IniFile
{
   private:
	IniFile() : mpDefaultSection(nullptr) {}
	bool Open(const std::string& filename);

   public:
	~IniFile();

	static IniFile* Create(const std::string& filename);

	const IniFileSection* GetDefaultSection() const { return mpDefaultSection; }

	u32 GetNumSections() const { return mSections.size(); }
	const IniFileSection* GetSection(u32 section_idx) const;

	const IniFileSection* GetSectionByName(const char* section_name) const;

   private:
	IniFileSection* mpDefaultSection;
	std::vector<IniFileSection*> mSections;
};

#endif  // UTILITY_INIFILE_H_
