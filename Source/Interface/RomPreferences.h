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

#pragma once

#ifndef INTERFACE_ROMPREFERENCES_H_
#define INTERFACE_ROMPREFERENCES_H_

#include "Config/ConfigOptions.h"

struct SRomPreferences
{
	bool PatchesEnabled;
	bool DynarecEnabled;  // Requires DynarceSupported in RomSettings
	bool DynarecLoopOptimisation;
	bool DynarecDoublesOptimisation;
	bool DoubleDisplayEnabled;
	bool CleanSceneEnabled;
	bool ClearDepthFrameBuffer;
	bool AudioRateMatch;
	bool VideoRateMatch;
	bool FogEnabled;
	bool MemoryAccessOptimisation;
	bool CheatsEnabled;
	EAudioPluginMode AudioEnabled;
	u32 SpeedSyncEnabled;
	u32 ControllerIndex;

	SRomPreferences() { Reset(); }

	void Reset();
	void Apply() const;
};

#endif  // INTERFACE_ROMPREFERENCES_H_
