/*
Copyright (C) 2009 StrmnNrmn

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

#ifndef HLEGRAPHICS_UCODES_UCODE_GBI1_H_
#define HLEGRAPHICS_UCODES_UCODE_GBI1_H_

// The previous way of calculating was based on the assumption that
// there was no "n" field. I didn't realise that the n/length fields shared the
// lower 16 bits (in a 6:10 split).
// u32 length    = (command.inst.cmd0)&0xFFFF;
// u32 num_verts = (length + 1) / 0x210;                        // 528
// u32 v0_idx    = ((command.inst.cmd0>>16)&0xFF)/gVertexStride;      // /5
void DLParser_GBI1_Vtx(MicroCodeCommand command)
{
	// u32 length    = (command.inst.cmd0)&0xFFFF;
	// u32 num_verts = (length + 1) / 0x410;
	// u32 v0_idx    = ((command.inst.cmd0>>16)&0x3f)/2;

	u32 addr = RDPSegAddr(command.vtx1.addr);
	u32 v0 = command.vtx1.v0;
	u32 n = command.vtx1.n;

	DL_COMMAND("gsSPVertex(0x%08x, %d, %d);", addr, n, v0);
	DAEDALUS_ASSERT((v0 + n) <= 64, "Warning, attempting to load into invalid vertex positions");

	// Wetrix
	if (addr >= MAX_RAM_ADDRESS)
	{
		DL_PF("Address out of range - ignoring load");
		return;
	}

	gRenderer->SetNewVertexInfo(addr, v0, n);

#ifdef DAEDALUS_DEBUG_DISPLAYLIST
	gNumVertices += n;
	DLParser_DumpVtxInfo(addr, v0, n);
#endif
}

void DLParser_GBI1_ModifyVtx(MicroCodeCommand command)
{
	u32 offset = command.modifyvtx.offset;
	u32 vert = command.modifyvtx.vtx;
	u32 value = command.modifyvtx.value;

	// Cures crash after swinging in Mario Golf
	if (vert > 80)
	{
		DAEDALUS_ERROR("ModifyVtx: Invalid vertex number: %d", vert);
		return;
	}

	gRenderer->ModifyVertexInfo(vert, offset, value);
}

void DLParser_GBI1_Mtx(MicroCodeCommand command)
{
	u32 address = RDPSegAddr(command.mtx1.addr);

	DL_COMMAND("gsSPMatrix(0x%08x, %s%s%s);",
		address,
		command.mtx1.projection == 1 ? "G_MTX_PROJECTION" : "G_MTX_MODELVIEW",
		command.mtx1.load == 1 ? "|G_MTX_LOAD" : "|G_MTX_MUL",
		command.mtx1.push == 1 ? "|G_MTX_PUSH" : "");

	if (command.mtx1.projection)
	{
		gRenderer->SetProjection(address, command.mtx1.load);
	}
	else
	{
		gRenderer->SetWorldView(address, command.mtx1.push, command.mtx1.load);
	}
}

void DLParser_GBI1_PopMtx(MicroCodeCommand command)
{
	DL_COMMAND("gsSPPopMatrix(%s);", command.inst.cmd1 ? "G_MTX_PROJECTION" : "G_MTX_MODELVIEW");

	// Do any of the other bits do anything?
	// So far only Extreme-G seems to Push/Pop projection matrices
	// Can't pop projection matrix
	if (command.inst.cmd1 == 0) gRenderer->PopWorldView();
}

void DLParser_GBI1_MoveMem(MicroCodeCommand command)
{
	u32 type = (command.inst.cmd0 >> 16) & 0xFF;
	u32 address = RDPSegAddr(command.inst.cmd1);

	switch (type)
	{
		case G_MV_VIEWPORT:
		{
			DL_COMMAND("gsSPViewport(0x%08x);", address);
			RDP_MoveMemViewport(address);
		}
		break;

		case G_MV_L0:
		case G_MV_L1:
		case G_MV_L2:
		case G_MV_L3:
		case G_MV_L4:
		case G_MV_L5:
		case G_MV_L6:
		case G_MV_L7:
		{
			u32 light_idx = (type - G_MV_L0) / 2;
			N64Light* light = (N64Light*)(gu8RamBase + address);

			DL_COMMAND("gsSPLight(0x%08x, LIGHT_%d);", address, light_idx+1);
			RDP_MoveMemLight(light_idx, light);
		}
		break;

		case G_MV_MATRIX_1:
		{
			DL_COMMAND("gsSPForceMatrix(0x%08x);", address);
			// Rayman 2, Donald Duck, Tarzan, all wrestling games use this
			gRenderer->ForceMatrix(address);
			// ForceMatrix takes four cmds
			gDlistStack.address[gDlistStackPointer] += 24;
		}
		break;

		// Next 3 MATRIX commands should not appear, since they were in the previous command.
		// case G_MV_MATRIX_2:	/*IGNORED*/	DL_PF("     G_MV_MATRIX_2"); break;  case
		// G_MV_MATRIX_3:	/*IGNORED*/	DL_PF("     G_MV_MATRIX_3");											break;
		// case G_MV_MATRIX_4:	/*IGNORED*/	DL_PF("     G_MV_MATRIX_4"); break;
		/*

		// Next 3 cmds are always ignored
		case G_MV_LOOKATY:
			DL_PF("    G_MV_LOOKATY");
			break;
		case G_MV_LOOKATX:
			DL_PF("    G_MV_LOOKATX");
			break;
		case G_MV_TXTATT:
			DL_PF("    G_MV_TXTATT");
			break;
*/
		default:
		{
			DL_PF("GBI1 MoveMem Type: Ignored!! 0x%08x", type);
		}
		break;
	}
}

void DLParser_GBI1_MoveWord(MicroCodeCommand command)
{
	// Type of movement is in low 8bits of cmd0.
	u32 value  = command.mw1.value;
	u32 offset = command.mw1.offset;

	switch (command.mw1.type)
	{
		case G_MW_MATRIX:
		{
			u32 where = command.inst.cmd0;
			u32 value = command.inst.cmd1;
			DL_COMMAND("gsSPInsertMatrix(0x%08x, 0x%08x);");
			gRenderer->InsertMatrix(where, value);
			break;
		}

		case G_MW_NUMLIGHT:
		{
			u32 num_lights = ((value - 0x80000000) >> 5) - 1;
			DL_COMMAND("gsSPNumLights(NUMLIGHTS_%d);", num_lights);
			gRenderer->SetNumLights(num_lights);
			break;
		}
		case G_MW_CLIP:  // Seems to be unused?
		{
			DL_COMMAND("gsSPClipRatio(0x%08x);", value);
			break;
		}

		case G_MW_SEGMENT:
		{
			u32 segment = (offset >> 2) & 0xF;
			DL_COMMAND("gsSPSegment(%d, %d);", segment, value);
			gSegments[segment] = value;
			break;
		}

		case G_MW_FOG:
		{
			s16 fm = (s16)(value >> 16);
			s16 fo = (s16)(value & 0xFFFF);
			// TODO: There's a gsSPFogPosition() which might be more intuitive?
			DL_COMMAND("gsSPFogFactor(%d, %d);", fm, fo);

#ifdef DAEDALUS_PSP
			gRenderer->SetFogMultOffs((f32)fm, (f32)fo);
#endif

// HW fog, only works for a few games
#if 0
			f32 a = f32(value >> 16);
			f32 b = f32(value & 0xFFFF);

			f32 fog_near = a / 256.0f;
			f32 fog_far = b / 6.0f;

			gRenderer->SetFogMinMax(fog_near, fog_far);
#endif
			break;
		}

		case G_MW_LIGHTCOL:
		{
			u32 field_offset = (offset & 0x7);
			u32 light_idx    = offset >> 5;

			DL_COMMAND("gsSPLightColor(LIGHT_%d, %s);", light_idx, MakeColourTextRGBA(value).c_str());

			if (field_offset == 0)
			{
				// Light col
				u8 r = ((value >> 24) & 0xFF);
				u8 g = ((value >> 16) & 0xFF);
				u8 b = ((value >> 8) & 0xFF);
				gRenderer->SetLightCol(light_idx, r, g, b);
			}
			break;
		}

		case G_MW_POINTS:  // Used in FIFA 98
		{
			u32 vtx   = offset / 40;
			u32 where = offset % 40;
			DL_COMMAND("gsSPModifyVertex(%d, %d, 0x%08x);", vtx, where);
			gRenderer->ModifyVertexInfo(vtx, where, value);
			break;
		}
		case G_MW_PERSPNORM:
		{
			DL_COMMAND("gsSPPerspNormalize(0x%08x);", value);
			break;
		}

		default:
		{
			DL_COMMAND("gMoveWd(%d, %d, 0x%08x)", command.mw1.type, command.mw1.offset, command.mw1.value);
			break;
		}
	}
}

void DLParser_GBI1_CullDL(MicroCodeCommand command)
{
	u32 first = command.culldl.first;
	u32 last = command.culldl.end;

	DL_PF("    Culling using verts %d to %d\n", first, last);

	if (last < first) return;
	if (gRenderer->TestVerts(first, last))
	{
		DL_PF("    Display list is visible, returning");
		return;
	}

#ifdef DAEDALUS_DEBUG_DISPLAYLIST
	++gNumDListsCulled;
#endif

	DL_PF("    No vertices were visible, culling rest of display list");

	DLParser_PopDL();
}

void DLParser_GBI1_DL(MicroCodeCommand command)
{
	DAEDALUS_ASSERT(RDPSegAddr(command.dlist.addr) < MAX_RAM_ADDRESS, "Dlist address out of range");
	DAEDALUS_ASSERT(gDlistStackPointer < 9, "Dlist array is getting too deep");

	DL_COMMAND("%s(0x%08x);",
		(command.dlist.param == G_DL_PUSH) ? "gsSPDisplayList" : "gsSPBranchList",
		RDPSegAddr(command.dlist.addr));

	if (command.dlist.param == G_DL_PUSH)
	{
		DLParser_PushDisplayList();
	}
	DLParser_SetDisplayList(RDPSegAddr(command.dlist.addr));
}

void DLParser_GBI1_EndDL(MicroCodeCommand command)
{
    DL_COMMAND("gsSPEndDisplayList();");

	DLParser_PopDL();
}

// When the depth is less than the z value provided, branch to given address
void DLParser_GBI1_BranchZ(MicroCodeCommand command)
{
	// Always branching will usually just waste a bit of fillrate (PSP got plenty)
	// Games seem not to bother if we branch less than Z all the time

	// Penny racers (cars)
	// Aerogauge (skips rendering ship shadows and exaust plumes from afar)
	// OOT : Death Mountain and MM : Clock Town

	// Seems to work differently for non Zelda games as if Z axis is inverted... //Corn

	// printf("VtxDepth[%d] Zval[%d] Vtx[%d]\n", gRenderer->GetVtxDepth(command.branchz.vtx),
	// (s32)command.branchz.value, command.branchz.vtx);  DL_PF("BranchZ VtxDepth[%d] Zval[%d] Vtx[%d]",
	// gRenderer->GetVtxDepth(command.branchz.vtx), (s32)command.branchz.value, command.branchz.vtx);

	if (gRenderer->GetVtxDepth(command.branchz.vtx) <= (s32)command.branchz.value)
	{
		u32 address = RDPSegAddr(gRDPHalf1);

		DL_PF("    Jump -> DisplayList 0x%08x", address);

		gDlistStack.address[gDlistStackPointer] = address;
	}
}

// AST, Yoshi's World, Scooby Doo use this
void DLParser_GBI1_LoadUCode(MicroCodeCommand command)
{
	u32 code_base = (command.inst.cmd1 & 0x1fffffff);
	u32 code_size = 0x1000;
	u32 data_base = gRDPHalf1 & 0x1fffffff;  // Preceeding RDP_HALF1 sets this up
	u32 data_size = (command.inst.cmd0 & 0xFFFF) + 1;

	DLParser_InitMicrocode(code_base, code_size, data_base, data_size);
}

std::string MakeGBI1GeometryModeFlagsText(u32 data)
{
	std::vector<std::string> flags;

	if (data & G_ZBUFFER)			flags.push_back("G_ZBUFFER");
	if (data & G_TEXTURE_ENABLE)	flags.push_back("G_TEXTURE_ENABLE");
	if (data & G_SHADE)				flags.push_back("G_SHADE");
	if (data & G_SHADING_SMOOTH)	flags.push_back("G_SHADING_SMOOTH");

	u32 cull = data & G_CULL_BOTH;
	if (cull == G_CULL_FRONT)		flags.push_back("G_CULL_FRONT");
	else if (cull == G_CULL_BACK)	flags.push_back("G_CULL_BACK");
	else if (cull == G_CULL_BOTH)	flags.push_back("G_CULL_BOTH");

	if (data & G_FOG)					flags.push_back("G_FOG");
	if (data & G_LIGHTING)				flags.push_back("G_LIGHTING");
	if (data & G_TEXTURE_GEN)			flags.push_back("G_TEXTURE_GEN");
	if (data & G_TEXTURE_GEN_LINEAR)	flags.push_back("G_TEXTURE_GEN_LINEAR");
	if (data & G_LOD)					flags.push_back("G_LOD");

	if (flags.empty())
	{
		return "0";
	}
	return absl::StrJoin(flags, "|");
}

void DLParser_GBI1_GeometryMode(MicroCodeCommand command)
{
	const u32 mask = command.inst.cmd1;

	if (command.inst.cmd & 1)
	{
		gGeometryMode._u32 |= mask;
		DL_COMMAND("gsSPSetGeometryMode(%s);", MakeGBI1GeometryModeFlagsText(mask).c_str());
	}
	else
	{
		gGeometryMode._u32 &= ~mask;
		DL_COMMAND("gsSPClearGeometryMode(%s);", MakeGBI1GeometryModeFlagsText(mask).c_str());
	}

	TnLMode TnL;
	TnL._u32 = 0;

	TnL.Light = gGeometryMode.GBI1_Lighting;
	TnL.TexGen = gGeometryMode.GBI1_TexGen;
	TnL.TexGenLin = gGeometryMode.GBI1_TexGenLin;
	TnL.Fog = gGeometryMode.GBI1_Fog & gFogEnabled;  // && (gRDPOtherMode.c1_m1a==3 || gRDPOtherMode.c1_m2a==3 ||
													 // gRDPOtherMode.c2_m1a==3 || gRDPOtherMode.c2_m2a==3);
	TnL.Shade = gGeometryMode.GBI1_Shade /* & gGeometryMode.GBI1_ShadingSmooth*/;
	TnL.Zbuffer = gGeometryMode.GBI1_Zbuffer;

	// CULL_BACK has priority, Fixes Mortal Kombat 4
	TnL.TriCull = gGeometryMode.GBI1_CullFront | gGeometryMode.GBI1_CullBack;
	TnL.CullBack = gGeometryMode.GBI1_CullBack;

	gRenderer->SetTnLMode(TnL._u32);
}

void DLParser_GBI1_SetOtherModeL(MicroCodeCommand command)
{
	const u32 mask = ((1 << command.othermode.len) - 1) << command.othermode.sft;

	gRDPOtherMode.L = (gRDPOtherMode.L & ~mask) | command.othermode.data;

#ifdef DAEDALUS_DEBUG_DISPLAYLIST
	DLDebug_DumpRDPOtherModeL(mask, command.othermode.data);
#endif
}

void DLParser_GBI1_SetOtherModeH(MicroCodeCommand command)
{
	const u32 mask = ((1 << command.othermode.len) - 1) << command.othermode.sft;

	gRDPOtherMode.H = (gRDPOtherMode.H & ~mask) | command.othermode.data;

#ifdef DAEDALUS_DEBUG_DISPLAYLIST
	DLDebug_DumpRDPOtherModeH(mask, command.othermode.data);
#endif
}

void DLParser_GBI1_Texture(MicroCodeCommand command)
{
	DL_COMMAND("gsSPTexture(%f, %f, %d, %s, %s);",
		command.texture.scaleS / 65535.0f,
		command.texture.scaleT / 65535.0f,
		command.texture.level,
		GetTileIdxText(command.texture.tile).c_str(),
		command.texture.enable_gbi0 ? "G_ON" : "G_OFF");

	gRenderer->SetTextureTile(command.texture.tile);
	gRenderer->SetTextureEnable(command.texture.enable_gbi0);

	f32 scale_s = f32(command.texture.scaleS) / (65535.0f * 32.0f);
	f32 scale_t = f32(command.texture.scaleT) / (65535.0f * 32.0f);
	gRenderer->SetTextureScale(scale_s, scale_t);
}

void DLParser_GBI1_Reserved(MicroCodeCommand command)
{
	// Not implemented!
	DL_UNIMPLEMENTED_ERROR("RDP: Reserved");
}

void DLParser_GBI1_Noop(MicroCodeCommand command)
{
	DL_COMMAND("gsDPNoOp();");
}

void DLParser_GBI1_SpNoop(MicroCodeCommand command)
{
	DL_COMMAND("gsSPNoOp();");
}

void DLParser_GBI1_RDPHalf_Cont(MicroCodeCommand command)
{
	// Console_Print("Unexpected RDPHalf_Cont: %08x %08x", command.inst.cmd0, command.inst.cmd1);
}

void DLParser_GBI1_RDPHalf_2(MicroCodeCommand command)
{
	DL_COMMAND("gsImmp1(G_RDPHALF_2, 0x%08x);", command.inst.cmd1);
}

void DLParser_GBI1_RDPHalf_1(MicroCodeCommand command)
{
	DL_COMMAND("gsImmp1(G_RDPHALF_1, 0x%08x);", command.inst.cmd1);

	gRDPHalf1 = command.inst.cmd1;
}

void DLParser_GBI1_Tri2(MicroCodeCommand command)
{
	// While the next command pair is Tri2, add vertices
	u32 pc = gDlistStack.address[gDlistStackPointer];
	u32* pCmdBase = (u32*)(gu8RamBase + pc);

	bool tris_added = false;

	do
	{
		// DL_PF("    0x%08x: %08x %08x %-10s", pc-8, command.inst.cmd0, command.inst.cmd1, "G_GBI1_TRI2");

		// Vertex indices are multiplied by 10 for GBI0, by 2 for GBI1
		u32 v0_idx = command.gbi1tri2.v0 >> 1;
		u32 v1_idx = command.gbi1tri2.v1 >> 1;
		u32 v2_idx = command.gbi1tri2.v2 >> 1;

		tris_added |= gRenderer->AddTri(v0_idx, v1_idx, v2_idx);

		u32 v3_idx = command.gbi1tri2.v3 >> 1;
		u32 v4_idx = command.gbi1tri2.v4 >> 1;
		u32 v5_idx = command.gbi1tri2.v5 >> 1;

		tris_added |= gRenderer->AddTri(v3_idx, v4_idx, v5_idx);

		command.inst.cmd0 = *pCmdBase++;
		command.inst.cmd1 = *pCmdBase++;
		pc += 8;
	} while (command.inst.cmd == G_GBI1_TRI2);

	gDlistStack.address[gDlistStackPointer] = pc - 8;

	if (tris_added)
	{
		gRenderer->FlushTris();
	}
}

void DLParser_GBI1_Line3D(MicroCodeCommand command)
{
	if (command.gbi1line3d.v3 == 0)
	{
		// This removes the tris that cover the screen in Flying Dragon
		// Actually this wrong, we should support line3D properly here..
		DAEDALUS_ERROR("Flying Dragon Hack -- Skipping Line3D");
		return;
	}

	// While the next command pair is Tri1, add vertices
	u32 pc = gDlistStack.address[gDlistStackPointer];
	u32 stride = gVertexStride;
	u32* pCmdBase = (u32*)(gu8RamBase + pc);

	bool tris_added = false;

	do
	{
		u32 v0_idx = command.gbi1line3d.v0 / stride;
		u32 v1_idx = command.gbi1line3d.v1 / stride;
		u32 v2_idx = command.gbi1line3d.v2 / stride;
		u32 v3_idx = command.gbi1line3d.v3 / stride;

		bool a_added = gRenderer->AddTri(v0_idx, v1_idx, v2_idx);
		bool b_added = gRenderer->AddTri(v2_idx, v3_idx, v0_idx);

		tris_added |= a_added | b_added;

		DL_COMMAND("gsSPLine3D(%d, %d, %d, %d);", v0_idx, v1_idx, v2_idx, v3_idx);
		DL_NOTE("%s",
			(a_added && b_added) ? "both accepted" :
			(a_added) ? "rejected" :
			(b_added) ? "a rejected" :
			"both rejected");

		command.inst.cmd0 = *pCmdBase++;
		command.inst.cmd1 = *pCmdBase++;
		pc += 8;
	} while (command.inst.cmd == G_GBI1_LINE3D && !DL_ACTIVE());

	gDlistStack.address[gDlistStackPointer] = pc - 8;

	if (tris_added)
	{
		gRenderer->FlushTris();
	}
}

void DLParser_GBI1_Tri1(MicroCodeCommand command)
{
	// DAEDALUS_PROFILE( "DLParser_GBI1_Tri1_T" );
	// While the next command pair is Tri1, add vertices
	u32 pc = gDlistStack.address[gDlistStackPointer];
	u32 stride = gVertexStride;
	u32* pCmdBase = (u32*)(gu8RamBase + pc);

	bool tris_added = false;

	do
	{
		u32 v0_idx = command.gbi1tri1.v0 / stride;
		u32 v1_idx = command.gbi1tri1.v1 / stride;
		u32 v2_idx = command.gbi1tri1.v2 / stride;

		bool added = gRenderer->AddTri(v0_idx, v1_idx, v2_idx);
		tris_added |= added;

		DL_COMMAND("gsSP1Triangle(%d, %d, %d, %d);", v0_idx, v1_idx, v2_idx, command.gbi1tri1.flag);
		DL_NOTE(added ? "accepted" : "rejected");

		command.inst.cmd0 = *pCmdBase++;
		command.inst.cmd1 = *pCmdBase++;
		pc += 8;
	} while (command.inst.cmd == G_GBI1_TRI1 && !DL_ACTIVE());

	gDlistStack.address[gDlistStackPointer] = pc - 8;

	if (tris_added)
	{
		gRenderer->FlushTris();
	}
}

#endif  // HLEGRAPHICS_UCODES_UCODE_GBI1_H_
