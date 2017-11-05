/*
Copyright (C) 2012 StrmnNrmn

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

#include "ROMFileMemory.h"
#include "MemoryHeap.h"

#include <stdlib.h>

//*****************************************************************************
//
//*****************************************************************************
CROMFileMemory::~CROMFileMemory()
{
}

//*****************************************************************************
//
//*****************************************************************************
class IROMFileMemory : public CROMFileMemory
{
public:
	virtual void *	Alloc( u32 size );
	virtual void	Free(void * ptr);
};


//*****************************************************************************
//
//*****************************************************************************
template<> bool CSingleton< CROMFileMemory >::Create()
{
	DAEDALUS_ASSERT_Q(mpInstance == NULL);

	mpInstance = new IROMFileMemory();
	return mpInstance != NULL;
}

//*****************************************************************************
//
//*****************************************************************************
void * IROMFileMemory::Alloc( u32 size )
{
	return malloc( size );
}

//*****************************************************************************
//
//*****************************************************************************
void  IROMFileMemory::Free(void * ptr)
{
	free( ptr );
}
