/*
Copyright (C) 2008-2009 Howard Su (howard0su@gmail.com)

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

#ifndef CORE_FLASHMEM_H_
#define CORE_FLASHMEM_H_

#include "Base/Types.h"

// Flash RAM Support
extern u32 FlashStatus[2];
void Flash_DoCommand(u32);
void Flash_Init();

#endif // CORE_FLASHMEM_H_
