#include "Base/Daedalus.h"
#include "HLEGraphics/DLDebug.h"

#ifdef DAEDALUS_DEBUG_DISPLAYLIST

#include <stdarg.h>

#include "absl/strings/str_cat.h"

#include "Base/Macros.h"
#include "Core/ROM.h"
#include "Debug/Console.h"
#include "Debug/Dump.h"
#include "HLEGraphics/RDP.h"
#include "System/IO.h"
#include "Ultra/ultra_gbi.h"


DLDebugOutput * gDLDebugOutput = NULL;

void DLDebug_SetOutput(DLDebugOutput * output)
{
    gDLDebugOutput = output;
}

DLDebugOutput::~DLDebugOutput()
{
}

void DLDebugOutput::PrintLine(const char * fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    // I've never been confident that this returns a sane value across platforms.
    /*len = */vsnprintf( mBuffer, kBufferLen, fmt, va );
    va_end(va);

    // This should be guaranteed...
    mBuffer[kBufferLen-1] = 0;
    size_t len = strlen(mBuffer);

    // Append a newline, if there's space in the buffer.
    if (len < kBufferLen)
    {
        mBuffer[len] = '\n';
        ++len;
    }

    Write(mBuffer, len);
}

void DLDebugOutput::Print(const char * fmt, ...)
{
    //char buffer[kBufferLen];

    va_list va;
    va_start(va, fmt);
    // I've never been confident that this returns a sane value across platforms.
    /*len = */vsnprintf( mBuffer, kBufferLen, fmt, va );
    va_end(va);


    // This should be guaranteed...
    mBuffer[kBufferLen-1] = 0;
    size_t len = strlen(mBuffer);

    Write(mBuffer, len);
}

// TODO(strmnnrmn): Dedupe the body.
void DLDebugOutput::AddNote(const char* fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    // I've never been confident that this returns a sane value across platforms.
    /*len = */vsnprintf( mBuffer, kBufferLen, fmt, va );
    va_end(va);

    // This should be guaranteed...
    mBuffer[kBufferLen-1] = 0;
    size_t len = strlen(mBuffer);

    // Append a newline, if there's space in the buffer.
    if (len < kBufferLen)
    {
        mBuffer[len] = '\n';
        ++len;
    }

    //Write(mBuffer, len);
}

std::string MakeColourTextRGB(u8 r, u8 g, u8 b)
{
    std::string rgb = absl::StrCat(r, ", ", g, ", ", b);

    if ((r < 128 && g < 128) ||
        (g < 128 && b < 128) ||
        (b < 128 && r < 128))
    {
        return absl::StrCat("<span style='color: white; background-color: rgb(", rgb, ")'>", rgb, "</span>");
    }
    return absl::StrCat("<span style='background-color: rgb(", rgb, ")'>", rgb, "</span>");
}

std::string MakeColourTextRGBA(u8 r, u8 g, u8 b, u8 a)
{
    std::string rgb = absl::StrCat(r, ", ", g, ", ", b);
    std::string rgba = absl::StrCat(rgb, ", ", a);

    if ((r < 128 && g < 128) ||
        (g < 128 && b < 128) ||
        (b < 128 && r < 128))
    {
        return absl::StrCat("<span style='color: white; background-color: rgb(", rgb, ")'>", rgba, "</span>");
    }
    return absl::StrCat("<span style='background-color: rgb(", rgb, ")'>", rgba, "</span>");
}

std::string MakeColourTextRGBA(u32 fill_colour)
{
    u8 r = (fill_colour >> 24) & 0xff;
    u8 g = (fill_colour >> 16) & 0xff;
    u8 b = (fill_colour >>  8) & 0xff;
    u8 a = (fill_colour >>  0) & 0xff;

    return MakeColourTextRGBA(r, g, b, a);
}

static const char * const kBlendColourSources[] = {
  "G_BL_CLR_IN",
  "G_BL_CLR_MEM",
  "G_BL_CLR_BL",
  "G_BL_CLR_FOG",
};

static const char * const kBlendSourceFactors[] = {
  "G_BL_A_IN",
  "G_BL_A_FOG",
  "G_BL_A_SHADE",
  "G_BL_0",
};

static const char * const kBlendDestFactors[] = {
  "G_BL_1MA",
  "G_BL_A_MEM",
  "G_BL_1",
  "G_BL_0",
};

static const char * const kMulInputRGB[32] =
{
    "Combined    ", "Texel0      ",
    "Texel1      ", "Primitive   ",
    "Shade       ", "Env         ",
    "KeyScale    ", "CombinedAlph",
    "Texel0_Alpha", "Texel1_Alpha",
    "Prim_Alpha  ", "Shade_Alpha ",
    "Env_Alpha   ", "LOD_Frac    ",
    "PrimLODFrac ", "K5          ",
    "0           ", "0           ",
    "0           ", "0           ",
    "0           ", "0           ",
    "0           ", "0           ",
    "0           ", "0           ",
    "0           ", "0           ",
    "0           ", "0           ",
    "0           ", "0           "
};

static const char * const kSubAInputRGB[16] =
{
    "Combined    ", "Texel0      ",
    "Texel1      ", "Primitive   ",
    "Shade       ", "Env         ",
    "1           ", "Noise       ",
    "0           ", "0           ",
    "0           ", "0           ",
    "0           ", "0           ",
    "0           ", "0           ",
};

static const char * const kSubBInputRGB[16] =
{
    "Combined    ", "Texel0      ",
    "Texel1      ", "Primitive   ",
    "Shade       ", "Env         ",
    "KeyCenter   ", "K4          ",
    "0           ", "0           ",
    "0           ", "0           ",
    "0           ", "0           ",
    "0           ", "0           ",
};

static const char * const kAddInputRGB[8] =
{
    "Combined    ", "Texel0      ",
    "Texel1      ", "Primitive   ",
    "Shade       ", "Env         ",
    "1           ", "0           ",
};

static const char * const kSubInputAlpha[8] =
{
    "Combined    ", "Texel0      ",
    "Texel1      ", "Primitive   ",
    "Shade       ", "Env         ",
    "1           ", "0           ",
};

static const char * const kMulInputAlpha[8] =
{
    "LOD_Frac    ", "Texel0      ",
    "Texel1      ", "Primitive   ",
    "Shade       ", "Env         ",
    "PrimLOD_Frac", "0           ",
};

static const char * const kAddInputAlpha[8] =
{
    "Combined    ", "Texel0      ",
    "Texel1      ", "Primitive   ",
    "Shade       ", "Env         ",
    "1           ", "0           ",
};


void DLDebug_DumpMux( u64 mux )
{
    if (!DLDebug_IsActive())
        return;

    u32 mux0 = (u32)(mux>>32);
    u32 mux1 = (u32)(mux);

    u32 aRGB0  = (mux0>>20)&0x0F;   // c1 c1        // a0
    u32 bRGB0  = (mux1>>28)&0x0F;   // c1 c2        // b0
    u32 cRGB0  = (mux0>>15)&0x1F;   // c1 c3        // c0
    u32 dRGB0  = (mux1>>15)&0x07;   // c1 c4        // d0

    u32 aA0    = (mux0>>12)&0x07;   // c1 a1        // Aa0
    u32 bA0    = (mux1>>12)&0x07;   // c1 a2        // Ab0
    u32 cA0    = (mux0>>9 )&0x07;   // c1 a3        // Ac0
    u32 dA0    = (mux1>>9 )&0x07;   // c1 a4        // Ad0

    u32 aRGB1  = (mux0>>5 )&0x0F;   // c2 c1        // a1
    u32 bRGB1  = (mux1>>24)&0x0F;   // c2 c2        // b1
    u32 cRGB1  = (mux0    )&0x1F;   // c2 c3        // c1
    u32 dRGB1  = (mux1>>6 )&0x07;   // c2 c4        // d1

    u32 aA1    = (mux1>>21)&0x07;   // c2 a1        // Aa1
    u32 bA1    = (mux1>>3 )&0x07;   // c2 a2        // Ab1
    u32 cA1    = (mux1>>18)&0x07;   // c2 a3        // Ac1
    u32 dA1    = (mux1    )&0x07;   // c2 a4        // Ad1

    DL_COMMAND("gsDPSetCombine(0x%08x, 0%08x);", mux0, mux1);

    DL_NOTE("RGB0 = (%s - %s) * %s + %s", kSubAInputRGB[aRGB0], kSubBInputRGB[bRGB0], kMulInputRGB[cRGB0], kAddInputRGB[dRGB0]);
    DL_NOTE("A0   = (%s - %s) * %s + %s", kSubInputAlpha[aA0],  kSubInputAlpha[bA0],  kMulInputAlpha[cA0], kAddInputAlpha[dA0]);
    DL_NOTE("RGB1 = (%s - %s) * %s + %s", kSubAInputRGB[aRGB1], kSubBInputRGB[bRGB1], kMulInputRGB[cRGB1], kAddInputRGB[dRGB1]);
    DL_NOTE("A1   = (%s - %s) * %s + %s", kSubInputAlpha[aA1],  kSubInputAlpha[bA1],  kMulInputAlpha[cA1], kAddInputAlpha[dA1]);
}



static const char * const kAlphaCompareValues[]     = {"None", "Threshold", "?", "Dither"};
static const char * const kDepthSourceValues[]      = {"Pixel", "Primitive"};

static const char * const kAlphaDitherValues[]      = {"Pattern", "NotPattern", "Noise", "Disable"};
static const char * const kRGBDitherValues[]        = {"MagicSQ", "Bayer", "Noise", "Disable"};
static const char * const kCombKeyValues[]          = {"None", "Key"};
static const char * const kTextureConvValues[]      = {"Conv", "?", "?", "?",   "?", "FiltConv", "Filt", "?"};
static const char * const kTextureFilterValues[]    = {"Point", "?", "Bilinear", "Average"};
static const char * const kTextureLUTValues[]       = {"None", "?", "RGBA16", "IA16"};
static const char * const kTextureLODValues[]       = {"Tile", "LOD"};
static const char * const kTextureDetailValues[]    = {"Clamp", "Sharpen", "Detail", "?"};
static const char * const kCycleTypeValues[]        = {"1Cycle", "2Cycle", "Copy", "Fill"};
static const char * const kColorDitherValues[]      = {"G_CD_DISABLE", "G_CD_ENABLE"};
static const char * const kPipelineValues[]         = {"NPrimitive", "1Primitive"};

static const char * const kOnOffValues[]            = {"Off", "On"};

struct OtherModeData
{
    const char *            Name;
    u32                     Bits;
    u32                     Shift;
    const char * const *    Values;
    void                    (*Fn)(u32);     // Custom function
};

static void DumpRenderMode(u32 data);

static const OtherModeData kOtherModeLData[] = {
    { "gsDPSetAlphaCompare", 2, G_MDSFT_ALPHACOMPARE,    kAlphaCompareValues },
    { "gsDPSetDepthSource",  1, G_MDSFT_ZSRCSEL,         kDepthSourceValues },
    { "gsDPSetRenderMode",   29, G_MDSFT_RENDERMODE,     NULL, &DumpRenderMode },
};

static const OtherModeData kOtherModeHData[] = {
    { "gsDPSetBlendMask",       4, G_MDSFT_BLENDMASK,    NULL },
    { "gsDPSetAlphaDither",     2, G_MDSFT_ALPHADITHER,  kAlphaDitherValues },
    { "gsDPSetColorDither",     2, G_MDSFT_RGBDITHER,    kRGBDitherValues },
    { "gsDPSetCombineKey",      1, G_MDSFT_COMBKEY,      kCombKeyValues },
    { "gsDPSetTextureConvert",  3, G_MDSFT_TEXTCONV,     kTextureConvValues },
    { "gsDPSetTextureFilter",   2, G_MDSFT_TEXTFILT,     kTextureFilterValues },
    { "gsDPSetTextureLUT",      2, G_MDSFT_TEXTLUT,      kTextureLUTValues },
    { "gsDPSetTextureLOD",      1, G_MDSFT_TEXTLOD,      kTextureLODValues },
    { "gsDPSetTextureDetail",   2, G_MDSFT_TEXTDETAIL,   kTextureDetailValues },
    { "gsDPSetTexturePersp",    1, G_MDSFT_TEXTPERSP,    kOnOffValues },
    { "gsDPSetCycleType",       2, G_MDSFT_CYCLETYPE,    kCycleTypeValues },
    { "gsDPSetColorDither",     1, G_MDSFT_COLORDITHER,  kColorDitherValues },
    { "gsDPPipelineMode",       1, G_MDSFT_PIPELINE,     kPipelineValues },
};

static void DumpOtherMode(const OtherModeData * table, u32 table_len, u32 * mask_, u32 * data_)
{
    u32 mask = *mask_;
    u32 data = *data_;

    for (u32 i = 0; i < table_len; ++i)
    {
        const OtherModeData & e = table[i];

        u32 mode_mask = ((1 << e.Bits)-1) << e.Shift;
        if ((mask & mode_mask) != mode_mask)
            continue;

        u32 val = (data & mode_mask) >> e.Shift;

        if (e.Values)
        {
            DL_COMMAND("%s(%s);", e.Name, e.Values[val]);
        }
        else if (e.Fn)
        {
            e.Fn(data); // NB pass unshifted value.
        }
        else
        {
            DL_COMMAND("%s(0x%08x);", e.Name, val);
        }

        mask &= ~mode_mask;
        data &= ~mode_mask;
    }

    *mask_ = mask;
    *data_ = data;
}

std::string BlendOpText(u32 v)
{
	u32 m1a = (v >> 12) & 0x3;
	u32 m1b = (v >>  8) & 0x3;
	u32 m2a = (v >>  4) & 0x3;
	u32 m2b = (v >>  0) & 0x3;

	return absl::StrCat(kBlendColourSources[m1a], ",", kBlendSourceFactors[m1b], ",",
                        kBlendColourSources[m2a], ",", kBlendDestFactors[m2b]);
}

// Slightly nicer output of rendermode, as a single line
static void DumpRenderMode(u32 data)
{
    std::string buf;
    if (data & AA_EN)               absl::StrAppend(&buf, "|AA_EN");
    if (data & Z_CMP)               absl::StrAppend(&buf, "|Z_CMP");
    if (data & Z_UPD)               absl::StrAppend(&buf, "|Z_UPD");
    if (data & IM_RD)               absl::StrAppend(&buf, "|IM_RD");
    if (data & CLR_ON_CVG)          absl::StrAppend(&buf, "|CLR_ON_CVG");

    u32 cvg = data & 0x0300;
         if (cvg == CVG_DST_CLAMP)  absl::StrAppend(&buf, "|CVG_DST_CLAMP");
    else if (cvg == CVG_DST_WRAP)   absl::StrAppend(&buf, "|CVG_DST_WRAP");
    else if (cvg == CVG_DST_FULL)   absl::StrAppend(&buf, "|CVG_DST_FULL");
    else if (cvg == CVG_DST_SAVE)   absl::StrAppend(&buf, "|CVG_DST_SAVE");

    u32 zmode = data & 0x0c00;
         if (zmode == ZMODE_OPA)    absl::StrAppend(&buf, "|ZMODE_OPA");
    else if (zmode == ZMODE_INTER)  absl::StrAppend(&buf, "|ZMODE_INTER");
    else if (zmode == ZMODE_XLU)    absl::StrAppend(&buf, "|ZMODE_XLU");
    else if (zmode == ZMODE_DEC)    absl::StrAppend(&buf, "|ZMODE_DEC");

    if (data & CVG_X_ALPHA)         absl::StrAppend(&buf, "|CVG_X_ALPHA");
    if (data & ALPHA_CVG_SEL)       absl::StrAppend(&buf, "|ALPHA_CVG_SEL");
    if (data & FORCE_BL)            absl::StrAppend(&buf, "|FORCE_BL");

    if (buf.empty()) {
        absl::StrAppend(&buf, "|0");
    }
    const char * p = buf.c_str() + 1; // Skip the first '|'

    u32 blend = data >> G_MDSFT_BLENDER;
	std::string blend_txt =
	    absl::StrCat("GBL_c1(", BlendOpText(blend >> 2), ") |  ",
                     "GBL_c2(", BlendOpText(blend), ") /*", absl::Hex(blend), "*/");

	DL_COMMAND("gsDPSetRenderMode(%s, %s);", p, blend_txt.c_str());
}

void DLDebug_DumpRDPOtherMode(const RDP_OtherMode & mode)
{
    if (!DLDebug_IsActive())
        return;

    u32 mask = 0xffffffff;
    u32 data = mode.L;
    DumpOtherMode(kOtherModeLData, ARRAYSIZE(kOtherModeLData), &mask, &data);

    mask = 0xffffffff;
    data = mode.H;
    DumpOtherMode(kOtherModeHData, ARRAYSIZE(kOtherModeHData), &mask, &data);
}

void DLDebug_DumpRDPOtherModeL(u32 mask, u32 data)
{
    if (!DLDebug_IsActive())
        return;

    DumpOtherMode(kOtherModeLData, ARRAYSIZE(kOtherModeLData), &mask, &data);

    // Just check we're not handling some unusual calls.
    DAEDALUS_ASSERT(mask == 0, "OtherModeL mask is non zero: %08x", mask);
    DAEDALUS_ASSERT(data == 0, "OtherModeL data is non zero: %08x", data);
}

void DLDebug_DumpRDPOtherModeH(u32 mask, u32 data)
{
    if (!DLDebug_IsActive())
        return;

    DumpOtherMode(kOtherModeHData, ARRAYSIZE(kOtherModeHData), &mask, &data);

    // Just check we're not handling some unusual calls.
    DAEDALUS_ASSERT(mask == 0, "OtherModeH mask is non zero: %08x", mask);
    DAEDALUS_ASSERT(data == 0, "OtherModeH data is non zero: %08x", data);
}

void DLDebug_DumpTaskInfo(const OSTask* pTask)
{
	DL_NOTE("Task:         %08x", pTask->t.type);
	DL_NOTE("Flags:        %08x", pTask->t.flags);
	DL_NOTE("BootCode:     %08x", pTask->t.ucode_boot);
	DL_NOTE("BootCodeSize: %08x", pTask->t.ucode_boot_size);

	DL_NOTE("uCode:        %08x", pTask->t.ucode);
	DL_NOTE("uCodeSize:    %08x", pTask->t.ucode_size);
	DL_NOTE("uCodeData:    %08x", pTask->t.ucode_data);
	DL_NOTE("uCodeDataSize:%08x", pTask->t.ucode_data_size);

	DL_NOTE("Stack:        %08x", pTask->t.dram_stack);
	DL_NOTE("StackS:       %08x", pTask->t.dram_stack_size);
	DL_NOTE("Output:       %08x", pTask->t.output_buff);
	DL_NOTE("OutputS:      %08x", pTask->t.output_buff_size);

	DL_NOTE("Data( PC ):   %08x", pTask->t.data_ptr);
	DL_NOTE("DataSize:     %08x", pTask->t.data_size);
	DL_NOTE("YieldData:    %08x", pTask->t.yield_data_ptr);
	DL_NOTE("YieldDataSize:%08x", pTask->t.yield_data_size);
}

#endif // DAEDALUS_DEBUG_DISPLAYLIST
