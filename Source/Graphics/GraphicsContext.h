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

#ifndef GRAPHICS_GRAPHICSCONTEXT_H_
#define GRAPHICS_GRAPHICSCONTEXT_H_

#include "Base/Types.h"
#include "Base/Singleton.h"

class c32;

// This class basically provides an extra level of security for our
// multithreaded code. Threads can Grab the CGraphicsContext to prevent
// other threads from changing/releasing any of the pointers while it is
// running.

class CGraphicsContext : public CSingleton< CGraphicsContext >
{
public:
	~CGraphicsContext() override;

	bool Initialise();

	bool IsInitialised() const;

	void ClearAllSurfaces();
	void ClearToBlack();
	void ClearZBuffer();
	void ClearColBuffer(const c32 & colour);
	void ClearColBufferAndDepth(const c32 & colour);

	void BeginFrame();
	void EndFrame();
	void UpdateFrame();

	void GetScreenSize(u32 * width, u32 * height) const;
	void ViewportType(u32 * width, u32 * height) const;
};

#endif // GRAPHICS_GRAPHICSCONTEXT_H_
