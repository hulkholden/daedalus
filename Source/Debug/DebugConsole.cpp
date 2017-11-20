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

#include "Base/Daedalus.h"
#include "Debug/Console.h"

#ifdef DAEDALUS_DEBUG_CONSOLE

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "absl/strings/string_view.h"

static const char* const kDefaultColour = "\033[0m";
static const char* const kColour_r = "\033[0;31m";
static const char* const kColour_g = "\033[0;32m";
static const char* const kColour_y = "\033[0;33m";
static const char* const kColour_b = "\033[0;34m";
static const char* const kColour_m = "\033[0;35m";
static const char* const kColour_c = "\033[0;36m";
static const char* const kColour_w = "\033[0;37m";
static const char* const kColour_R = "\033[1;31m";
static const char* const kColour_G = "\033[1;32m";
static const char* const kColour_Y = "\033[1;33m";
static const char* const kColour_B = "\033[1;34m";
static const char* const kColour_M = "\033[1;35m";
static const char* const kColour_C = "\033[1;36m";
static const char* const kColour_W = "\033[1;37m";

static const char* GetTerminalColourString(char c)
{
	switch (c)
	{
		case 'r':
			return kColour_r;
		case 'g':
			return kColour_g;
		case 'y':
			return kColour_y;
		case 'b':
			return kColour_b;
		case 'm':
			return kColour_m;
		case 'c':
			return kColour_c;
		case 'w':
			return kColour_w;
		case 'R':
			return kColour_R;
		case 'G':
			return kColour_G;
		case 'Y':
			return kColour_Y;
		case 'B':
			return kColour_B;
		case 'M':
			return kColour_M;
		case 'C':
			return kColour_C;
		case 'W':
			return kColour_W;
	}

	return "";
}

struct ParseFormatState
{
	char* output;
	size_t len;

	void Append(absl::string_view str)
	{
		len += str.length();
		if (output)
		{
			memcpy(output, str.data(), str.length());
			output += str.length();
		}
	}

	void Append(char c)
	{
		len++;
		if (output)
		{
			*output = c;
			output++;
		}
	}
};

static size_t ParseFormatString(const char *format, char *out)
{
	bool colour_active = false;
	ParseFormatState state = {out, 0};

	for (const char *p = format; *p; ++p)
	{
		if (*p == '[')
		{
			++p;
			colour_active = true;
			state.Append(GetTerminalColourString(*p));
		}
		else if (*p == ']')
		{
			colour_active = false;
			state.Append(kDefaultColour);
		}
		else
		{
			state.Append(*p);
		}
	}

	if (colour_active)
	{
		state.Append(kDefaultColour);
	}

	if (state.output)
	{
		const char nil = '\0';
		state.Append(nil);
		DAEDALUS_ASSERT(state.output == out + state.len, "Oops");
	}

	return state.len;
}

static void ApplyStyleAndPrint(const char *format, va_list args)
{
	char *temp = nullptr;

	if (strchr(format, '[') != nullptr)
	{
		size_t len = ParseFormatString(format, nullptr);
		temp = (char *)malloc(len + 1);
		ParseFormatString(format, temp);
		format = temp;
	}

	vprintf(format, args);
	printf("\n");

	if (temp) free(temp);
}

void DAEDALUS_VARARG_CALL_TYPE Console_Print(const char *format, ...)
{
	va_list marker;
	va_start(marker, format);
	ApplyStyleAndPrint(format, marker);
	va_end(marker);
}

void Console_OverwriteStart()
{
}

void DAEDALUS_VARARG_CALL_TYPE Console_Overwrite(const char *format, ...)
{
	va_list marker;
	va_start(marker, format);
	ApplyStyleAndPrint(format, marker);
	va_end(marker);
}

void Console_OverwriteEnd()
{
}

void Console_Flush()
{
	fflush(stdout);
}

#endif
