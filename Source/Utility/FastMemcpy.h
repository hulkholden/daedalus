
#ifndef UTILITY_FASTMEMCPY_H_
#define UTILITY_FASTMEMCPY_H_

#include <stdlib.h>

// Define to profile memcpys (PSP only!)
//#define PROFILE_MEMCPY

#ifdef PROFILE_MEMCPY
void memcpy_test(void* dst, const void* src, size_t size);
#endif

void memcpy_byteswap(void* dst, const void* src, size_t size);  // Little endian, platform independent, ALWAYS swaps.

// memcpy_swizzle is just a regular memcpy on big-endian targets.
#if (DAEDALUS_ENDIAN_MODE == DAEDALUS_ENDIAN_BIG)
#define memcpy_swizzle memcpy
#elif (DAEDALUS_ENDIAN_MODE == DAEDALUS_ENDIAN_LITTLE)
#define memcpy_swizzle memcpy_byteswap
#else
#error No DAEDALUS_ENDIAN_MODE specified
#endif

#endif  // UTILITY_FASTMEMCPY_H_
