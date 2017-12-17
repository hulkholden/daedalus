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

#include "Base/Daedalus.h"
#include "DLParser.h"

#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"

#include "Base/MathUtil.h"
#include "Config/ConfigOptions.h"
#include "Core/CPU.h"
#include "Core/Memory.h"
#include "Core/ROM.h"
#include "Debug/Console.h"
#include "Debug/Dump.h"
#include "Graphics/GraphicsContext.h"
#include "Graphics/NativePixelFormat.h"
#include "HLEGraphics/BaseRenderer.h"
#include "HLEGraphics/ConvertImage.h"			// Convert555ToRGBA
#include "HLEGraphics/DLDebug.h"
#include "HLEGraphics/HLEGraphics.h"
#include "HLEGraphics/Microcode.h"
#include "HLEGraphics/N64PixelFormat.h"
#include "HLEGraphics/RDP.h"
#include "HLEGraphics/RDPStateManager.h"
#include "HLEGraphics/TextureCache.h"
#include "HLEGraphics/uCodes/Ucode.h"
#include "HLEGraphics/uCodes/UcodeDefs.h"
#include "System/IO.h"
#include "Ultra/ultra_gbi.h"
#include "Ultra/ultra_rcp.h"
#include "Ultra/ultra_sptask.h"
#include "Utility/Profiler.h"


#ifdef DAEDALUS_DEBUG_DISPLAYLIST
#define DL_UNIMPLEMENTED_ERROR( msg )			\
{												\
	static bool shown = false;					\
	if (!shown )								\
	{											\
		DL_PF( "~*Not Implemented %s", msg );	\
		DAEDALUS_DL_ERROR( "%s: %08x %08x", (msg), command.inst.cmd0, command.inst.cmd1 );				\
		shown = true;							\
	}											\
}
#else
#define DL_UNIMPLEMENTED_ERROR( msg )
#endif

static const u32 kMaxDisplayListDepth = 32;

// Mask down to 0x003FFFFF?
#define RDPSegAddr(seg) ( (gSegments[((seg)>>24)&0x0F]&0x00ffffff) + ((seg)&0x00FFFFFF) )

struct N64Viewport
{
    s16 scale_y, scale_x, scale_w, scale_z;
	s16 trans_y, trans_x, trans_w, trans_z;
};

struct N64mat
{
	struct _s16
	{
		s16 y, x, w, z;
	};

	struct _u16
	{
		u16 y, x, w, z;
	};

	_s16 h[4];
	_u16 l[4];
};

struct N64Light
{
	u8 ca, b, g, r;	// Colour and ca (ca is different for conker)
	u8 la, b2, g2, r2;
	union
	{
		struct
		{
			s8 pad0, dir_z, dir_y, dir_x;	// Direction
			u8 pad1, qa, pad2, nonzero;
		};
		struct
		{
			s16 y1, x1, w1, z1;		// Position, GBI2 ex Majora's Mask
		};
	};
	s32 pad4, pad5, pad6, pad7;		// Padding..
	s16 y, x, w, z; 				// Position, Conker
};

struct TriDKR
{
    u8	v2, v1, v0, flag;
    s16	t0, s0;
    s16	t1, s1;
    s16	t2, s2;
};

struct RDP_Scissor
{
	u32 left, top, right, bottom;
};

// The display list PC stack. Before this was an array of 10
// items, but this way we can nest as deeply as necessary.
struct DList
{
	u32 address[kMaxDisplayListDepth];
	s32 limit;
};


void RDP_MoveMemViewport(u32 address);
void MatrixFromN64FixedPoint( Matrix4x4 & mat, u32 address );
void DLParser_InitMicrocode( u32 code_base, u32 code_size, u32 data_base, u32 data_size );
void RDP_MoveMemLight(u32 light_idx, const N64Light *light);

// Used to keep track of when we're processing the first display list
static bool gFirstCall = true;

static u32				gSegments[16];
static RDP_Scissor		scissors;
static RDP_GeometryMode gGeometryMode;
static DList			gDlistStack;
static s32				gDlistStackPointer = -1;
static u32				gVertexStride	 = 0;
static u32				gRDPHalf1		 = 0;
static u32				gLastUcodeBase   = 0;

       SImageDescriptor g_TI = { G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, 0 };
static SImageDescriptor g_CI = { G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, 0 };
static SImageDescriptor g_DI = { G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, 0 };

const MicroCodeInstruction *gUcodeFunc = NULL;
MicroCodeInstruction gCustomInstruction[256];

static const char ** gUcodeName = gNormalInstructionName[ 0 ];
static const char * gCustomInstructionName[256];

#ifdef DAEDALUS_ENABLE_PROFILING

#define PROFILE_DL_CMD( cmd )		\
        rmt_BeginCPUSampleDynamic(gUcodeName[ cmd ], RMTSF_Aggregate);  \
        rmt_EndCPUSampleOnScopeExit rmt_ScopedCPUSampleDl;
#else

#define PROFILE_DL_CMD( cmd )		do { } while(0)

#endif

inline void FinishRDPJob()
{
	Memory_MI_SetRegisterBits(MI_INTR_REG, MI_INTR_DP);
	gCPUState.AddJob(CPU_CHECK_INTERRUPTS);
}

// Reads the next command from the display list, updates the PC.
inline MicroCodeCommand	DLParser_FetchNextCommand()
{
	u32 pc = gDlistStack.address[gDlistStackPointer];
	DAEDALUS_ASSERT(pc + 8 <= MAX_RAM_ADDRESS, "Display list PC is out of range: 0x%08x", pc);
	MicroCodeCommand command = *(MicroCodeCommand*)(gu8RamBase + pc);
	gDlistStack.address[gDlistStackPointer] += 8;
	return command;
}

inline void DLParser_PushDisplayList()
{
	gDlistStackPointer++;
}

inline void DLParser_SetDisplayList(u32 address)
{
	gDlistStack.address[gDlistStackPointer] = address & (MAX_RAM_ADDRESS - 1);
}

inline void DLParser_PopDL()
{
	gDlistStackPointer--;
}

static std::string GetTileIdxText(int tile_idx)
{
	switch (tile_idx) {
		case G_TX_LOADTILE:	return "G_TX_LOADTILE";
		case G_TX_RENDERTILE: return "G_TX_RENDERTILE";
	}
	return absl::StrCat(tile_idx);
}

#ifdef DAEDALUS_DEBUG_DISPLAYLIST
void DLParser_DumpVtxInfo(u32 address, u32 v0_idx, u32 num_verts);

u32			gNumDListsCulled;
u32			gNumVertices;
u32			gNumRectsClipped;
#endif

u32 gRDPFrame		= 0;
u32 gAuxAddr		= 0;

extern u32 gViWidthMinusOne;
extern u32 gViHeightMinusOne;

// Include ucode header files
#include "uCodes/Ucode_GBI0.h"
#include "uCodes/Ucode_GBI1.h"
#include "uCodes/Ucode_GBI2.h"
#include "uCodes/Ucode_DKR.h"
#include "uCodes/Ucode_FB.h"
#include "uCodes/Ucode_GE.h"
#include "uCodes/Ucode_PD.h"
#include "uCodes/Ucode_Conker.h"
#include "uCodes/Ucode_LL.h"
#include "uCodes/Ucode_WRUS.h"
#include "uCodes/Ucode_SOTE.h"
#include "uCodes/Ucode_Sprite2D.h"
#include "uCodes/Ucode_S2DEX.h"

static const char * const gFormatNames[8] = {
	"G_IM_FMT_RGBA",
	"G_IM_FMT_YUV",
	"G_IM_FMT_CI",
	"G_IM_FMT_IA",
	"G_IM_FMT_I",
	"G_IM_FMT_?1",
	"G_IM_FMT_?2",
	"G_IM_FMT_?3",
};
static const char * const gSizeNames[4] =
{
	"G_IM_FMT_SIZ_4b",
	"G_IM_FMT_SIZ_8b",
	"G_IM_FMT_SIZ_16b",
	"G_IM_FMT_SIZ_32b",
};

#ifdef DAEDALUS_DEBUG_DISPLAYLIST

void DLParser_DumpVtxInfo(u32 address, u32 v0_idx, u32 num_verts)
{
	if (!DLDebug_IsActive())
	{
		return;
	}

    const char * cols[] = {"#", "x", "y", "z", "?", "u", "v", "norm", "rgba"};

	DL_NOTE("<table class='vertex-table'>");
    DL_NOTE("<tr><th>%s</th></tr>", absl::StrJoin(cols, "</th><th>").c_str());

	std::string v[ARRAYSIZE(cols)];

	const FiddledVtx * pVtxBase = reinterpret_cast<const FiddledVtx*>(gu8RamBase + address);
	for ( u32 idx = v0_idx; idx < v0_idx + num_verts; idx++ )
	{
		const FiddledVtx & vert = pVtxBase[idx - v0_idx];

		v[0] = absl::StrCat(idx);
		v[1] = absl::StrCat(vert.x);
		v[2] = absl::StrCat(vert.y);
		v[3] = absl::StrCat(vert.z);
		v[4] = absl::StrCat(vert.flag);
		v[5] = absl::StrCat(vert.tu);
		v[6] = absl::StrCat(vert.tv);
		v[7] = absl::StrCat(vert.norm_x, ",", vert.norm_y, ",", vert.norm_z, ",", vert.norm_a);
		v[8] = MakeColourTextRGBA(vert.rgba_r, vert.rgba_g, vert.rgba_b, vert.rgba_a);

		// TODO(strmnnrmn): Also include computed properties.
		//u32 clip_flags = gRenderer->GetVtxFlags( idx );
		//const v4 & t = gRenderer->GetTransformedVtxPos( idx );
		//const v4 & p = gRenderer->GetProjectedVtxPos( idx );

		DL_NOTE("<tr><td>%s</td></tr>", absl::StrJoin(v, "</td><td>").c_str());
	}
	DL_NOTE("</table>");
}
#endif

bool DLParser_Initialise()
{
	gFirstCall = true;
	gRDPFrame = 0;

	// Reset scissor to default
	scissors.top = 0;
	scissors.left = 0;
	scissors.right = 320;
	scissors.bottom = 240;

	GBIMicrocode_Reset();

	// TODO(strmnnrmn): Should we zero more state here?
#ifdef DAEDALUS_FAST_TMEM
	//Clear pointers in TMEM block //Corn
	memset(gTlutLoadAddresses, 0, sizeof(gTlutLoadAddresses));
#endif
	return true;
}

void DLParser_Finalise()
{
}

static void SetCommand( u8 cmd, MicroCodeInstruction func, const char* name )
{
	gCustomInstruction[ cmd ] = func;
	gCustomInstructionName[ cmd ] = name;
}

// This is called from Microcode.cpp after a custom ucode has been detected and cached
// This function is only called once per custom ucode set
// Main resaon for this function is to save memory since custom ucodes share a common table
//	ucode:			custom ucode (ucode>= MAX_UCODE)
//	offset:			offset to normal ucode this custom ucode is based of ex GBI0
static void DLParser_SetCustom( u32 ucode, u32 offset )
{
	for (int i = 0; i < 256; i++)
	{
		gCustomInstruction[i] = gNormalInstruction[offset][i];
		gCustomInstructionName[i] = gNormalInstructionName[offset][i];
	}

	// Start patching to create our custom ucode table ;)
	switch( ucode )
	{
		case GBI_GE:
			SetCommand( 0xb4, DLParser_RDPHalf1_GoldenEye, "G_RDPHalf1_GoldenEye" );
			break;
		case GBI_WR:
			SetCommand( 0x04, DLParser_GBI0_Vtx_WRUS, "G_Vtx_WRUS" );
			SetCommand( 0xb1, DLParser_Nothing,		  "G_Nothing" ); // Just in case
			break;
		case GBI_SE:
			SetCommand( 0x04, DLParser_GBI0_Vtx_SOTE, "G_Vtx_SOTE" );
			break;
		case GBI_LL:
			SetCommand( 0x80, DLParser_Last_Legion_0x80,	"G_Last_Legion_0x80" );
			SetCommand( 0x00, DLParser_Last_Legion_0x00,	"G_Last_Legion_0x00" );
			SetCommand( 0xe4, DLParser_TexRect_Last_Legion,	"G_TexRect_Last_Legion" );
			break;
		case GBI_PD:
			SetCommand( 0x04, DLParser_Vtx_PD,				"G_Vtx_PD" );
			SetCommand( 0x07, DLParser_Set_Vtx_CI_PD,		"G_Set_Vtx_CI_PD" );
			SetCommand( 0xb4, DLParser_RDPHalf1_GoldenEye,	"G_RDPHalf1_GoldenEye" );
			break;
		case GBI_DKR:
			SetCommand( 0x01, DLParser_Mtx_DKR,		 "G_Mtx_DKR" );
			SetCommand( 0x04, DLParser_GBI0_Vtx_DKR, "G_Vtx_DKR" );
			SetCommand( 0x05, DLParser_DMA_Tri_DKR,  "G_DMA_Tri_DKR" );
			SetCommand( 0x07, DLParser_DLInMem,		 "G_DLInMem" );
			SetCommand( 0xbc, DLParser_MoveWord_DKR, "G_MoveWord_DKR" );
			SetCommand( 0xbf, DLParser_Set_Addr_DKR, "G_Set_Addr_DKR" );
			SetCommand( 0xbb, DLParser_GBI1_Texture_DKR,"G_Texture_DKR" );
			break;
		case GBI_CONKER:
			SetCommand( 0x01, DLParser_Vtx_Conker,	"G_Vtx_Conker" );
			SetCommand( 0x05, DLParser_Tri1_Conker, "G_Tri1_Conker" );
			SetCommand( 0x06, DLParser_Tri2_Conker, "G_Tri2_Conker" );
			SetCommand( 0x10, DLParser_Tri4_Conker, "G_Tri4_Conker" );
			SetCommand( 0x11, DLParser_Tri4_Conker, "G_Tri4_Conker" );
			SetCommand( 0x12, DLParser_Tri4_Conker, "G_Tri4_Conker" );
			SetCommand( 0x13, DLParser_Tri4_Conker, "G_Tri4_Conker" );
			SetCommand( 0x14, DLParser_Tri4_Conker, "G_Tri4_Conker" );
			SetCommand( 0x15, DLParser_Tri4_Conker, "G_Tri4_Conker" );
			SetCommand( 0x16, DLParser_Tri4_Conker, "G_Tri4_Conker" );
			SetCommand( 0x17, DLParser_Tri4_Conker, "G_Tri4_Conker" );
			SetCommand( 0x18, DLParser_Tri4_Conker, "G_Tri4_Conker" );
			SetCommand( 0x19, DLParser_Tri4_Conker, "G_Tri4_Conker" );
			SetCommand( 0x1a, DLParser_Tri4_Conker, "G_Tri4_Conker" );
			SetCommand( 0x1b, DLParser_Tri4_Conker, "G_Tri4_Conker" );
			SetCommand( 0x1c, DLParser_Tri4_Conker, "G_Tri4_Conker" );
			SetCommand( 0x1d, DLParser_Tri4_Conker, "G_Tri4_Conker" );
			SetCommand( 0x1e, DLParser_Tri4_Conker, "G_Tri4_Conker" );
			SetCommand( 0x1f, DLParser_Tri4_Conker, "G_Tri4_Conker" );
			SetCommand( 0xdb, DLParser_MoveWord_Conker,  "G_MoveWord_Conker");
			SetCommand( 0xdc, DLParser_MoveMem_Conker,   "G_MoveMem_Conker" );
			break;
	}
}

void DLParser_InitMicrocode( u32 code_base, u32 code_size, u32 data_base, u32 data_size )
{
	u32 ucode = GBIMicrocode_DetectVersion( code_base, code_size, data_base, data_size, &DLParser_SetCustom );

	gVertexStride  = ucode_stride[ucode];
	gLastUcodeBase = code_base;
	gUcodeFunc	   = IS_CUSTOM_UCODE(ucode) ? gCustomInstruction : gNormalInstruction[ucode];

	// Used for fetching ucode names (Debug Only)
	gUcodeName = IS_CUSTOM_UCODE(ucode) ? gCustomInstructionName : gNormalInstructionName[ucode];
}

// Process the entire display list in one go
static u32 DLParser_ProcessDList(u32 instruction_limit)
{
	DAEDALUS_PROFILE( "DLParser_ProcessDList" );
	MicroCodeCommand command;

	u32 current_instruction_count = 0;

	while(gDlistStackPointer >= 0)
	{
		command = DLParser_FetchNextCommand();

		DL_BEGIN_INSTR(current_instruction_count, command.inst.cmd0, command.inst.cmd1, gDlistStackPointer, gUcodeName[command.inst.cmd]);

		PROFILE_DL_CMD( command.inst.cmd );

		gUcodeFunc[ command.inst.cmd ]( command );

		DL_END_INSTR();

#ifdef DAEDALUS_DEBUG_DISPLAYLIST
		// Note: make sure have frame skip disabled for the dlist debugger to work
		if( instruction_limit != kUnlimitedInstructionCount )
		{
			if( current_instruction_count >= instruction_limit )
			{
				return current_instruction_count;
			}
		}
		current_instruction_count++;
#endif

		// Check limit
		if (gDlistStack.limit >= 0)
		{
			if (--gDlistStack.limit < 0)
			{
				DL_PF("**EndDLInMem");
				gDlistStackPointer--;
				// limit is already reset to default -1 at this point
				//gDlistStack.limit = -1;
			}
		}
	}

	return current_instruction_count;
}

u32 DLParser_Process(u32 instruction_limit, DLDebugOutput * debug_output)
{
	DAEDALUS_PROFILE( "DLParser_Process" );

	if ( !CGraphicsContext::Get()->IsInitialised() || !gRenderer )
	{
		return 0;
	}

	// Shut down the debug console when we start rendering
	// TODO: Clear the front/backbuffer the first time this function is called
	// to remove any stuff lingering on the screen.
	if(gFirstCall)
	{
		CGraphicsContext::Get()->ClearAllSurfaces();

		gFirstCall = false;
	}

	// Update Screen only when something is drawn, otherwise several games ex Army Men will flash or shake.
	if( g_ROM.GameHacks != CHAMELEON_TWIST_2 ) gHLEGraphics->UpdateScreen();

	OSTask * pTask = (OSTask *)(gu8SpMemBase + 0x0FC0);
	u32 code_base = (u32)pTask->t.ucode & 0x1fffffff;
	u32 code_size = pTask->t.ucode_size;
	u32 data_base = (u32)pTask->t.ucode_data & 0x1fffffff;
	u32 data_size = pTask->t.ucode_data_size;
	u32 stack_size = pTask->t.dram_stack_size >> 6;

	if ( gLastUcodeBase != code_base )
	{
		DLParser_InitMicrocode( code_base, code_size, data_base, data_size );
	}

	//
	// Not sure what to init this with. We should probably read it from the dmem
	//
	gRDPOtherMode.L = 0x00500001;
	gRDPOtherMode.H = 0;

	gRDPFrame++;

	CTextureCache::Get()->PurgeOldTextures();

	// Initialise stack
	gDlistStackPointer=0;
	gDlistStack.address[0] = (u32)pTask->t.data_ptr;
	gDlistStack.limit = -1;

	gRDPStateManager.Reset();

#ifdef DAEDALUS_DEBUG_DISPLAYLIST
	gNumDListsCulled = 0;
	gNumVertices = 0;
	gNumRectsClipped = 0;
	DLDebug_SetOutput(debug_output);
	DLDebug_DumpTaskInfo(pTask);
#endif

	gRenderer->SetVIScales();
	gRenderer->ResetMatrices(stack_size);
	gRenderer->Reset();
	gRenderer->BeginScene();
	u32 count = DLParser_ProcessDList(instruction_limit);
	gRenderer->EndScene();

	// Hack for Chameleon Twist 2, only works if screen is update at last
	if( g_ROM.GameHacks == CHAMELEON_TWIST_2 ) gHLEGraphics->UpdateScreen();

	// Do this regardless!
	FinishRDPJob();

#ifdef DAEDALUS_DEBUG_DISPLAYLIST
	DLDebug_SetOutput(nullptr);
#endif

	return count;
}

void MatrixFromN64FixedPoint( Matrix4x4 & mat, u32 address )
{
	DAEDALUS_ASSERT( address+64 < MAX_RAM_ADDRESS, "Mtx: Address invalid (0x%08x)", address);

	const f32 fRecip = 1.0f / 65536.0f;
	const N64mat *Imat = (N64mat *)( gu8RamBase + address );

	s16 hi;
	s32 tmp;
	for (u32 i = 0; i < 4; i++)
	{
#if 1	// Crappy compiler.. reordering is to optimize the ASM // Corn
		tmp = ((Imat->h[i].x << 16) | Imat->l[i].x);
		hi = Imat->h[i].y;
		mat.m[i][0] =  tmp * fRecip;

		tmp = ((hi << 16) | Imat->l[i].y);
		hi = Imat->h[i].z;
		mat.m[i][1] = tmp * fRecip;

		tmp = ((hi << 16) | Imat->l[i].z);
		hi = Imat->h[i].w;
		mat.m[i][2] = tmp * fRecip;

		tmp = ((hi << 16) | Imat->l[i].w);
		mat.m[i][3] = tmp * fRecip;
#else
		mat.m[i][0] = ((Imat->h[i].x << 16) | Imat->l[i].x) * fRecip;
		mat.m[i][1] = ((Imat->h[i].y << 16) | Imat->l[i].y) * fRecip;
		mat.m[i][2] = ((Imat->h[i].z << 16) | Imat->l[i].z) * fRecip;
		mat.m[i][3] = ((Imat->h[i].w << 16) | Imat->l[i].w) * fRecip;
#endif
	}
}

void RDP_MoveMemLight(u32 light_idx, const N64Light *light)
{
	DAEDALUS_ASSERT( light_idx < 12, "Warning: invalid light # = %d", light_idx );

	u8 r = light->r;
	u8 g = light->g;
	u8 b = light->b;

	s8 dir_x = light->dir_x;
	s8 dir_y = light->dir_y;
	s8 dir_z = light->dir_z;

	DL_NOTE("col (%s) dir (%d, %d, %d)", MakeColourTextRGB(r, g, b).c_str(), dir_x, dir_y, dir_z);

	//Light color
	gRenderer->SetLightCol( light_idx, r, g, b );

	//Direction
	gRenderer->SetLightDirection( light_idx, dir_x, dir_y, dir_z );
}

//0x000b46b0: dc080008 800b46a0 G_GBI2_MOVEMEM
//    Type: 08 Len: 08 Off: 0000
//        Scale: 640 480 511 0 = 160,120
//        Trans: 640 480 511 0 = 160,120
//vscale is the scale applied to the normalized homogeneous coordinates after 4x4 projection transformation
//vtrans is the offset added to the scaled number

void RDP_MoveMemViewport(u32 address)
{
	DAEDALUS_ASSERT( address+16 < MAX_RAM_ADDRESS, "MoveMem Viewport, invalid memory" );

	// Address is offset into RD_RAM of 8 x 16bits of data...
	// TODO(strmnrmn): Why doesn't this need byteswapping?
	N64Viewport *vp = (N64Viewport*)(gu8RamBase + address);

	v2 vec_scale( vp->scale_x / 4.0f, vp->scale_y / 4.0f );
	v2 vec_trans( vp->trans_x / 4.0f, vp->trans_y / 4.0f );

	DL_NOTE("Scale: %d %d", vp->scale_x, vp->scale_y);
	DL_NOTE("Trans: %d %d", vp->trans_x, vp->trans_y);

	gRenderer->SetN64Viewport( vec_scale, vec_trans );
}

//Nintro64 uses Sprite2d
void DLParser_Nothing( MicroCodeCommand command )
{
	DAEDALUS_DL_ERROR( "RDP Command %08x Does not exist...", command.inst.cmd0 );

	// Terminate!
	//	Console_Print("Warning, DL cut short with unknown command: 0x%08x 0x%08x", command.inst.cmd0, command.inst.cmd1);
	DLParser_PopDL();

}

void DLParser_SetKeyGB( MicroCodeCommand command )
{
	DL_COMMAND("gsDPSetKeyGB(?);");
}

void DLParser_SetKeyR( MicroCodeCommand command )
{
	DL_COMMAND("gsDPSetKeyR(?);");
}

void DLParser_SetConvert( MicroCodeCommand command )
{
	DL_COMMAND("gsDPSetConvert(?);");
}

void DLParser_SetPrimDepth( MicroCodeCommand command )
{
	DL_COMMAND("gsDPSetPrimDepth(%d, %d);", command.primdepth.z, command.primdepth.dz);

	gRenderer->SetPrimitiveDepth( command.primdepth.z );
}

void DLParser_RDPSetOtherMode( MicroCodeCommand command )
{
	DL_PF( "    RDPSetOtherMode: 0x%08x 0x%08x", command.inst.cmd0, command.inst.cmd1 );

	gRDPOtherMode.H = command.inst.cmd0;
	gRDPOtherMode.L = command.inst.cmd1;

#ifdef DAEDALUS_DEBUG_DISPLAYLIST
	DLDebug_DumpRDPOtherMode(gRDPOtherMode);
#endif
}

void DLParser_RDPLoadSync( MicroCodeCommand command )
{
	DL_COMMAND("gsDPLoadSync();");
}
void DLParser_RDPPipeSync( MicroCodeCommand command )
{
	DL_COMMAND("gsDPPipeSync();");
}
void DLParser_RDPTileSync( MicroCodeCommand command )
{
	DL_COMMAND("gsDPTileSync();");
}

void DLParser_RDPFullSync( MicroCodeCommand command )
{
	DL_COMMAND("gsDPFullSync();");

	// This is done after DLIST processing anyway
	// FinishRDPJob();
}

static const char* const kScissorModeNames[] =
{
	"G_SC_NON_INTERLACE",
	"?",
	"G_SC_EVEN_INTERLACE",
	"G_SC_ODD_INTERLACE",
};

void DLParser_SetScissor( MicroCodeCommand command )
{
	// The coords are all in 10:2 fixed point
	// Set up scissoring zone, we'll use it to scissor other stuff ex Texrect
	//
	scissors.left    = command.scissor.x0>>2;
	scissors.top     = command.scissor.y0>>2;
	scissors.right   = command.scissor.x1>>2;
	scissors.bottom  = command.scissor.y1>>2;

	// Hack to correct Super Bowling's right and left screens
	if ( g_ROM.GameHacks == SUPER_BOWLING && g_CI.Address%0x100 != 0 )
	{
		scissors.left += 160;
		scissors.right += 160;
		v2 vec_trans( 240, 120 );
		v2 vec_scale( 80, 120 );
		gRenderer->SetN64Viewport( vec_scale, vec_trans );
	}

	DL_COMMAND("gsDPSetScissor(%s, %d, %d, %d, %d);",
		kScissorModeNames[command.scissor.mode],
		scissors.left, scissors.top, scissors.right, scissors.bottom);

	// Set the cliprect now...
	if ( scissors.left < scissors.right && scissors.top < scissors.bottom )
	{
		gRenderer->SetScissor( scissors.left, scissors.top, scissors.right, scissors.bottom );
	}
}

const char* GetClampMirrorWrapText(bool clamp, bool mirror)
{
	if (clamp && mirror) return "GG_TX_MIRROR|G_TX_CLAMP";
	if (clamp) return "G_TX_CLAMP";
	if (mirror) return "GG_TX_MIRROR";
	return "GG_TX_WRAP";
}

void DLParser_SetTile( MicroCodeCommand command )
{
	RDP_Tile tile;
	tile.cmd0 = command.inst.cmd0;
	tile.cmd1 = command.inst.cmd1;

	DL_COMMAND("gsDPSetTile(%s, %s, %d, %d, %s, %d, %s, %d, %d, %s, %d, %d);",
		gFormatNames[tile.format], gSizeNames[tile.size],
		tile.line, tile.tmem, GetTileIdxText(tile.tile_idx).c_str(),
		tile.palette,
		GetClampMirrorWrapText(tile.clamp_s, tile.mirror_s), tile.mask_s, tile.shift_s,
		GetClampMirrorWrapText(tile.clamp_t, tile.mirror_t), tile.mask_t, tile.shift_t);

	gRDPStateManager.SetTile( tile );
}

void DLParser_SetTileSize( MicroCodeCommand command )
{
	RDP_TileSize tile;
	tile.cmd0 = command.inst.cmd0;
	tile.cmd1 = command.inst.cmd1;

	DL_COMMAND("gsDPSetTileSize(%s, %d, %d, %d, %d); // (%d, %d) -> (%d, %d) = [%d, %d]",
		GetTileIdxText(tile.tile_idx).c_str(),
		tile.left, tile.top, tile.right, tile.bottom,
		tile.left/4, tile.top/4,
        tile.right/4, tile.bottom/4,
		tile.GetWidth(), tile.GetHeight());

	gRDPStateManager.SetTileSize( tile );
}

void DLParser_SetTImg( MicroCodeCommand command )
{
	DL_COMMAND("gsDPSetTextureImage(%s, %s, %d, 0x%08x);",
		gFormatNames[command.img.fmt],
		gSizeNames[command.img.siz],
		command.img.width + 1,
		RDPSegAddr(command.img.addr));

	g_TI.Format		= command.img.fmt;
	g_TI.Size		= command.img.siz;
	g_TI.Width		= command.img.width + 1;
	g_TI.Address	= RDPSegAddr(command.img.addr) & (MAX_RAM_ADDRESS-1);
	//g_TI.bpl		= g_TI.Width << g_TI.Size >> 1;
}

void DLParser_LoadBlock( MicroCodeCommand command )
{
	const SetLoadTile & load = command.loadtile;

	DL_COMMAND("gsDPLoadBlock(%s, %d, %d, %d, 0x%08x);",
		GetTileIdxText(load.tile).c_str(), load.sl, load.tl, load.sh, load.th);

	gRDPStateManager.LoadBlock( load );
}

void DLParser_LoadTile( MicroCodeCommand command )
{
	const SetLoadTile& load = command.loadtile;

	DL_COMMAND("gsDPLoadTile(%s, %d, %d, %d, %d);",
		GetTileIdxText(load.tile).c_str(),
		load.sl / 4, load.tl / 4,
		load.sh / 4, load.th / 4);
	DL_NOTE("(%d x %d), (%d, %d)",
		load.sl / 4, load.tl / 4,
		(load.sh - load.sl) / 4 + 1,
		(load.th - load.tl) / 4 + 1);

	gRDPStateManager.LoadTile(load);
}

void DLParser_LoadTLut( MicroCodeCommand command )
{
	gRDPStateManager.LoadTlut( command.loadtile );
}

void DLParser_TexRect( MicroCodeCommand command )
{
	MicroCodeCommand command2 = DLParser_FetchNextCommand();
	MicroCodeCommand command3 = DLParser_FetchNextCommand();

	RDP_TexRect tex_rect;
	tex_rect.cmd0 = command.inst.cmd0;
	tex_rect.cmd1 = command.inst.cmd1;
	tex_rect.cmd2 = command2.inst.cmd1;
	tex_rect.cmd3 = command3.inst.cmd1;

	DAEDALUS_DL_ASSERT(gRDPOtherMode.cycle_type != CYCLE_COPY || tex_rect.dsdx == (4<<10), "Expecting dsdx of 4<<10 in copy mode, got %d", tex_rect.dsdx);

	// NB: In FILL and COPY mode, rectangles are scissored to the nearest four pixel boundary.
	// This isn't currently handled, but I don't know of any games that depend on it.

	//Keep integers for as long as possible //Corn

	// X for upper left corner should be less than X for lower right corner else skip rendering it, seems to happen in Rayman 2 and Star Soldier
	//if( tex_rect.x0 >= tex_rect.x1 )

	// Hack for Banjo Tooie shadow
	if (g_ROM.GameHacks == BANJO_TOOIE && gRDPOtherMode.L == 0x00504241)
	{
		return;
	}

	// Fixes black box in SSB when moving far way from the screen and offscreen in Conker
	if (g_DI.Address == g_CI.Address || g_CI.Format != G_IM_FMT_RGBA)
	{
		DL_NOTE("Ignoring Texrect");
		return;
	}

	// Removes offscreen texrect, also fixes several glitches like in John Romero's Daikatana
	if( tex_rect.x0 >= (scissors.right<<2) ||
		tex_rect.y0 >= (scissors.bottom<<2) ||
		tex_rect.x1 <  (scissors.left<<2) ||
		tex_rect.y1 <  (scissors.top<<2) )
	{
#ifdef DAEDALUS_DEBUG_DISPLAYLIST
		++gNumRectsClipped;
#endif
		return;
	};

	s16 rect_s0 = tex_rect.s;
	s16 rect_t0 = tex_rect.t;

	s32 rect_dsdx = tex_rect.dsdx;
	s32 rect_dtdy = tex_rect.dtdy;

	rect_s0 += (((u32)rect_dsdx >> 31) << 5);	//Fixes California Speed, if(rect_dsdx<0) rect_s0 += 32;
	rect_t0 += (((u32)rect_dtdy >> 31) << 5);

	// In Fill/Copy mode the coordinates are inclusive (i.e. add 1<<2 to the w/h)
	u32 cycle_mode = gRDPOtherMode.cycle_type;
	if ( cycle_mode >= CYCLE_COPY )
	{
		// In copy mode 4 pixels are copied at once.
		if ( cycle_mode == CYCLE_COPY )
			rect_dsdx = rect_dsdx >> 2;

		tex_rect.x1 += 4;
		tex_rect.y1 += 4;
	}

	s16 rect_s1 = rect_s0 + (rect_dsdx * ( tex_rect.x1 - tex_rect.x0 ) >> 7);	// 7 = (>>10)=1/1024, (>>2)=1/4 and (<<5)=32
	s16 rect_t1 = rect_t0 + (rect_dtdy * ( tex_rect.y1 - tex_rect.y0 ) >> 7);

	TexCoord st0( rect_s0, rect_t0 );
	TexCoord st1( rect_s1, rect_t1 );

	v2 xy0( tex_rect.x0 / 4.0f, tex_rect.y0 / 4.0f );
	v2 xy1( tex_rect.x1 / 4.0f, tex_rect.y1 / 4.0f );

	DL_COMMAND("gsSPTextureRectangle(%.1f, %.1f, %.1f, %.1f, %s, %#5.3f, %#5.3f, %#5.3f, %#5.3f);",
		xy0.x, xy0.y, xy1.x, xy1.y,
		GetTileIdxText(tex_rect.tile_idx).c_str(),
		st0.s/32.f, st0.t/32.f, st1.s/32.f, st1.t/32.f);

	gRenderer->TexRect( tex_rect.tile_idx, xy0, xy1, st0, st1 );
}

void DLParser_TexRectFlip( MicroCodeCommand command )
{
	MicroCodeCommand command2 = DLParser_FetchNextCommand();
	MicroCodeCommand command3 = DLParser_FetchNextCommand();

	RDP_TexRect tex_rect;
	tex_rect.cmd0 = command.inst.cmd0;
	tex_rect.cmd1 = command.inst.cmd1;
	tex_rect.cmd2 = command2.inst.cmd1;
	tex_rect.cmd3 = command3.inst.cmd1;

	DAEDALUS_DL_ASSERT(gRDPOtherMode.cycle_type != CYCLE_COPY || tex_rect.dsdx == (4<<10), "Expecting dsdx of 4<<10 in copy mode, got %d", tex_rect.dsdx);

	//Keep integers for as long as possible //Corn

	s16 rect_s0 = tex_rect.s;
	s16 rect_t0 = tex_rect.t;

	s32 rect_dsdx = tex_rect.dsdx;
	s32 rect_dtdy = tex_rect.dtdy;

	rect_s0 += (((u32)rect_dsdx >> 31) << 5);	// For Wetrix
	rect_t0 += (((u32)rect_dtdy >> 31) << 5);

	// In Fill/Copy mode the coordinates are inclusive (i.e. add 1<<2 to the w/h)
	u32 cycle_mode = gRDPOtherMode.cycle_type;
	if ( cycle_mode >= CYCLE_COPY )
	{
		// In copy mode 4 pixels are copied at once.
		if ( cycle_mode == CYCLE_COPY )
			rect_dsdx = rect_dsdx >> 2;

		tex_rect.x1 += 4;
		tex_rect.y1 += 4;
	}

	s16 rect_s1 = rect_s0 + (rect_dsdx * ( tex_rect.y1 - tex_rect.y0 ) >> 7);	// Flip - use y
	s16 rect_t1 = rect_t0 + (rect_dtdy * ( tex_rect.x1 - tex_rect.x0 ) >> 7);	// Flip - use x

	TexCoord st0( rect_s0, rect_t0 );
	TexCoord st1( rect_s1, rect_t1 );

	v2 xy0( tex_rect.x0 / 4.0f, tex_rect.y0 / 4.0f );
	v2 xy1( tex_rect.x1 / 4.0f, tex_rect.y1 / 4.0f );

	DL_PF("    Screen(%.1f,%.1f) -> (%.1f,%.1f) Tile[%d]", xy0.x, xy0.y, xy1.x, xy1.y, tex_rect.tile_idx);
	DL_PF("    FlipTex:(%#5.3f,%#5.3f) -> (%#5.3f,%#5.3f) (DSDX:%#5f DTDY:%#5f)", rect_s0/32.f, rect_t0/32.f, rect_s1/32.f, rect_t1/32.f, rect_dsdx/1024.f, rect_dtdy/1024.f);

	gRenderer->TexRectFlip( tex_rect.tile_idx, xy0, xy1, st0, st1 );
}

//Clear framebuffer, thanks Gonetz! http://www.emutalk.net/threads/15818-How-to-implement-quot-emulate-clear-quot-Answer-and-Question
//This fixes the jumpy camera in DK64, also the sun and flames glare in Zelda
void Clear_N64DepthBuffer( MicroCodeCommand command )
{
	u32 x0 = command.fillrect.x0 + 1;
	u32 x1 = command.fillrect.x1 + 1;
	u32 y1 = command.fillrect.y1;
	u32 y0 = command.fillrect.y0;

	// Using s32 to force min/max to be done in a single op code for the PSP
	x0 = Min<s32>(Max<s32>(x0, scissors.left), scissors.right);
	x1 = Min<s32>(Max<s32>(x1, scissors.left), scissors.right);
	y1 = Min<s32>(Max<s32>(y1, scissors.top), scissors.bottom);
	y0 = Min<s32>(Max<s32>(y0, scissors.top), scissors.bottom);
	x0 >>= 1;
	x1 >>= 1;
	u32 zi_width_in_dwords = g_CI.Width >> 1;
	u32 fill_colour = gRenderer->GetFillColour();
	u32 * dst = (u32*)(gu8RamBase + g_CI.Address) + y0 * zi_width_in_dwords;

	for( u32 y = y0; y <y1; y++ )
	{
		for( u32 x = x0; x < x1; x++ )
		{
			dst[x] = fill_colour;
		}
		dst += zi_width_in_dwords;
	}
}

void DLParser_FillRect( MicroCodeCommand command )
{
	// Removes annoying rect that appears in Conker and fillrects that cover screen in banjo tooie
	if( g_CI.Format != G_IM_FMT_RGBA )
	{
		DL_PF("Ignoring Fillrect");
		return;
	}

	// Always clear Zbuffer if Depthbuffer is selected //Corn
	if (g_DI.Address == g_CI.Address)
	{
		CGraphicsContext::Get()->ClearZBuffer();

#ifdef DAEDALUS_PSP
		if(gClearDepthFrameBuffer)
#else
		if(true)	// This always enabled for PC, this should be optional once we have a GUI to disable it!
#endif
		{
			Clear_N64DepthBuffer(command);
		}
		DL_NOTE("Clearing ZBuffer");
		return;
	}

	DL_COMMAND("gsDPFillRectangle(%d, %d, %d, %d);",
		command.fillrect.x0, command.fillrect.y0,
		command.fillrect.x1, command.fillrect.y1);

	// Note, in some modes, the right/bottom lines aren't drawn

	// TODO - Check colour image format to work out how this should be decoded!
	// Should we init with Prim or Blend colour? Doesn't work well see Mk64 transition before a race
	c32 colour = c32(0);

	u32 cycle_mode = gRDPOtherMode.cycle_type;
	// In Fill/Copy mode the coordinates are inclusive (i.e. add 1.0f to the w/h)
	if ( cycle_mode >= CYCLE_COPY )
	{
		if ( cycle_mode == CYCLE_FILL )
		{
			u32 fill_colour = gRenderer->GetFillColour();
			if(g_CI.Size == G_IM_SIZ_16b)
			{
				const N64Pf5551	c( (u16)fill_colour );
				colour = ConvertPixelFormat< c32, N64Pf5551 >( c );
			}
			else
			{
				const N64Pf8888	c( (u32)fill_colour );
				colour = ConvertPixelFormat< c32, N64Pf8888 >( c );
			}

			u32 clear_screen_x = command.fillrect.x1 - command.fillrect.x0;
			u32 clear_screen_y = command.fillrect.y1 - command.fillrect.y0;

			// Clear color buffer (screen clear)
			if( gViWidthMinusOne == clear_screen_x && gViHeightMinusOne == clear_screen_y )
			{
				CGraphicsContext::Get()->ClearColBuffer( colour );
				DL_NOTE("Clearing Colour Buffer");
				return;
			}
		}

		command.fillrect.x1++;
		command.fillrect.y1++;
	}

	v2 xy0( (f32)command.fillrect.x0, (f32)command.fillrect.y0 );
	v2 xy1( (f32)command.fillrect.x1, (f32)command.fillrect.y1 );

	// TODO - In 1/2cycle mode, skip bottom/right edges!?
	// This is done in BaseRenderer.
	gRenderer->FillRect( xy0, xy1, colour.GetColour() );
}

void DLParser_SetZImg( MicroCodeCommand command )
{
	u32 address = RDPSegAddr(command.img.addr);

	DL_COMMAND("gsDPSetDepthImage(0x%08x);", address);

	// No need check for (MAX_RAM_ADDRESS-1) here, since g_DI.Address is never used to reference a RAM location
	g_DI.Address = address;
}

void DLParser_SetCImg( MicroCodeCommand command )
{
	u32 address = RDPSegAddr(command.img.addr);

	DL_COMMAND("gsDPSetColorImage(%s, %s, %d, 0x%08x);",
		gFormatNames[command.img.fmt],
		gSizeNames[command.img.siz],
		command.img.width + 1,
		address);

	g_CI.Format = command.img.fmt;
	g_CI.Size   = command.img.siz;
	g_CI.Width  = command.img.width + 1;
	g_CI.Address = address & (MAX_RAM_ADDRESS-1);
	//g_CI.Bpl		= g_CI.Width << g_CI.Size >> 1;
}

void DLParser_SetCombine( MicroCodeCommand command )
{
	//Swap the endian
	REG64 Mux;
	Mux._u32_0 = command.inst.cmd1;
	Mux._u32_1 = command.inst.arg0;

	gRenderer->SetMux( Mux._u64 );

#ifdef DAEDALUS_DEBUG_DISPLAYLIST
	DLDebug_DumpMux( Mux._u64 );
#endif
}

void DLParser_SetFillColor( MicroCodeCommand command )
{
	u32 fill_colour = command.inst.cmd1;

	// TODO(strmnnrmn) Colour can be 16 or 32 bit...
	DL_COMMAND("gsDPSetFillColor(%s);", MakeColourTextRGBA(fill_colour).c_str());

	gRenderer->SetFillColour( fill_colour );
}

void DLParser_SetFogColor( MicroCodeCommand command )
{
	DL_COMMAND("gsDPSetFogColor(%s);", MakeColourTextRGBA(command.inst.cmd1).c_str());

	// Ignore alpha from the command, and always set it to 0.
	c32	fog_colour( command.color.r, command.color.g, command.color.b, 0 );

	gRenderer->SetFogColour( fog_colour );
}

void DLParser_SetBlendColor( MicroCodeCommand command )
{
	DL_COMMAND("gsDPSetBlendColor(%s);", MakeColourTextRGBA(command.inst.cmd1).c_str());

	c32	blend_colour( command.color.r, command.color.g, command.color.b, command.color.a );

	gRenderer->SetBlendColour( blend_colour );
}

void DLParser_SetPrimColor( MicroCodeCommand command )
{
	DL_COMMAND("gsDPSetPrimColor(%d, %d, %s);",
		command.color.prim_min_level, command.color.prim_level, MakeColourTextRGBA(command.inst.cmd1).c_str());

	c32	prim_colour( command.color.r, command.color.g, command.color.b, command.color.a );

	gRenderer->SetPrimitiveLODFraction(command.color.prim_level / 256.f);
	gRenderer->SetPrimitiveColour( prim_colour );
}

void DLParser_SetEnvColor( MicroCodeCommand command )
{
	DL_COMMAND("gsDPSetEnvColor(%s);", MakeColourTextRGBA(command.inst.cmd1).c_str());

	c32	env_colour( command.color.r, command.color.g,command.color.b, command.color.a );

	gRenderer->SetEnvColour( env_colour );
}

// RSP TRI commands.
// In HLE emulation you NEVER see these commands !
void DLParser_TriRSP( MicroCodeCommand command )
{
	DL_PF("    RSP Tri: (Ignored)");
}


