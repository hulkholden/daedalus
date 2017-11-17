/*
Copyright (C) 2001 CyRUS64 (http://www.boob.co.uk)
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

#include "Base/Daedalus.h"
#include "Interface/Preferences.h"

#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <set>
#include <map>

#include "Config/ConfigOptions.h"
#include "Config/Preferences.h"
#include "Input/InputManager.h"
#include "Interface/RomDB.h"
#include "System/IO.h"
#include "System/Paths.h"
#include "Utility/IniFile.h"

class IPreferences : public CPreferences
{
   public:
	IPreferences();
	virtual ~IPreferences();

	bool OpenPreferencesFile(const std::string& filename) override;
	void Commit() override;

	bool GetRomPreferences(const RomID& id, SRomPreferences* preferences) const override;
	void SetRomPreferences(const RomID& id, const SRomPreferences& preferences) override;

   private:
	void OutputSectionDetails(const RomID& id, const SRomPreferences& preferences, FILE* fh);

   private:
	typedef std::map<RomID, SRomPreferences> PreferencesMap;

	PreferencesMap mPreferences;

	bool mDirty;  // (STRMNNRMN - Changed since read from disk?)
	std::string mFilename;
};

template <>
bool CSingleton<CPreferences>::Create()
{
	DAEDALUS_ASSERT_Q(mpInstance == NULL);

	mpInstance = new IPreferences();

	return true;
}

CPreferences::~CPreferences() {}

IPreferences::IPreferences() : mDirty(false)
{
	std::string ini_filename = IO::Path::Join(gDaedalusExePath, "preferences.ini");
	OpenPreferencesFile(ini_filename);
}

IPreferences::~IPreferences()
{
	if (mDirty)
	{
		Commit();
	}
}

static RomID RomIDFromString(const char* str)
{
	u32 crc1, crc2, country;
	sscanf(str, "%08x%08x-%02x", &crc1, &crc2, &country);
	return RomID(crc1, crc2, (u8)country);
}

bool IPreferences::OpenPreferencesFile(const std::string& filename)
{
	mFilename = filename;

	IniFile* inifile = IniFile::Create(filename);
	if (inifile == nullptr)
	{
		return false;
	}

	const IniFileSection* section = inifile->GetDefaultSection();
	if (section != nullptr)
	{
		std::string str_value;
		section->GetProperty("ForceLinearFilter", &gGlobalPreferences.ForceLinearFilter);
		section->GetProperty("RumblePak", &gGlobalPreferences.RumblePak);
	}

	for (u32 section_idx = 0; section_idx < inifile->GetNumSections(); ++section_idx)
	{
		const IniFileSection* section = inifile->GetSection(section_idx);

		RomID id = RomIDFromString(section->name().c_str());
		SRomPreferences preferences;

		int int_value;
		section->GetProperty("PatchesEnabled", &preferences.PatchesEnabled);
		section->GetProperty("SpeedSyncEnabled", &preferences.SpeedSyncEnabled);
		section->GetProperty("DynarecEnabled", &preferences.DynarecEnabled);
		section->GetProperty("DynarecLoopOptimisation", &preferences.DynarecLoopOptimisation);
		section->GetProperty("DynarecDoublesOptimisation", &preferences.DynarecDoublesOptimisation);
		section->GetProperty("DoubleDisplayEnabled", &preferences.DoubleDisplayEnabled);
		section->GetProperty("CleanSceneEnabled", &preferences.CleanSceneEnabled);
		section->GetProperty("ClearDepthFrameBuffer", &preferences.ClearDepthFrameBuffer);
		section->GetProperty("AudioRateMatch", &preferences.AudioRateMatch);
		section->GetProperty("VideoRateMatch", &preferences.VideoRateMatch);
		section->GetProperty("FogEnabled", &preferences.FogEnabled);
		if (section->GetProperty("AudioEnabled", &int_value))
		{
			if (int_value >= APM_DISABLED && int_value <= APM_ENABLED_SYNC)
			{
				preferences.AudioEnabled = static_cast<EAudioPluginMode>(int_value);
			}
		}
		section->GetProperty("MemoryAccessOptimisation", &preferences.MemoryAccessOptimisation);
		section->GetProperty("CheatsEnabled", &preferences.CheatsEnabled);
		mPreferences[id] = preferences;
	}

	mDirty = false;

	delete inifile;
	return true;
}

void IPreferences::OutputSectionDetails(const RomID& id, const SRomPreferences& preferences, FILE* fh)
{
	// Generate the CRC-ID for this rom:
	RomSettings settings;
	CRomSettingsDB::Get()->GetSettings(id, &settings);

	fprintf(fh, "{%08x%08x-%02x}\t// %s\n", id.CRC[0], id.CRC[1], id.CountryID, settings.GameName.c_str());
	fprintf(fh, "PatchesEnabled=%d\n", preferences.PatchesEnabled);
	fprintf(fh, "SpeedSyncEnabled=%d\n", preferences.SpeedSyncEnabled);
	fprintf(fh, "DynarecEnabled=%d\n", preferences.DynarecEnabled);
	fprintf(fh, "DynarecLoopOptimisation=%d\n", preferences.DynarecLoopOptimisation);
	fprintf(fh, "DynarecDoublesOptimisation=%d\n", preferences.DynarecDoublesOptimisation);
	fprintf(fh, "DoubleDisplayEnabled=%d\n", preferences.DoubleDisplayEnabled);
	fprintf(fh, "CleanSceneEnabled=%d\n", preferences.CleanSceneEnabled);
	fprintf(fh, "ClearDepthFrameBuffer=%d\n", preferences.ClearDepthFrameBuffer);
	fprintf(fh, "AudioRateMatch=%d\n", preferences.AudioRateMatch);
	fprintf(fh, "VideoRateMatch=%d\n", preferences.VideoRateMatch);
	fprintf(fh, "FogEnabled=%d\n", preferences.FogEnabled);
	fprintf(fh, "AudioEnabled=%d\n", preferences.AudioEnabled);
	fprintf(fh, "MemoryAccessOptimisation=%d\n", preferences.MemoryAccessOptimisation);
	fprintf(fh, "CheatsEnabled=%d\n", preferences.CheatsEnabled);
	fprintf(fh, "\n");  // Spacer
}

// Write out the .ini file, keeping the original comments intact
void IPreferences::Commit()
{
	FILE* fh(fopen(mFilename.c_str(), "w"));
	if (fh != NULL)
	{
		const SGlobalPreferences defaults;

#define OUTPUT_BOOL(b, nm, def) fprintf(fh, "%s=%s\n", #nm, b.nm ? "yes" : "no");
#define OUTPUT_FLOAT(b, nm, def) fprintf(fh, "%s=%f\n", #nm, b.nm);
#define OUTPUT_INT(b, nm, def) fprintf(fh, "%s=%d\n", #nm, b.nm);
		OUTPUT_BOOL(gGlobalPreferences, ForceLinearFilter, defaults);
		OUTPUT_BOOL(gGlobalPreferences, RumblePak, defaults);
		fprintf(fh, "\n\n");  // Spacer to go before Rom Settings

		for (const auto& it : mPreferences)
		{
			OutputSectionDetails(it.first, it.second, fh);
		}

		fclose(fh);
		mDirty = false;
	}
}

// Retreive the preferences for the specified rom. Returns false if the rom is
// not in the database
bool IPreferences::GetRomPreferences(const RomID& id, SRomPreferences* preferences) const
{
	PreferencesMap::const_iterator it(mPreferences.find(id));
	if (it != mPreferences.end())
	{
		*preferences = it->second;
		return true;
	}
	else
	{
		return false;
	}
}

// Update the preferences for the specified rom - creates a new entry if necessary
void IPreferences::SetRomPreferences(const RomID& id, const SRomPreferences& preferences)
{
	PreferencesMap::iterator it(mPreferences.find(id));
	if (it != mPreferences.end())
	{
		it->second = preferences;
	}
	else
	{
		mPreferences[id] = preferences;
	}

	mDirty = true;
}
