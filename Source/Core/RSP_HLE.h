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

#ifndef CORE_RSP_HLE_H_
#define CORE_RSP_HLE_H_

#include "Core/Memory.h"

// Returns true if the rsp is running either LLE or HLE
inline bool RSP_IsRunning(){ return (Memory_SP_GetRegister( SP_STATUS_REG ) & SP_STATUS_HALT) == 0; }

enum EProcessResult
{
	PR_NOT_STARTED,	// Couldn't start
	PR_STARTED,		// Was started asynchronously, active
	PR_COMPLETED,	// Was processed synchronously, completed
};

void RSP_HLE_ProcessTask();

class DisplayListEventHandler
{
  public:
	virtual ~DisplayListEventHandler();
	virtual void OnDisplayListComplete() = 0;
};

void RSP_HLE_RegisterDisplayListEventHandler(DisplayListEventHandler* handler);
void RSP_HLE_UnregisterDisplayListEventHandler(DisplayListEventHandler* handler);


class DisplayListProcessor
{
  public:
	virtual ~DisplayListProcessor();
	virtual void ProcessDisplayList() = 0;
};

void RSP_HLE_RegisterDisplayListProcessor(DisplayListProcessor* processor);
void RSP_HLE_UnregisterDisplayListProcessor(DisplayListProcessor* processor);

#endif // CORE_RSP_HLE_H_
