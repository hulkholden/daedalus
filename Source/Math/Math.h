#ifndef MATH_MATH_H_
#define MATH_MATH_H_

#include <math.h>

//ToDo: Use M_PI for x86 platform?
#define PI   3.141592653589793f

#ifdef DAEDALUS_W32
inline f64 trunc(f64 x)				{ return (x>0) ? floor(x) : ceil(x); }
inline f32 truncf(f32 x)			{ return (x>0) ? floorf(x) : ceilf(x); }
inline f64 round(f64 x)				{ return floor(x + 0.5); }
inline f32 roundf(f32 x)			{ return floorf(x + 0.5f); }
#endif

inline void sincosf(float x, float * s, float * c)
{
	*s = sinf(x);
	*c = cosf(x);
}

inline float InvSqrt(float x)
{
	return 1.0f / sqrtf( x );
}

#endif // MATH_MATH_H_
