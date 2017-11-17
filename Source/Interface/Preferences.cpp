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
#include "Input/InputManager.h"
#include "Interface/RomDB.h"
#include "System/IO.h"
#include "System/Paths.h"
#include "Utility/IniFile.h"

static u32 GetTexureHashFrequencyAsFrames(ETextureHashFrequency thf);
static ETextureHashFrequency GetTextureHashFrequencyFromFrames(u32 frames);

static u32 GetFrameskipValueAsInt(EFrameskipValue value);
static EFrameskipValue GetFrameskipValueFromInt(u32 value);

extern EFrameskipValue gFrameskipValue;
extern f32 gZoomX;

SGlobalPreferences gGlobalPreferences;

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
		section->GetProperty("DisplayFramerate", &gGlobalPreferences.DisplayFramerate);
		section->GetProperty("ForceLinearFilter", &gGlobalPreferences.ForceLinearFilter);
		section->GetProperty("RumblePak", &gGlobalPreferences.RumblePak);
#ifdef DAEDALUS_DEBUG_DISPLAYLIST
		section->GetProperty("HighlightInexactBlendModes", &gGlobalPreferences.HighlightInexactBlendModes);
		section->GetProperty("CustomBlendModes", &gGlobalPreferences.CustomBlendModes);
#endif
		section->GetProperty("BatteryWarning", &gGlobalPreferences.BatteryWarning);
		section->GetProperty("LargeROMBuffer", &gGlobalPreferences.LargeROMBuffer);
		section->GetProperty("StickMinDeadzone", &gGlobalPreferences.StickMinDeadzone);
		section->GetProperty("StickMaxDeadzone", &gGlobalPreferences.StickMaxDeadzone);
		int int_value;
		if (section->GetProperty("GuiColor", &int_value))
		{
			if (int_value >= 0 && int_value < NUM_COLOR_TYPES)
			{
				gGlobalPreferences.GuiColor = EGuiColor(int_value);
			}
		}

		if (section->GetProperty("ViewportType", &int_value))
		{
			if (int_value >= 0 && int_value < NUM_VIEWPORT_TYPES)
			{
				gGlobalPreferences.ViewportType = EViewportType(int_value);
			}
		}

		section->GetProperty("TVEnable", &gGlobalPreferences.TVEnable);
		section->GetProperty("TVLaced", &gGlobalPreferences.TVLaced);
		if (section->GetProperty("TVType", &int_value))
		{
			if (int_value >= 0 && int_value < 2)
			{
				gGlobalPreferences.TVType = ETVType(int_value);
			}
		}
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
		if (section->GetProperty("CheckTextureHashFrequency", &int_value))
		{
			preferences.CheckTextureHashFrequency = GetTextureHashFrequencyFromFrames(int_value);
		}
		if (section->GetProperty("Frameskip", &int_value))
		{
			preferences.Frameskip = GetFrameskipValueFromInt(int_value);
		}
		if (section->GetProperty("AudioEnabled", &int_value))
		{
			if (int_value >= APM_DISABLED && int_value <= APM_ENABLED_SYNC)
			{
				preferences.AudioEnabled = static_cast<EAudioPluginMode>(int_value);
			}
		}
		section->GetProperty("ZoomX", &preferences.ZoomX);
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
	fprintf(fh, "CheckTextureHashFrequency=%d\n",
			GetTexureHashFrequencyAsFrames(preferences.CheckTextureHashFrequency));
	fprintf(fh, "Frameskip=%d\n", GetFrameskipValueAsInt(preferences.Frameskip));
	fprintf(fh, "AudioEnabled=%d\n", preferences.AudioEnabled);
	fprintf(fh, "ZoomX=%f\n", preferences.ZoomX);
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
		OUTPUT_INT(gGlobalPreferences, DisplayFramerate, defaults);
		OUTPUT_BOOL(gGlobalPreferences, ForceLinearFilter, defaults);
		OUTPUT_BOOL(gGlobalPreferences, RumblePak, defaults);
#ifdef DAEDALUS_DEBUG_DISPLAYLIST
		OUTPUT_BOOL(gGlobalPreferences, HighlightInexactBlendModes, defaults);
		OUTPUT_BOOL(gGlobalPreferences, CustomBlendModes, defaults);
#endif
		OUTPUT_BOOL(gGlobalPreferences, BatteryWarning, defaults);
		OUTPUT_BOOL(gGlobalPreferences, LargeROMBuffer, defaults);
		OUTPUT_INT(gGlobalPreferences, GuiColor, defaults)
		OUTPUT_FLOAT(gGlobalPreferences, StickMinDeadzone, defaults);
		OUTPUT_FLOAT(gGlobalPreferences, StickMaxDeadzone, defaults);
		OUTPUT_INT(gGlobalPreferences, ViewportType, defaults);
		OUTPUT_BOOL(gGlobalPreferences, TVEnable, defaults);
		OUTPUT_BOOL(gGlobalPreferences, TVLaced, defaults);
		OUTPUT_INT(gGlobalPreferences, TVType, defaults);
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

SGlobalPreferences::SGlobalPreferences()
	: DisplayFramerate(0)
#ifdef DAEDALUS_DEBUG_DISPLAYLIST
	  ,
	  HighlightInexactBlendModes(false),
	  CustomBlendModes(true)
#endif
	  ,
	  BatteryWarning(false),
	  LargeROMBuffer(true),
	  ForceLinearFilter(false),
	  RumblePak(false),
	  GuiColor(BLACK),
	  StickMinDeadzone(0.28f),
	  StickMaxDeadzone(1.0f),
	  ViewportType(VT_FULLSCREEN),
	  TVEnable(false),
	  TVLaced(false),
	  TVType(TT_4_3)
{
}

void SGlobalPreferences::Apply() const {}

void SRomPreferences::Reset()
{
	PatchesEnabled = true;
	DynarecEnabled = true;
	DynarecLoopOptimisation = false;
	DynarecDoublesOptimisation = false;
	DoubleDisplayEnabled = true;
	CleanSceneEnabled = false;
	ClearDepthFrameBuffer = false;
	AudioRateMatch = false;
	VideoRateMatch = false;
	FogEnabled = false;
	MemoryAccessOptimisation = false;
	CheatsEnabled = false;
	CheckTextureHashFrequency = THF_EVERY_FRAME;
	Frameskip = FV_DISABLED;
	AudioEnabled = APM_ENABLED_SYNC;
	ZoomX = 1.0f;
	SpeedSyncEnabled = 0;
}

void SRomPreferences::Apply() const
{
	gOSHooksEnabled = PatchesEnabled;
	gSpeedSyncEnabled = SpeedSyncEnabled;
	gDynarecEnabled = g_ROM.settings.DynarecSupported && DynarecEnabled;
	gDynarecLoopOptimisation = DynarecLoopOptimisation;  // && g_ROM.settings.DynarecLoopOptimisation;
	gDynarecDoublesOptimisation = g_ROM.settings.DynarecDoublesOptimisation || DynarecDoublesOptimisation;
	gDoubleDisplayEnabled =
		g_ROM.settings.DoubleDisplayEnabled && DoubleDisplayEnabled;  // I don't know why DD won't disabled if we set ||
	gCleanSceneEnabled = g_ROM.settings.CleanSceneEnabled || CleanSceneEnabled;
	gClearDepthFrameBuffer = g_ROM.settings.ClearDepthFrameBuffer || ClearDepthFrameBuffer;
	gAudioRateMatch = g_ROM.settings.AudioRateMatch || AudioRateMatch;
	gFogEnabled = g_ROM.settings.FogEnabled || FogEnabled;
	gCheckTextureHashFrequency = GetTexureHashFrequencyAsFrames(CheckTextureHashFrequency);
	gMemoryAccessOptimisation = g_ROM.settings.MemoryAccessOptimisation || MemoryAccessOptimisation;
	gFrameskipValue = Frameskip;
	gZoomX = ZoomX;
	gCheatsEnabled = g_ROM.settings.CheatsEnabled || CheatsEnabled;
	gAudioPluginEnabled = AudioEnabled;
}

static const u32 gTextureHashFreqeuncies[] = {
	0,   // THF_DISABLED = 0,
	1,   // THF_EVERY_FRAME,
	2,   // THF_EVERY_2,
	4,   // THF_EVERY_4,
	8,   // THF_EVERY_8,
	16,  // THF_EVERY_16,
	32,  // THF_EVERY_32,
};

static const char* const gTextureHashFreqeuncyDescriptions[] = {
	"Disabled",			// THF_DISABLED = 0,
	"Every Frame",		// THF_EVERY_FRAME,
	"Every 2 Frames",   // THF_EVERY_2,
	"Every 4 Frames",   // THF_EVERY_4,
	"Every 8 Frames",   // THF_EVERY_8,
	"Every 16 Frames",  // THF_EVERY_16,
	"Every 32 Frames",  // THF_EVERY_32,
};

static u32 GetTexureHashFrequencyAsFrames(ETextureHashFrequency thf)
{
	if (thf >= 0 && thf < NUM_THF)
	{
		return gTextureHashFreqeuncies[thf];
	}

	return 0;
}

static ETextureHashFrequency GetTextureHashFrequencyFromFrames(u32 frames)
{
	for (u32 i = 0; i < NUM_THF; ++i)
	{
		if (frames <= gTextureHashFreqeuncies[i])
		{
			return ETextureHashFrequency(i);
		}
	}

	return THF_EVERY_32;  // Return the maximum
}

const char* Preferences_GetTextureHashFrequencyDescription(ETextureHashFrequency thf)
{
	if (thf >= 0 && thf < NUM_THF)
	{
		return gTextureHashFreqeuncyDescriptions[thf];
	}

	return "?";
}

u32 GetFrameskipValueAsInt(EFrameskipValue value) { return value; }

EFrameskipValue GetFrameskipValueFromInt(u32 value)
{
	//  This is always False
	//	if( value < FV_DISABLED )
	//		return FV_DISABLED;

	//	if( value > FV_10 )
	//		return FV_10;

	return EFrameskipValue(value);
}

const char* Preferences_GetFrameskipDescription(EFrameskipValue value)
{
	switch (value)
	{
		case FV_DISABLED:
			return "Disabled";
		case FV_AUTO1:
			return "Auto 1";
		case FV_AUTO2:
			return "Auto 2";
		case FV_1:
			return "1";
		case FV_2:
			return "2";
		case FV_3:
			return "3";
		case FV_4:
			return "4";
		case FV_5:
			return "5";
		case FV_6:
			return "6";
		case FV_7:
			return "7";
		case FV_8:
			return "8";
		case FV_9:
			return "9";
#ifdef DAEDALUS_DEBUG_DISPLAYLIST
		case FV_99:
			return "99";
#endif
		case NUM_FRAMESKIP_VALUES:
			break;
	}
	DAEDALUS_ERROR("Unhandled frameskip value");
	return "?";
}
