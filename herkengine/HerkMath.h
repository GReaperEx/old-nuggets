// Copyright (C) 2011-2013, GReaperEx(Marios F.)
// This file is part of the "Herakles Engine" project
/* For conditions about distribution and use, consult
   the GNU GPLv3.                                      */

#ifndef __HERK_MATH_H_INCLUDED__
#define __HERK_MATH_H_INCLUDED__

#include "HerkCompileConfig.h"
#include "HerkTypes.h"
#include <cmath>

namespace herk
{

    //! A scalar that all of Herakles Engine will be using
#ifdef _HERK_DOUBLE_PRECISION
    typedef f64 scalar_t;
#else
    typedef f32 scalar_t;
#endif
    //! Rounding error constant often used when comparing f32 values.

	const f32 ROUNDING_ERROR_f32 = 0.000001f;
	const f64 ROUNDING_ERROR_f64 = 0.0000000001;

#ifdef PI // make sure we don't collide with a define
#undef PI
#endif
	//! Constant for PI.
	const f32 PI		= 3.14159265359f;

	//! Constant for reciprocal of PI.
	const f32 RECIPROCAL_PI	= 1.0f/PI;

	//! Constant for half of PI.
	const f32 HALF_PI	= PI/2.0f;

#ifdef PI64 // make sure we don't collide with a define
#undef PI64
#endif
	//! Constant for 64bit PI.
	const f64 PI64		= 3.1415926535897932384626433832795028841971693993751;

	//! Constant for 64bit reciprocal of PI.
	const f64 RECIPROCAL_PI64 = 1.0/PI64;

	//! Constant for 64bit half of PI.
	const f64 HALF_PI64 = PI64/2.0;

	//! 32bit Constant for converting from degrees to radians
	const f32 DEGTORAD = PI / 180.0f;

	//! 32bit constant for converting from radians to degrees (formally known as GRAD_PI)
	const f32 RADTODEG   = 180.0f / PI;

	//! 64bit constant for converting from degrees to radians (formally known as GRAD_PI2)
	const f64 DEGTORAD64 = PI64 / 180.0;

	//! 64bit constant for converting from radians to degrees
	const f64 RADTODEG64 = 180.0 / PI64;

	//! Utility function to convert a radian value to degrees
	static inline f32 radToDeg(f32 radians)
	{
		return RADTODEG * radians;
	}

	//! Utility function to convert a radian value to degrees
	static inline f64 radToDeg(f64 radians)
	{
		return RADTODEG64 * radians;
	}

	//! Utility function to convert a degrees value to radians
	static inline f32 degToRad(f32 degrees)
	{
		return DEGTORAD * degrees;
	}

	//! Utility function to convert a degrees value to radians
	static inline f64 degToRad(f64 degrees)
	{
		return DEGTORAD64 * degrees;
	}

	static inline s32 Sign(s32 value)
	{
	    return -(static_cast<s32>(static_cast<u32>(value) >> 31));
	}

	//! Works only with IEEE-754 floating point numbers
	static inline s32 Sign(f32 value)
    {
        return -(static_cast<s32>(*(reinterpret_cast<u32*>(&value)) >> 31));
    }

    //! Works only with IEEE-754 floating point numbers
    static inline s32 Sign(f64 value)
    {
        return -(static_cast<s32>((reinterpret_cast<u32*>(&value)[1]) >> 31));
    }

	//! returns minimum of two values. Own implementation for convenience
	template<typename T>
	inline const T& Min(const T& a, const T& b)
	{
	    return a < b ? a : b;
	}

	//! returns minimum of three values. Own implementation for convenience
	template<typename T>
	inline const T& Min(const T& a, const T& b, const T& c)
	{
	    return a < b ? Min(a, c) : Min(b, c);
	}

	//! returns maximum of two values. Own implementation for convenience
	template<typename T>
	inline const T& Max(const T& a, const T& b)
	{
	    return a > b ? a : b;
	}

	//! returns maximum of three values. Own implementation for convenience
	template<typename T>
	inline const T& Max(const T& a, const T& b, const T& c)
	{
	    return a > b ? Max(a, c) : Max(b, c);
	}

	//! returns the absolute of a value. Own implementation for convenience
	template<typename T>
	inline const T Abs(const T& a)
	{
	    return a < T(0) ? -a : a;
	}

	//! returns linear interpolation of a and b with ratio t
	template<typename T, typename R>
	inline const T Lerp(const T& a, const T& b, const R& t)
	{
		return T(a + (b - a)*t);
	}

	//! clamps a value between low and high
	template <typename T>
	inline const T Clamp (const T& value, const T& low, const T& high)
	{
		return Min(Max(value,low), high);
	}

	//! swaps the content of the passed parameters
	template <typename T1, typename T2>
	inline void Swap(T1& a, T2& b)
	{
		T1 c(a);
		a = b;
		b = c;
	}

	//! returns if a equals b, taking possible rounding errors into account
	static inline bool equals(const f64 a, const f64 b, const f64 tolerance = ROUNDING_ERROR_f64)
	{
		return fabs(a-b) <= tolerance;
	}

	//! returns if a equals b, taking possible rounding errors into account
	static inline bool equals(const f32 a, const f32 b, const f32 tolerance = ROUNDING_ERROR_f32)
	{
		return fabsf(a-b) <= tolerance;
	}

	static inline f32 SqRt(const f32 f)
	{
        return sqrt((double)f);
	}

	static inline f64 SqRt(const f64 f)
	{
        return sqrt(f);
	}

	template<typename T>
	inline int Sign(const T a)
	{
	    return a < 0 ? -1 : 1;
	}

}


#endif // __HERK_MATH_H_INCLUDED__
