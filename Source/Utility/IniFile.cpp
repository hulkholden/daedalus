/*
Copyright (C) 2001 CyRUS64 (http://www.boob.co.uk)
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
#include "Utility/IniFile.h"

#include <stdio.h>

#include "absl/strings/numbers.h"
#include "absl/strings/str_split.h"
#include "absl/strings/string_view.h"
#include "absl/strings/strip.h"

bool IniFileSection::GetProperty(const std::string& name, std::string* value) const
{
	const auto& it = mProperties.find(name);
	if (it != mProperties.end())
	{
		*value = it->second;
		return true;
	}
	return false;
}

bool IniFileSection::GetProperty(const std::string& name, bool* value) const
{
	std::string str_value;
	if (!GetProperty(name, &str_value))
	{
		return false;
	}

	const char* str = str_value.c_str();

	if (_strcmpi(str, "yes") == 0 || _strcmpi(str, "true") == 0 || _strcmpi(str, "1") == 0 || _strcmpi(str, "on") == 0)
	{
		*value = true;
		return true;
	}
	if (_strcmpi(str, "no") == 0 || _strcmpi(str, "false") == 0 || _strcmpi(str, "0") == 0 || _strcmpi(str, "off") == 0)
	{
		*value = false;
		return true;
	}

	return false;
}

bool IniFileSection::GetProperty(const std::string& name, int* value) const
{
	std::string str;
	if (!GetProperty(name, &str))
	{
		return false;
	}

	return absl::SimpleAtoi(str, value);
}

bool IniFileSection::GetProperty(const std::string& name, u32* value) const
{
	std::string str;
	if (!GetProperty(name, &str))
	{
		return false;
	}

	return absl::SimpleAtoi(str, value);
}

bool IniFileSection::GetProperty(const std::string& name, float* value) const
{
	std::string str;
	if (!GetProperty(name, &str))
	{
		return false;
	}

	return absl::SimpleAtof(str, value);
}

IniFile::~IniFile()
{
	delete mpDefaultSection;

	for (u32 i = 0; i < mSections.size(); ++i)
	{
		delete mSections[i];
	}
}

IniFile* IniFile::Create(const std::string& filename)
{
	IniFile* p_file = new IniFile();
	if (p_file)
	{
		if (p_file->Open(filename))
		{
			return p_file;
		}

		delete p_file;
	}

	return nullptr;
}

bool IniFile::Open(const std::string& filename)
{
	FILE* fh = fopen(filename.c_str(), "r");
	if (!fh)
	{
		return false;
	}

	// By default start with the default section
	mpDefaultSection = new IniFileSection("");
	IniFileSection* current_section = mpDefaultSection;

	const u32 kBufferLen = 1024;
	char buffer[kBufferLen + 1];
	buffer[kBufferLen] = '\0';

	// XXXX Using fgets needs reworking...
	while (fgets(buffer, kBufferLen, fh) != nullptr)
	{
		absl::string_view line(buffer);
		line = absl::StripTrailingAsciiWhitespace(line);

		if (line.empty())
		{
			continue;
		}

		// Handle comments
		if (line[0] == '/')
		{
			continue;
		}

		// Check for a section heading
		if (absl::ConsumePrefix(&line, "["))
		{
			line = absl::StripSuffix(line, "]");
			current_section = new IniFileSection(line);
			mSections.push_back(current_section);
		}
		else if (absl::ConsumePrefix(&line, "{"))
		{
			line = absl::StripSuffix(line, "}");
			current_section = new IniFileSection(line);
			mSections.push_back(current_section);
		}
		else
		{
			// FIXME: check this is ok if value is empty
			std::pair<std::string, std::string> kv = absl::StrSplit(line, absl::MaxSplits('=', 1));
			absl::StripTrailingAsciiWhitespace(&kv.first);
			absl::StripTrailingAsciiWhitespace(&kv.second);

			DAEDALUS_ASSERT(current_section != nullptr, "There is no current section");
			current_section->AddProperty(kv.first, kv.second);
		}
	}
	fclose(fh);
	return true;
}

const IniFileSection* IniFile::GetSection(u32 section_idx) const
{
	if (section_idx < mSections.size())
	{
		return mSections[section_idx];
	}

	DAEDALUS_ERROR("Invalid section index");
	return nullptr;
}

const IniFileSection* IniFile::GetSectionByName(const char* section_name) const
{
	// TODO: Could use a map for this if it starts to prove expensive
	for (u32 i = 0; i < mSections.size(); ++i)
	{
		if (mSections[i]->name() == section_name)
		{
			return mSections[i];
		}
	}

	return nullptr;
}
