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
#include "Base/Assert.h"

#include <execinfo.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef DAEDALUS_ENABLE_ASSERTS

DaedalusAssertHook gAssertHook = nullptr;

EAssertResult DaedalusAssert(const char* expression, const char* file, unsigned int line, const char* msg, ...)
{
	char buffer[1024];
	va_list va;
	va_start(va, msg);
	vsnprintf(buffer, 1024, msg, va);
	buffer[1023] = 0;
	va_end(va);

	printf("************************************************************\n");
	printf("%s:%d: Assert Failed: %s\n", file, line, expression);
	printf("\n");
	printf("%s\n", buffer);
	printf("\n");

	bool done = false;
	while (!done)
	{
		printf("a: abort, b: break, c: continue, i: ignore\n");
		switch (getchar())
		{
			case EOF:
				// If getchar() fails, treat this the same as 'a'.
				abort();
				return AR_BREAK;  // Should be unreachable.
			case 'a':
				abort();
				return AR_BREAK;  // Should be unreachable.
			case 'b':
				return AR_BREAK;
			case 'c':
				return AR_IGNORE_ONCE;
			case 'i':
				return AR_IGNORE;
		}
	}

	return AR_IGNORE;
}

#endif  // DAEDALUS_ENABLE_ASSERTS

// See https://oroboro.com/stack-trace-on-crash/
static inline void PrintStackTrace(FILE* out = stderr, unsigned int max_frames = 63)
{
	fprintf(out, "stack trace:\n");

	void* addrlist[max_frames + 1];
	u32 addrlen = backtrace(addrlist, sizeof(addrlist) / sizeof(void*));

	if (addrlen == 0)
	{
		fprintf(out, "  \n");
		return;
	}

	// create readable strings to each frame.
	char** symbollist = backtrace_symbols(addrlist, addrlen);
	for (u32 i = 4; i < addrlen; i++) fprintf(out, "%s\n", symbollist[i]);

	free(symbollist);
}

void AbortHandler(int signum, siginfo_t* si, void* unused)
{
	// associate each signal with a signal name string.
	const char* name = nullptr;
	switch (signum)
	{
		case SIGABRT:
			name = "SIGABRT";
			break;
		case SIGSEGV:
			name = "SIGSEGV";
			break;
		case SIGBUS:
			name = "SIGBUS";
			break;
		case SIGILL:
			name = "SIGILL";
			break;
		case SIGFPE:
			name = "SIGFPE";
			break;
	}

	if (name)
	{
		fprintf(stderr, "Caught signal %d (%s)\n", signum, name);
	}
	else
	{
		fprintf(stderr, "Caught signal %d\n", signum);
	}

	PrintStackTrace();
	exit(signum);
}

void InstallAbortHandlers()
{
	struct sigaction sa;
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = AbortHandler;
	sigemptyset(&sa.sa_mask);

	sigaction(SIGABRT, &sa, nullptr);
	sigaction(SIGSEGV, &sa, nullptr);
	sigaction(SIGBUS, &sa, nullptr);
	sigaction(SIGILL, &sa, nullptr);
	sigaction(SIGFPE, &sa, nullptr);
	sigaction(SIGPIPE, &sa, nullptr);
}
