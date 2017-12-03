#pragma once

#ifndef UTILITY_PROFILER_H_
#define UTILITY_PROFILER_H_

#ifdef DAEDALUS_ENABLE_PROFILING

#include "external/remotery/lib/Remotery.h"

#define DAEDALUS_PROFILE(x)              \
        rmt_BeginCPUSampleDynamic(x, RMTSF_Aggregate);  \
        rmt_EndCPUSampleOnScopeExit rmt_ScopedCPUSampleXXX;

#else

#define DAEDALUS_PROFILE(x)

#endif

#endif  // UTILITY_PROFILER_H_
