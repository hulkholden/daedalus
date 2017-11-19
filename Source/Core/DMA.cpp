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

// Various stuff to map an address onto the correct memory region

#include "Base/Daedalus.h"
#include "Core/DMA.h"

#include "Core/CPU.h"
#include "Core/Interrupt.h"
#include "Core/Memory.h"
#include "Core/PIF.h"
#include "Core/ROM.h"
#include "Core/ROMBuffer.h"
#include "Core/RSP_HLE.h"
#include "Core/Save.h"
#include "Debug/Console.h"
#include "Debug/DebugLog.h"
#include "OSHLE/OSHLE.h"
#include "OSHLE/OSTask.h"
#include "Utility/FastMemcpy.h"

bool gDMAUsed = false;
//*****************************************************************************
//
//*****************************************************************************
void DMA_SP_CopyFromRDRAM()
{
	u32 spmem_address_reg = Memory_SP_GetRegister(SP_MEM_ADDR_REG);
	u32 rdram_address_reg = Memory_SP_GetRegister(SP_DRAM_ADDR_REG);
	u32 rdlen_reg         = Memory_SP_GetRegister(SP_RD_LEN_REG);

	u32 rdram_address = (rdram_address_reg&0x00FFFFFF)	& ~7;	// Align to 8 byte boundary
	u32 spmem_address = (spmem_address_reg&0x1FFF)		& ~7;	// Align to 8 byte boundary
	u32 length = ((rdlen_reg    &0x0FFF) | 7)+1;					// Round up to 8 bytes
	u32 count  = ((rdlen_reg>>12)&0x00FF)+1;
	u32 skip   = ((rdlen_reg>>20)&0x0FFF);

	for (u32 c = 0; c < count; c++ )
	{
		// Conker needs this
		if ( rdram_address  > gRamSize )
		{
			//Console_Print("(0x%08x) (0x%08x)", spmem_address, rdram_address);
			break;
		}
		memcpy_swizzle( &gu8SpMemBase[spmem_address], &gu8RamBase[rdram_address], length );

		rdram_address += length + skip;
		spmem_address += length;
	}

	//Clear the DMA Busy
	Memory_SP_SetRegister(SP_DMA_BUSY_REG, 0);
	Memory_SP_ClrRegisterBits(SP_STATUS_REG, SP_STATUS_DMA_BUSY);
}

//*****************************************************************************
//
//*****************************************************************************
void DMA_SP_CopyToRDRAM()
{
	u32 spmem_address_reg = Memory_SP_GetRegister(SP_MEM_ADDR_REG);
	u32 rdram_address_reg = Memory_SP_GetRegister(SP_DRAM_ADDR_REG);
	u32 wrlen_reg         = Memory_SP_GetRegister(SP_WR_LEN_REG);

	u32 rdram_address = (rdram_address_reg&0x00FFFFFF)	& ~7;	// Align to 8 byte boundary
	u32 spmem_address = (spmem_address_reg&0x1FFF)		& ~7;	// Align to 8 byte boundary
	u32 length = ((wrlen_reg    &0x0FFF) | 7)+1;				// Round up to 8 bytes
	u32 count  = ((wrlen_reg>>12)&0x00FF)+1;
	u32 skip   = ((wrlen_reg>>20)&0x0FFF);

	for ( u32 c = 0; c < count; c++ )
	{
		if ( rdram_address  > gRamSize )
		{
			//Console_Print("(0x%08x) (0x%08x)", spmem_address, rdram_address);
			break;
		}
		memcpy_swizzle( &gu8RamBase[rdram_address], &gu8SpMemBase[spmem_address], length );
		rdram_address += length + skip;
		spmem_address += length;
	}

	//Clear the DMA Busy
	Memory_SP_SetRegister(SP_DMA_BUSY_REG, 0);
	Memory_SP_ClrRegisterBits(SP_STATUS_REG, SP_STATUS_DMA_BUSY);

}

//*****************************************************************************
// Copy 64bytes from DRAM to PIF_RAM
//*****************************************************************************
void DMA_SI_CopyFromDRAM( )
{
	u32 mem = Memory_SI_GetRegister(SI_DRAM_ADDR_REG) & 0x1fffffff;
	u32 * p_dst = (u32 *)gMemBuffers[MEM_PIF_RAM];
	u32 * p_src = (u32 *)(gu8RamBase + mem);

	DPF( DEBUG_MEMORY_PIF, "DRAM (0x%08x) -> PIF Transfer ", mem );

	// Fuse 4 reads and 4 writes to just one which is a lot faster - Corn
	for(u32 i = 0; i < 16; i++)
	{
		p_dst[i] = BSWAP32(p_src[i]);
	}

	Memory_SI_SetRegisterBits(SI_STATUS_REG, SI_STATUS_INTERRUPT);
	Memory_MI_SetRegisterBits(MI_INTR_REG, MI_INTR_SI);
	R4300_Interrupt_UpdateCause3();
}

//*****************************************************************************
// Copy 64bytes to DRAM from PIF_RAM
//*****************************************************************************
void DMA_SI_CopyToDRAM( )
{
	// Check controller status!
	CController::Get()->Process();

	u32 mem = Memory_SI_GetRegister(SI_DRAM_ADDR_REG) & 0x1fffffff;
	u32 * p_src = (u32 *)gMemBuffers[MEM_PIF_RAM];
	u32 * p_dst = (u32 *)(gu8RamBase + mem);

	DPF( DEBUG_MEMORY_PIF, "PIF -> DRAM (0x%08x) Transfer ", mem );

	// Fuse 4 reads and 4 writes to just one which is a lot faster - Corn
	for(u32 i = 0; i < 16; i++)
	{
		p_dst[i] = BSWAP32(p_src[i]);
	}


	Memory_SI_SetRegisterBits(SI_STATUS_REG, SI_STATUS_INTERRUPT);
	Memory_MI_SetRegisterBits(MI_INTR_REG, MI_INTR_SI);

	//Skipping this IRQ fixes allows Body Harvest, Nightmare Creatures and Cruisn' USA to boot
	//ToDo: Implement Delay SI, PJ64 uses that option to make these games boot..
	if (g_ROM.GameHacks != BODY_HARVEST)
		R4300_Interrupt_UpdateCause3();
}

/*
#define PI_DOM2_ADDR1		0x05000000	// to 0x05FFFFFF
#define PI_DOM1_ADDR1		0x06000000	// to 0x07FFFFFF
#define PI_DOM2_ADDR2		0x08000000	// to 0x0FFFFFFF
#define PI_DOM1_ADDR2		0x10000000	// to 0x1FBFFFFF
#define PI_DOM1_ADDR3		0x1FD00000	// to 0x7FFFFFFF
*/

#define IsDom1Addr1( x )		( (x) >= PI_DOM1_ADDR1 && (x) < PI_DOM2_ADDR2 )
#define IsDom1Addr2( x )		( (x) >= PI_DOM1_ADDR2 && (x) < 0x1FBFFFFF )
#define IsDom1Addr3( x )		( (x) >= PI_DOM1_ADDR3 && (x) < 0x7FFFFFFF )
#define IsDom2Addr1( x )		( (x) >= PI_DOM2_ADDR1 && (x) < PI_DOM1_ADDR1 )
#define IsDom2Addr2( x )		( (x) >= PI_DOM2_ADDR2 && (x) < PI_DOM1_ADDR2 )

#define IsFlashDomAddr( x )		( (x) >= PI_DOM2_ADDR2 && (x) < 0x08010000 )

//*****************************************************************************
//
//*****************************************************************************
bool DMA_HandleTransfer( u8 * p_dst, u32 dst_offset, u32 dst_size, const u8 * p_src, u32 src_offset, u32 src_size, u32 length )
{
	if( ( s32( length ) <= 0 ) ||
		(src_offset + length) > src_size ||
		(dst_offset + length) > dst_size )
	{
		return false;
	}

	memcpy_swizzle(&p_dst[dst_offset], &p_src[src_offset], length);
	return true;
}

//*****************************************************************************
//
//*****************************************************************************
static void OnCopiedRom()
{
	if (!gDMAUsed)
	{
		gDMAUsed = true;

#ifdef DAEDALUS_ENABLE_OS_HOOKS
		// Note the rom is only scanned when the ROM jumps to the game boot address
		// ToDO: try to reapply patches - certain roms load in more of the OS after a number of transfers ?
		Patch_ApplyPatches();
#endif

		// Set RDRAM size
		u32 addr = (g_ROM.cic_chip != CIC_6105) ? 0x318 : 0x3F0;
		*(u32 *)(gu8RamBase + addr) = gRamSize;

		// Azimer's DK64 hack, it makes DK64 boot!
		if(g_ROM.GameHacks == DK64)
			*(u32 *)(gu8RamBase + 0x2FE1C0) = 0xAD170014;
	}
}

void DMA_PI_CopyToRDRAM()
{
	u32 mem_address  = Memory_PI_GetRegister(PI_DRAM_ADDR_REG) & 0x00FFFFFF;
	u32 cart_address = Memory_PI_GetRegister(PI_CART_ADDR_REG)  & 0xFFFFFFFF;
	u32 pi_length_reg = (Memory_PI_GetRegister(PI_WR_LEN_REG) & 0xFFFFFFFF) + 1;

	DPF( DEBUG_MEMORY_PI, "PI: Copying %d bytes of data from 0x%08x to 0x%08x", pi_length_reg, cart_address, mem_address );

	//DAEDALUS_ASSERT(!IsDom1Addr1(cart_address), "The code below doesn't handle dom1/addr1 correctly");
	//DAEDALUS_ASSERT(!IsDom1Addr3(cart_address), "The code below doesn't handle dom1/addr3 correctly");

	if (cart_address < 0x10000000)
    {
		if (IsFlashDomAddr(cart_address))
		{
			const u8* p_src    = (const u8*)gMemBuffers[MEM_SAVE];
			u32       src_size = (gMemBufferSizes[MEM_SAVE]);
			cart_address -= PI_DOM2_ADDR2;

			if (g_ROM.settings.SaveType != SAVE_TYPE_FLASH)
				DMA_HandleTransfer( gu8RamBase, mem_address, gRamSize, p_src, cart_address, src_size, pi_length_reg );
			else
				DMA_FLASH_CopyToDRAM(mem_address, cart_address, pi_length_reg);
		}
		else if (IsDom1Addr1(cart_address))
		{
			Console_Print("[YReading from Cart domain 1/addr1] (Ignored)");
		}
		else
		{
			Console_Print("[YUnknown PI Address 0x%08x]", cart_address);
		}
	}
	else
	{
		if (cart_address < 0x1fc00000)
		{
			//Console_Print("[YReading from Cart domain 1/addr2]");
			cart_address -= PI_DOM1_ADDR2;
			CPU_InvalidateICacheRange( 0x80000000 | mem_address, pi_length_reg );
			RomBuffer::CopyToRam( gu8RamBase, mem_address, gRamSize, cart_address, pi_length_reg );

			OnCopiedRom();
		}
		else
		{
			// Paper Mario
			Console_Print("[YReading from Cart domain 1/addr3]");
		}
	}

	Memory_PI_ClrRegisterBits(PI_STATUS_REG, PI_STATUS_DMA_BUSY);
	Memory_MI_SetRegisterBits(MI_INTR_REG, MI_INTR_PI);
	R4300_Interrupt_UpdateCause3();
}

//*****************************************************************************
//
//*****************************************************************************
void DMA_PI_CopyFromRDRAM()
{
	u32 mem_address  = Memory_PI_GetRegister(PI_DRAM_ADDR_REG) & 0xFFFFFFFF;
	u32 cart_address = Memory_PI_GetRegister(PI_CART_ADDR_REG)  & 0xFFFFFFFF;
	u32 pi_length_reg = (Memory_PI_GetRegister(PI_RD_LEN_REG)  & 0xFFFFFFFF) + 1;

	DPF(DEBUG_MEMORY_PI, "PI: Copying %d bytes of data from 0x%08x to 0x%08x", pi_length_reg, mem_address, cart_address );

	/*
	if(pi_length_reg & 0x1)
	{
		Console_Print("PI Copy RDRAM to CART %db from %08X to %08X", pi_length_reg, cart_address|0xA0000000, mem_address);
		Console_Print("Warning, PI DMA, odd length");

		// Tonic Trouble triggers this !

		pi_length_reg ++;
	}
	*/

	// Only care for DOM2/ADDR2
	if(IsFlashDomAddr(cart_address))
	{
		u8* p_dst    = (u8*)gMemBuffers[MEM_SAVE];
		u32 dst_size = gMemBufferSizes[MEM_SAVE];
		cart_address -= PI_DOM2_ADDR2;

		Console_Print("[YWriting to Cart domain 2/addr2 0x%08x]", cart_address);

		if (g_ROM.settings.SaveType != SAVE_TYPE_FLASH)
			DMA_HandleTransfer( p_dst, cart_address, dst_size, gu8RamBase, mem_address, gRamSize, pi_length_reg );
		else
			DMA_FLASH_CopyFromDRAM(mem_address, pi_length_reg);

		Save_MarkSaveDirty();
	}
	else
	{
		Console_Print("[YUnknown PI Address 0x%08x]", cart_address);
	}

	Memory_PI_ClrRegisterBits(PI_STATUS_REG, PI_STATUS_DMA_BUSY);
	Memory_MI_SetRegisterBits(MI_INTR_REG, MI_INTR_PI);
	R4300_Interrupt_UpdateCause3();
}

