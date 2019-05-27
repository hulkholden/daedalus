/*
Copyright (C) 2003 Azimer
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

//
//	N.B. This source code is derived from Azimer's Audio plugin (v0.55?)
//	and modified by StrmnNrmn to work with Daedalus PSP. Thanks Azimer!
//	Drop me a line if you get chance :)
//

#include "stdafx.h"

#include <string.h>

#include "audiohle.h"
#include "AudioHLEProcessor.h"

#include "Debug/DBGConsole.h"

#include "Math/MathUtil.h"
#include "Utility/FastMemcpy.h"

inline s32		FixedPointMul16( s32 a, s32 b )
{
	return s32( ( a * b ) >> 16 );
}

static void SPNOOP( AudioHLECommand command )
{
	DBGConsole_Msg( 0, "AudioHLE: Unknown/Unimplemented Audio Command %i in ABI 3", command.cmd );
}

static void SETVOL3( AudioHLECommand command )
{
	u8 Flags {(u8)(command.cmd0 >> 0x10)};
	if (Flags & 0x4)
	{ // 288
		if (Flags & 0x2)
		{ // 290
			gAudioHLEState.VolLeft  = (s16)command.cmd0; // 0x50
			gAudioHLEState.EnvDry	= (s16)(command.cmd1 >> 16); // 0x4E
			gAudioHLEState.EnvWet	= (s16)command.cmd1; // 0x4C
		}
		else
		{
			gAudioHLEState.VolTrgRight  = (s16)command.cmd0; // 0x46
			//gAudioHLEState.VolRampRight = (u16)(command.cmd1 >> 16) | (s32)(s16)(command.cmd1 << 0x10);
			gAudioHLEState.VolRampRight = command.cmd1; // 0x48/0x4A
		}
	}
	else
	{
		gAudioHLEState.VolTrgLeft  = (s16)command.cmd0; // 0x40
		gAudioHLEState.VolRampLeft = command.cmd1; // 0x42/0x44
	}
}

static void ENVMIXER3( AudioHLECommand command )
{
	u8 flags {(u8)((command.cmd0 >> 16) & 0xff)};
	u32 addy {(command.cmd1 & 0xFFFFFF)};

 	s16 *inp {(s16 *)(gAudioHLEState.Buffer+0x4F0)};
	s16 *out {(s16 *)(gAudioHLEState.Buffer+0x9D0)};
	s16 *aux1 {(s16 *)(gAudioHLEState.Buffer+0xB40)};
	s16 *aux2 {(s16 *)(gAudioHLEState.Buffer+0xCB0)};
	s16 *aux3 {(s16 *)(gAudioHLEState.Buffer+0xE20)};

	s32 MainR {},MainL {},AuxR {}, AuxL {};
	s32 i1 {}, o1 {}, a1 {}, a2 {}, a3 {};
	s32 LAdder {}, LAcc {}, LVol {}, RAdder {}, RAcc {}, RVol {};
	s16 RSig {}, LSig {}, Wet {}, Dry {}, LTrg {}, RTrg {};

	gAudioHLEState.VolRight = (s16)command.cmd0;

	s16* buff {(s16*)(rdram+addy)};

	if (flags & A_INIT)
	{
		LAdder = gAudioHLEState.VolRampLeft / 8;
		LAcc  = 0;
		LVol  = gAudioHLEState.VolLeft;
		LSig = (s16)(gAudioHLEState.VolRampLeft >> 16);

		RAdder = gAudioHLEState.VolRampRight / 8;
		RAcc  = 0;
		RVol  = gAudioHLEState.VolRight;
		RSig = (s16)(gAudioHLEState.VolRampRight >> 16);

		Wet = gAudioHLEState.EnvWet;
		Dry = gAudioHLEState.EnvDry; // Save Wet/Dry values
		LTrg = gAudioHLEState.VolTrgLeft; RTrg = gAudioHLEState.VolTrgRight; // Save Current Left/Right Targets
	}
	else
	{
		Wet    = *(s16 *)(buff +  0); // 0-1
		Dry    = *(s16 *)(buff +  2); // 2-3
		LTrg   = *(s16 *)(buff +  4); // 4-5
		RTrg   = *(s16 *)(buff +  6); // 6-7
		LAdder = *(s32 *)(buff +  8); // 8-9 (buff is a 16bit pointer)
		RAdder = *(s32 *)(buff + 10); // 10-11
		LAcc   = *(s32 *)(buff + 12); // 12-13
		RAcc   = *(s32 *)(buff + 14); // 14-15
		LVol   = *(s32 *)(buff + 16); // 16-17
		RVol   = *(s32 *)(buff + 18); // 18-19
		LSig   = *(s16 *)(buff + 20); // 20-21
		RSig   = *(s16 *)(buff + 22); // 22-23
		//u32 test  = *(s32 *)(buff + 24); // 22-23
		//if (test != 0x13371337)
		//	__asm int 3;
	}


	//if(!(flags&A_AUX)) {
	//	AuxIncRate=0;
	//	aux2=aux3=zero;
	//}

	for (s32 y {}; y < (0x170/2); y++) {

		// Left
		LAcc += LAdder;
		LVol += (LAcc >> 16);
		LAcc &= 0xFFFF;

		// Right
		RAcc += RAdder;
		RVol += (RAcc >> 16);
		RAcc &= 0xFFFF;
// ****************************************************************
		// Clamp Left
		if (LSig >= 0) { // VLT
			if (LVol > LTrg) {
				LVol = LTrg;
			}
		} else { // VGE
			if (LVol < LTrg) {
				LVol = LTrg;
			}
		}

		// Clamp Right
		if (RSig >= 0) { // VLT
			if (RVol > RTrg) {
				RVol = RTrg;
			}
		} else { // VGE
			if (RVol < RTrg) {
				RVol = RTrg;
			}
		}
// ****************************************************************
		MainL = ((Dry * LVol) + 0x4000) >> 15;
		MainR = ((Dry * RVol) + 0x4000) >> 15;

		o1 = out [y^1];
		a1 = aux1[y^1];
		i1 = inp [y^1];

		o1+=((i1*MainL)+0x4000)>>15;
		a1+=((i1*MainR)+0x4000)>>15;

// ****************************************************************

		o1 = Saturate<s16>( o1 );
		a1 = Saturate<s16>( a1 );

// ****************************************************************

		out[y^1]=o1;
		aux1[y^1]=a1;

// ****************************************************************
		//if (!(flags&A_AUX)) {
			a2 = aux2[y^1];
			a3 = aux3[y^1];

			AuxL  = ((Wet * LVol) + 0x4000) >> 15;
			AuxR  = ((Wet * RVol) + 0x4000) >> 15;

			a2+=((i1*AuxL)+0x4000)>>15;
			a3+=((i1*AuxR)+0x4000)>>15;

			a2 = Saturate<s16>( a2 );
			a3 = Saturate<s16>( a3 );

			aux2[y^1]=a2;
			aux3[y^1]=a3;
		}
	//}

	*(s16 *)(buff +  0) = Wet; // 0-1
	*(s16 *)(buff +  2) = Dry; // 2-3
	*(s16 *)(buff +  4) = LTrg; // 4-5
	*(s16 *)(buff +  6) = RTrg; // 6-7
	*(s32 *)(buff +  8) = LAdder; // 8-9 (buff is a 16bit pointer)
	*(s32 *)(buff + 10) = RAdder; // 10-11
	*(s32 *)(buff + 12) = LAcc; // 12-13
	*(s32 *)(buff + 14) = RAcc; // 14-15
	*(s32 *)(buff + 16) = LVol; // 16-17
	*(s32 *)(buff + 18) = RVol; // 18-19
	*(s16 *)(buff + 20) = LSig; // 20-21
	*(s16 *)(buff + 22) = RSig; // 22-23
	//*(u32 *)(buff + 24) = 0x13371337; // 22-23
}

static void CLEARBUFF3( AudioHLECommand command )
{
	u16 addr = (u16)(command.cmd0 &  0xffff);
	u16 count = (u16)(command.cmd1 & 0xffff);
	memset(gAudioHLEState.Buffer+addr+0x4f0, 0, count);
}

static void MIXER3( AudioHLECommand command )
{
	// Needs accuracy verification...
	u16 dmemin  = (u16)(command.cmd1 >> 0x10)  + 0x4f0;
	u16 dmemout = (u16)(command.cmd1 & 0xFFFF) + 0x4f0;
	//u8  flags   = (u8)((command.cmd0 >> 16) & 0xff);
	s32 gain    = (s16)(command.cmd0 & 0xFFFF);

	gAudioHLEState.Mixer( dmemout, dmemin, gain, 0x170 );		// NB - did mult gain by 2 above, then shifted by 16 inside mixer.
}

static void LOADBUFF3( AudioHLECommand command )
{
	u32 v0 {};
	u32 cnt {(((command.cmd0 >> 0xC)+3)&0xFFC)};
	v0 = (command.cmd1 & 0xfffffc);
	u32 src {(command.cmd0&0xffc)+0x4f0};
	memcpy (gAudioHLEState.Buffer+src, rdram+v0, cnt);

}

static void SAVEBUFF3( AudioHLECommand command )
{
	u32 v0 {};
	u32 cnt {(((command.cmd0 >> 0xC)+3)&0xFFC)};
	v0 = (command.cmd1 & 0xfffffc);
	u32 src {(command.cmd0&0xffc)+0x4f0};

	memcpy (rdram+v0, gAudioHLEState.Buffer+src, cnt);

}

// Loads an ADPCM table - Works 100% Now 03-13-01
static void LOADADPCM3( AudioHLECommand command )
{
	u32		address(command.Abi3LoadADPCM.Address );// + gAudioHLEState.Segments[(command.cmd1>>24)&0xf];
	u16		count( command.Abi3LoadADPCM.Count );

	gAudioHLEState.LoadADPCM( address, count );
}

// Needs accuracy verification...
static void DMEMMOVE3( AudioHLECommand command )
{
	u16 src( command.Abi3DmemMove.Src );
	u16 dst( command.Abi3DmemMove.Dst );
	u16 count( command.Abi3DmemMove.Count );

	gAudioHLEState.DmemMove( dst + 0x4f0, src + 0x4f0, count );
}

static void SETLOOP3( AudioHLECommand command )
{
	u32	loopval( command.Abi3SetLoop.LoopVal );// + gAudioHLEState.Segments[(command.cmd1>>24)&0xf];

	gAudioHLEState.SetLoop( loopval );
}

// Verified to be 100% Accurate...
static void ADPCM3( AudioHLECommand command )
{
	u8 Flags {(u8)((command.cmd1>>0x1c)&0xff)};
	//u16 Gain=(u16)(command.cmd0&0xffff);
	u32 Address {(command.cmd0 & 0xffffff)};// + gAudioHLEState.Segments[(command.cmd1>>24)&0xf];
	u32 inPtr {(command.cmd1>>12)&0xf};
	//s16 *out=(s16 *)(testbuff+(gAudioHLEState.OutBuffer>>2));
	s16 *out {(s16 *)(gAudioHLEState.Buffer+(command.cmd1&0xfff)+0x4f0)};
	//u8 *in=(u8 *)(gAudioHLEState.Buffer+((command.cmd1>>12)&0xf)+0x4f0);
	s16 count {(s16)((command.cmd1 >> 16)&0xfff)};
	u8 icode {}, code {};
	s32 vscale {};
	u16 index {}, j {};
	s32 a[8] {};
	s16 *book1 {},*book2 {};

	memset(out,0,32);

	if(!(Flags&0x1))
	{
			memcpy(out,&rdram[(Flags&0x2) ? gAudioHLEState.LoopVal : Address],32);
	}

	s32 l1 {out[15]};
	s32 l2 {out[14]};
	s32 inp1[8] {};
	s32 inp2[8] {};
	out+=16;
	while(count>0)
	{
													// the first interation through, these values are
													// either 0 in the case of A_INIT, from a special
													// area of memory in the case of A_LOOP or just
													// the values we calculated the last time

		code = gAudioHLEState.Buffer[(0x4f0+inPtr)^3];
		index =  code&0xf;
		index <<= 4;									// index into the adpcm code table
		book1 = (s16 *)&gAudioHLEState.ADPCMTable[index];
		book2 = book1+8;
		code >>= 4;									// upper nibble is scale
		vscale = (0x8000>>((12-code)-1));				// very strange. 0x8000 would be .5 in 16:16 format
													// so this appears to be a fractional scale based
													// on the 12 based inverse of the scale value.  note
													// that this could be negative, in which case we do
													// not use the calculated vscale value... see the
													// if(code>12) check below

		inPtr++;									// coded adpcm data lies next
		j = 0;
		while(j<8)									// loop of 8, for 8 coded nibbles from 4 bytes
													// which yields 8 s16 pcm values
		{
			icode = gAudioHLEState.Buffer[(0x4f0+inPtr)^3];
			inPtr++;

			inp1[j]=(s16)((icode&0xf0)<<8);			// this will in effect be signed

			// Conker and Banjo set this!
			if ( code < 12 )
				inp1[j] = ((s32)((s32)inp1[j]*(s32)vscale)>>16);
			j++;

			inp1[j] = (s16)((icode&0xf) << 12);

			inp1[j] = ((s32)((s32)inp1[j]*(s32)vscale) >> 16);
			j++;
		}

		j = 0;
		while(j < 8)
		{
			icode=gAudioHLEState.Buffer[(0x4f0+inPtr)^3];
			inPtr++;

			inp2[j]=(s16)((icode&0xf0)<<8);			// this will in effect be signed

			if(code<12)
				inp2[j]=((s32)((s32)inp2[j]*(s32)vscale)>>16);

			j++;

			inp2[j]=(s16)((icode&0xf)<<12);

			inp2[j]=((s32)((s32)inp2[j]*(s32)vscale)>>16);
			j++;
		}

		a[0]= (s32)book1[0]*(s32)l1;
		a[0]+=(s32)book2[0]*(s32)l2;
		a[0]+=(s32)inp1[0]*(s32)2048;

		a[1] =(s32)book1[1]*(s32)l1;
		a[1]+=(s32)book2[1]*(s32)l2;
		a[1]+=(s32)book2[0]*inp1[0];
		a[1]+=(s32)inp1[1]*(s32)2048;

		a[2] =(s32)book1[2]*(s32)l1;
		a[2]+=(s32)book2[2]*(s32)l2;
		a[2]+=(s32)book2[1]*inp1[0];
		a[2]+=(s32)book2[0]*inp1[1];
		a[2]+=(s32)inp1[2]*(s32)2048;

		a[3] =(s32)book1[3]*(s32)l1;
		a[3]+=(s32)book2[3]*(s32)l2;
		a[3]+=(s32)book2[2]*inp1[0];
		a[3]+=(s32)book2[1]*inp1[1];
		a[3]+=(s32)book2[0]*inp1[2];
		a[3]+=(s32)inp1[3]*(s32)2048;

		a[4] =(s32)book1[4]*(s32)l1;
		a[4]+=(s32)book2[4]*(s32)l2;
		a[4]+=(s32)book2[3]*inp1[0];
		a[4]+=(s32)book2[2]*inp1[1];
		a[4]+=(s32)book2[1]*inp1[2];
		a[4]+=(s32)book2[0]*inp1[3];
		a[4]+=(s32)inp1[4]*(s32)2048;

		a[5] =(s32)book1[5]*(s32)l1;
		a[5]+=(s32)book2[5]*(s32)l2;
		a[5]+=(s32)book2[4]*inp1[0];
		a[5]+=(s32)book2[3]*inp1[1];
		a[5]+=(s32)book2[2]*inp1[2];
		a[5]+=(s32)book2[1]*inp1[3];
		a[5]+=(s32)book2[0]*inp1[4];
		a[5]+=(s32)inp1[5]*(s32)2048;

		a[6] =(s32)book1[6]*(s32)l1;
		a[6]+=(s32)book2[6]*(s32)l2;
		a[6]+=(s32)book2[5]*inp1[0];
		a[6]+=(s32)book2[4]*inp1[1];
		a[6]+=(s32)book2[3]*inp1[2];
		a[6]+=(s32)book2[2]*inp1[3];
		a[6]+=(s32)book2[1]*inp1[4];
		a[6]+=(s32)book2[0]*inp1[5];
		a[6]+=(s32)inp1[6]*(s32)2048;

		a[7] =(s32)book1[7]*(s32)l1;
		a[7]+=(s32)book2[7]*(s32)l2;
		a[7]+=(s32)book2[6]*inp1[0];
		a[7]+=(s32)book2[5]*inp1[1];
		a[7]+=(s32)book2[4]*inp1[2];
		a[7]+=(s32)book2[3]*inp1[3];
		a[7]+=(s32)book2[2]*inp1[4];
		a[7]+=(s32)book2[1]*inp1[5];
		a[7]+=(s32)book2[0]*inp1[6];
		a[7]+=(s32)inp1[7]*(s32)2048;

		*(out++) =      Saturate<s16>( a[1] >> 11 );
		*(out++) =      Saturate<s16>( a[0] >> 11 );
		*(out++) =      Saturate<s16>( a[3] >> 11 );
		*(out++) =      Saturate<s16>( a[2] >> 11 );
		*(out++) =      Saturate<s16>( a[5] >> 11 );
		*(out++) =      Saturate<s16>( a[4] >> 11 );
		*(out++) = l2 = Saturate<s16>( a[7] >> 11 );
		*(out++) = l1 = Saturate<s16>( a[6] >> 11 );

		a[0]= (s32)book1[0]*(s32)l1;
		a[0]+=(s32)book2[0]*(s32)l2;
		a[0]+=(s32)inp2[0]*(s32)2048;

		a[1] =(s32)book1[1]*(s32)l1;
		a[1]+=(s32)book2[1]*(s32)l2;
		a[1]+=(s32)book2[0]*inp2[0];
		a[1]+=(s32)inp2[1]*(s32)2048;

		a[2] =(s32)book1[2]*(s32)l1;
		a[2]+=(s32)book2[2]*(s32)l2;
		a[2]+=(s32)book2[1]*inp2[0];
		a[2]+=(s32)book2[0]*inp2[1];
		a[2]+=(s32)inp2[2]*(s32)2048;

		a[3] =(s32)book1[3]*(s32)l1;
		a[3]+=(s32)book2[3]*(s32)l2;
		a[3]+=(s32)book2[2]*inp2[0];
		a[3]+=(s32)book2[1]*inp2[1];
		a[3]+=(s32)book2[0]*inp2[2];
		a[3]+=(s32)inp2[3]*(s32)2048;

		a[4] =(s32)book1[4]*(s32)l1;
		a[4]+=(s32)book2[4]*(s32)l2;
		a[4]+=(s32)book2[3]*inp2[0];
		a[4]+=(s32)book2[2]*inp2[1];
		a[4]+=(s32)book2[1]*inp2[2];
		a[4]+=(s32)book2[0]*inp2[3];
		a[4]+=(s32)inp2[4]*(s32)2048;

		a[5] =(s32)book1[5]*(s32)l1;
		a[5]+=(s32)book2[5]*(s32)l2;
		a[5]+=(s32)book2[4]*inp2[0];
		a[5]+=(s32)book2[3]*inp2[1];
		a[5]+=(s32)book2[2]*inp2[2];
		a[5]+=(s32)book2[1]*inp2[3];
		a[5]+=(s32)book2[0]*inp2[4];
		a[5]+=(s32)inp2[5]*(s32)2048;

		a[6] =(s32)book1[6]*(s32)l1;
		a[6]+=(s32)book2[6]*(s32)l2;
		a[6]+=(s32)book2[5]*inp2[0];
		a[6]+=(s32)book2[4]*inp2[1];
		a[6]+=(s32)book2[3]*inp2[2];
		a[6]+=(s32)book2[2]*inp2[3];
		a[6]+=(s32)book2[1]*inp2[4];
		a[6]+=(s32)book2[0]*inp2[5];
		a[6]+=(s32)inp2[6]*(s32)2048;

		a[7] =(s32)book1[7]*(s32)l1;
		a[7]+=(s32)book2[7]*(s32)l2;
		a[7]+=(s32)book2[6]*inp2[0];
		a[7]+=(s32)book2[5]*inp2[1];
		a[7]+=(s32)book2[4]*inp2[2];
		a[7]+=(s32)book2[3]*inp2[3];
		a[7]+=(s32)book2[2]*inp2[4];
		a[7]+=(s32)book2[1]*inp2[5];
		a[7]+=(s32)book2[0]*inp2[6];
		a[7]+=(s32)inp2[7]*(s32)2048;

		*(out++) =      Saturate<s16>( a[1] >> 11 );
		*(out++) =      Saturate<s16>( a[0] >> 11 );
		*(out++) =      Saturate<s16>( a[3] >> 11 );
		*(out++) =      Saturate<s16>( a[2] >> 11 );
		*(out++) =      Saturate<s16>( a[5] >> 11 );
		*(out++) =      Saturate<s16>( a[4] >> 11 );
		*(out++) = l2 = Saturate<s16>( a[7] >> 11 );
		*(out++) = l1 = Saturate<s16>( a[6] >> 11 );

		count-=32;
	}
	out-=16;

	memcpy(&rdram[Address],out,32);
}

static void RESAMPLE3( AudioHLECommand command )
{
	u8 Flags {(u8)((command.cmd1>>0x1e))};
	u32 Pitch {((command.cmd1>>0xe)&0xffff) << 1};
	u32 addy {(command.cmd0 & 0xffffff)};
	u32 Accum {};
	s16 *dst {};
	s16 *src {};
	dst = (s16 *)(gAudioHLEState.Buffer);
	src = (s16 *)(gAudioHLEState.Buffer);
	u32 srcPtr{ ((((command.cmd1>>2)&0xfff)+0x4f0)/2)};
	u32 dstPtr {};//=(gAudioHLEState.OutBuffer/2);

	srcPtr -= 1;

	if (command.cmd1 & 0x3) {
		dstPtr = 0x660/2;
	} else {
		dstPtr = 0x4f0/2;
	}

	if ((Flags & 0x1) == 0) {
		src[srcPtr^1] = ((u16 *)rdram)[((addy/2))^1];
		Accum = *(u16 *)(rdram+addy+10);
	} else {
		src[(srcPtr)^1] = 0;
		Accum = 0;
	}

	for(u32 i {} ; i < 0x170/2;i++)
	{
		dst[dstPtr^1] = src[srcPtr^1] + FixedPointMul16( src[(srcPtr+1)^1] - src[srcPtr^1], Accum );
		++dstPtr;
		Accum += Pitch;
		srcPtr += (Accum>>16);
		Accum &= 0xFFFF;
	}

	((u16 *)rdram)[((addy/2))^1] = src[srcPtr^1];
	*(u16 *)(rdram+addy+10) = u16( Accum );
}

static void INTERLEAVE3( AudioHLECommand command )
{
	// Needs accuracy verification...
	//inR = command.cmd1 & 0xFFFF;
	//inL = (command.cmd1 >> 16) & 0xFFFF;

	gAudioHLEState.Interleave( 0x4f0, 0x9d0, 0xb40, 0x170 );
}

/*
typedef struct {
	u8 sync;

	u8 error_protection	: 1;	//  0=yes, 1=no
	u8 lay				: 2;	// 4-lay = layerI, II or III
	u8 version			: 1;	// 3=mpeg 1.0, 2=mpeg 2.5 0=mpeg 2.0
	u8 sync2			: 4;

	u8 extension		: 1;    // Unknown
	u8 padding			: 1;    // padding
	u8 sampling_freq	: 2;	// see table below
	u8 bitrate_index	: 4;	//     see table below

	u8 emphasis			: 2;	//see table below
	u8 original			: 1;	// 0=no 1=yes
	u8 copyright		: 1;	// 0=no 1=yes
	u8 mode_ext			: 2;    // used with "joint stereo" mode
	u8 mode				: 2;    // Channel Mode
} mp3struct;

mp3struct mp3;
FILE *mp3dat;
*/

static void WHATISTHIS( AudioHLECommand command )
{
}

//static FILE *fp = fopen ("d:\\mp3info.txt", "wt");
//u32 setaddr;
static void MP3ADDY( AudioHLECommand command )
{
	//setaddr = (command.cmd1 & 0xffffff);
}


void MP3( AudioHLECommand command );


/*

FFT = Fast Fourier Transform
DCT = Discrete Cosine Transform
MPEG-1 Layer 3 retains Layer 2�s 1152-sample window, as well as the FFT polyphase filter for
backward compatibility, but adds a modified DCT filter. DCT�s advantages over DFTs (discrete
Fourier transforms) include half as many multiply-accumulate operations and half the
generated coefficients because the sinusoidal portion of the calculation is absent, and DCT
generally involves simpler math. The finite lengths of a conventional DCTs� bandpass impulse
responses, however, may result in block-boundary effects. MDCTs overlap the analysis blocks
and lowpass-filter the decoded audio to remove aliases, eliminating these effects. MDCTs also
have a higher transform coding gain than the standard DCT, and their basic functions
correspond to better bandpass response.

MPEG-1 Layer 3�s DCT sub-bands are unequally sized, and correspond to the human auditory
system�s critical bands. In Layer 3 decoders must support both constant- and variable-bit-rate
bit streams. (However, many Layer 1 and 2 decoders also handle variable bit rates). Finally,
Layer 3 encoders Huffman-code the quantized coefficients before archiving or transmission for
additional lossless compression. Bit streams range from 32 to 320 kbps, and 128-kbps rates
achieve near-CD quality, an important specification to enable dual-channel ISDN
(integrated-services-digital-network) to be the future high-bandwidth pipe to the home.

*/
static void DISABLE( AudioHLECommand command )
{
	//MessageBox (NULL, "Help", "ABI 3 Command 0", MB_OK);
	//ChangeABI (5);
}


AudioHLEInstruction ABI3[0x20] =
{
    DISABLE , ADPCM3 , CLEARBUFF3,	ENVMIXER3  , LOADBUFF3, RESAMPLE3  , SAVEBUFF3, MP3,
	MP3ADDY, SETVOL3, DMEMMOVE3 , LOADADPCM3 , MIXER3   , INTERLEAVE3, WHATISTHIS   , SETLOOP3,
    SPNOOP , SPNOOP, SPNOOP   , SPNOOP    , SPNOOP  , SPNOOP    , SPNOOP  , SPNOOP,
    SPNOOP , SPNOOP, SPNOOP   , SPNOOP    , SPNOOP  , SPNOOP    , SPNOOP  , SPNOOP
};
