/*
Copyright (C) 2001,2006 StrmnNrmn

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

#ifndef CORE_R4300INSTRUCTION_H_
#define CORE_R4300INSTRUCTION_H_

#include "Base/Types.h"

#ifdef DAEDALUS_W32
// Calling convention for the R4300 instruction handlers.
// These are called from dynarec so we need to ensure they're __fastcall,
// even if the project is not compiled with that option in the project settings.
#define R4300_CALL_TYPE __fastcall
#else
#define R4300_CALL_TYPE
#endif

// Through various tests, it seems the compiler generates much better code by
// passing the opcode as a primitive datatype (as it can stick the value directly
// in a register rather than passing it via the stack). Unfortunately we have to
// create a temporary to be able to pull the various fields from the opcode, but
// the compiler is clever enough to optimiste this overhead away.

#define R4300_CALL_SIGNATURE	u32	op_code_bits
#define R4300_CALL_ARGUMENTS	op_code_bits

typedef void (R4300_CALL_TYPE *CPU_Instruction )( R4300_CALL_SIGNATURE );


void R4300_CALL_TYPE R4300_Special_ADD( R4300_CALL_SIGNATURE );
void R4300_CALL_TYPE R4300_Special_ADDU( R4300_CALL_SIGNATURE );
void R4300_CALL_TYPE R4300_Special_SUB( R4300_CALL_SIGNATURE );
void R4300_CALL_TYPE R4300_Special_SUBU( R4300_CALL_SIGNATURE );
void R4300_CALL_TYPE R4300_Special_AND( R4300_CALL_SIGNATURE );
void R4300_CALL_TYPE R4300_Special_OR( R4300_CALL_SIGNATURE );
void R4300_CALL_TYPE R4300_Special_XOR( R4300_CALL_SIGNATURE );
void R4300_CALL_TYPE R4300_Special_NOR( R4300_CALL_SIGNATURE );
void R4300_CALL_TYPE R4300_Special_SLT( R4300_CALL_SIGNATURE );
void R4300_CALL_TYPE R4300_Special_SLTU( R4300_CALL_SIGNATURE );

void R4300_CALL_TYPE R4300_Special_DADD( R4300_CALL_SIGNATURE );
void R4300_CALL_TYPE R4300_Special_DADDU( R4300_CALL_SIGNATURE );
void R4300_CALL_TYPE R4300_Special_DSUB( R4300_CALL_SIGNATURE );
void R4300_CALL_TYPE R4300_Special_DSUBU( R4300_CALL_SIGNATURE );

void R4300_CALL_TYPE R4300_Special_DSLL( R4300_CALL_SIGNATURE );
void R4300_CALL_TYPE R4300_Special_DSRL( R4300_CALL_SIGNATURE );
void R4300_CALL_TYPE R4300_Special_DSRA( R4300_CALL_SIGNATURE );
void R4300_CALL_TYPE R4300_Special_DSLL32( R4300_CALL_SIGNATURE );
void R4300_CALL_TYPE R4300_Special_DSRL32( R4300_CALL_SIGNATURE );
void R4300_CALL_TYPE R4300_Special_DSRA32( R4300_CALL_SIGNATURE );


#endif // CORE_R4300INSTRUCTION_H_
