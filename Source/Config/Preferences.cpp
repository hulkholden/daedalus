/*
Copyright (C) 2007 StrmnNrmn

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
#include "Config/Preferences.h"

SGlobalPreferences gGlobalPreferences;

SGlobalPreferences::SGlobalPreferences()
	: ForceLinearFilter(false),
	  RumblePak(false)
{
}

void SGlobalPreferences::Apply() const {}

void SRomPreferences::Reset()
{
	PatchesEnabled = true;
	DynarecEnabled = true;
	DoubleDisplayEnabled = true;
	CleanSceneEnabled = false;
	ClearDepthFrameBuffer = false;
	FogEnabled = false;
	MemoryAccessOptimisation = false;
	CheatsEnabled = false;
	AudioEnabled = AM_ENABLED_SYNC;
	SpeedSyncEnabled = 0;
}

void SRomPreferences::Apply(const RomSettings& rom_settings) const
{
	gOSHooksEnabled = PatchesEnabled;
	gSpeedSyncEnabled = SpeedSyncEnabled;
	gDynarecEnabled = rom_settings.DynarecSupported && DynarecEnabled;
	gDoubleDisplayEnabled =
		rom_settings.DoubleDisplayEnabled && DoubleDisplayEnabled;  // I don't know why DD won't disabled if we set ||
	gCleanSceneEnabled = rom_settings.CleanSceneEnabled || CleanSceneEnabled;
	gClearDepthFrameBuffer = rom_settings.ClearDepthFrameBuffer || ClearDepthFrameBuffer;
	gFogEnabled = rom_settings.FogEnabled || FogEnabled;
	gCheatsEnabled = rom_settings.CheatsEnabled || CheatsEnabled;
	gAudioPluginMode = AudioEnabled;
}
