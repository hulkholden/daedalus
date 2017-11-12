#pragma once

#ifndef BASE_MACROS_H_
#define BASE_MACROS_H_

// Branch prediction.
#ifdef _MSC_VER

#define DAEDALUS_FORCEINLINE __forceinline
#define DAEDALUS_EXPECT_LIKELY(c) (c)
#define DAEDALUS_EXPECT_UNLIKELY(c) (c)
#define DAEDALUS_ATTRIBUTE_NOINLINE

#else

#define DAEDALUS_FORCEINLINE inline __attribute__((always_inline))
#define DAEDALUS_EXPECT_LIKELY(c) __builtin_expect((c), 1)
#define DAEDALUS_EXPECT_UNLIKELY(c) __builtin_expect((c), 0)
#define DAEDALUS_ATTRIBUTE_NOINLINE __attribute__((noinline))

#endif


// Calling conventions.
#ifdef _MSC_VER

// Thread functions need to be __stdcall to work with the W32 api
#define DAEDALUS_THREAD_CALL_TYPE			__stdcall
// Vararg functions need to be __cdecl
#define DAEDALUS_VARARG_CALL_TYPE			__cdecl
// Zlib is compiled as __cdecl
#define	DAEDALUS_ZLIB_CALL_TYPE				__cdecl

#else

#define DAEDALUS_THREAD_CALL_TYPE
#define DAEDALUS_VARARG_CALL_TYPE
#define DAEDALUS_ZLIB_CALL_TYPE

#endif


// Breakpoints.
#ifdef _MSC_VER
#define DAEDALUS_HALT __debugbreak()
#else
#define DAEDALUS_HALT __builtin_trap()
#endif

// Function name introspection.
#ifdef _MSC_VER
#define DAEDALUS_FUNCTION_NAME __FUNCTION__
#else
#define DAEDALUS_FUNCTION_NAME __PRETTY_FUNCTION__
#endif

// Feature detection.
#ifdef _MSC_VER
#define DAEDALUS_COMPILER_HAS_FEATURE(x) 0
#else
#define DAEDALUS_COMPILER_HAS_FEATURE(x) __has_feature(x)
#endif


#ifdef DAEDALUS_ENABLE_ASSERTS

#define NODEFAULT		DAEDALUS_ERROR( "No default - we shouldn't be here" )

#else

#ifdef _MSC_VER
#define NODEFAULT		__assume( 0 )
#else
#define NODEFAULT		//DAEDALUS_EXPECT_LIKELY(1)?
#endif

#endif

#ifndef ARRAYSIZE
#define ARRAYSIZE(arr)   (sizeof(arr) / sizeof(arr[0]))
#endif

#define DAEDALUS_USE(...)	do { (void)sizeof(__VA_ARGS__, 0); } while(0)

#endif // BASE_MACROS_H_
