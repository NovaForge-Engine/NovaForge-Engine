#pragma once


#include <stdint.h>

//======================================================================================================================
// Floating point tricks
//======================================================================================================================

union uFloat
{
	float f;
	uint32_t i;

	 uFloat() : i(0) {}

	 uFloat(float x) : f(x) {}

	 uFloat(uint32_t x) : i(x) {}

	 void abs() { i &= ~(1 << 31); }

	 bool IsNegative() const { return (i >> 31) != 0; }

	 uint32_t Mantissa() const { return i & ((1 << 23) - 1); }

	 uint32_t Exponent() const { return (i >> 23) & 255; }

	 bool IsInf() const
	{
		return Exponent() == 255 && Mantissa() == 0;
	}

	 bool IsNan() const
	{
		return Exponent() == 255 && Mantissa() != 0;
	}

	static  float PrecisionGreater(float x)
	{
		uFloat y(x);
		y.i++;

		return y.f - x;
	}

	static  float PrecisionLess(float x)
	{
		uFloat y(x);
		y.i--;

		return y.f - x;
	}
};

union uDouble
{
	double f;
	uint64_t i;

	 uDouble() : i(0) {}

	 uDouble(double x) : f(x) {}

	 uDouble(uint64_t x) : i(x) {}

	 bool IsNegative() const { return (i >> 63) != 0; }

	 void abs() { i &= ~(1ULL << 63); }

	 uint64_t Mantissa() const { return i & ((1ULL << 52) - 1); }

	 uint64_t Exponent() const { return (i >> 52) & 2047; }

	 bool IsInf() const
	{
		return Exponent() == 2047 && Mantissa() == 0;
	}

	 bool IsNan() const
	{
		return Exponent() == 2047 && Mantissa() != 0;
	}

	static  double PrecisionGreater(double x)
	{
		uDouble y(x);
		y.i++;

		return y.f - x;
	}

	static  double PrecisionLess(double x)
	{
		uDouble y(x);
		y.i--;

		return y.f - x;
	}
};



#define F16_M_BITS 10
#define F16_E_BITS 5
#define F16_S_MASK 0x8000
// Note: this code is supposed to compress 32-bit float to 16-bit float
// All SIMD instructions are deleted
// I hope this 'll compile and work
// This code somehow rewrited to glm, I hope it's worked

template<uint32_t M_BITS, uint32_t E_BITS, uint32_t S_MASK>
uint32_t ToSmallFloat(float x)
{
	const int32_t E_MASK = (1 << E_BITS) - 1;
	const uint32_t INF = uint32_t(E_MASK) << uint32_t(M_BITS);
	const int32_t BIAS = E_MASK >> 1;
	const int32_t ROUND = 1 << (23 - M_BITS - 1);

	// decompose float
	uint32_t f32 = *(uint32_t*)&x;
	uint32_t packed = (f32 >> 16) & S_MASK;
	int32_t e = ((f32 >> 23) & 0xFF) - 127 + BIAS;
	int32_t m = f32 & 0x007FFFFF;

	if (e == 128 + BIAS)
	{
		// Inf
		packed |= INF;

		if (m)
		{
			// NaN
			m >>= 23 - M_BITS;
			packed |= m | (m == 0);
		}
	}
	else if (e > 0)
	{
		// round to nearest, round "0.5" up
		if (m & ROUND)
		{
			m += ROUND << 1;

			if (m & 0x00800000)
			{
				// mantissa overflow
				m = 0;
				e++;
			}
		}

		if (e >= E_MASK)
		{
			// exponent overflow - flush to Inf
			packed |= INF;
		}
		else
		{
			// representable value
			m >>= 23 - M_BITS;
			packed |= (e << M_BITS) | m;
		}
	}
	else
	{
		// denormalized or zero
		m = ((m | 0x00800000) >> (1 - e)) + ROUND;
		m >>= 23 - M_BITS;
		packed |= m;
	}

	return packed;
}

template<int32_t M_BITS, int32_t E_BITS, int32_t S_MASK>
float FromSmallFloat(uint32_t x)
{
	const uint32_t E_MASK = (1 << E_BITS) - 1;
	const int32_t BIAS = E_MASK >> 1;
	const float DENORM_SCALE = 1.0f / (1 << (14 + M_BITS));
	const float NORM_SCALE = 1.0f / float(1 << M_BITS);

	int32_t s = (x & S_MASK) << 15;
	int32_t e = (x >> M_BITS) & E_MASK;
	int32_t m = x & ((1 << M_BITS) - 1);

	uFloat f;
	if (e == 0)
		f.f = DENORM_SCALE * m;
	else if (e == E_MASK)
		f.i = s | 0x7F800000 | (m << (23 - M_BITS));
	else
	{
		f.f = 1.0f + float(m) * NORM_SCALE;

		if (e < BIAS)
			f.f /= float(1 << (BIAS - e));
		else
			f.f *= float(1 << (e - BIAS));
	}

	if (s)
		f.f = -f.f;

	return f.f;
}

inline uint32_t f32tof16(float x)
{
	uint32_t r = ToSmallFloat<F16_M_BITS, F16_E_BITS, F16_S_MASK>(x);
	return r;
}

inline float f16tof32(uint32_t x)
{
	return FromSmallFloat<F16_M_BITS, F16_E_BITS, F16_S_MASK>(x);
}

struct float16_t
{
	inline float16_t(float v) { x = (uint16_t)f32tof16(v); }
	inline operator float() const { return f16tof32(x); }
	inline float16_t() = default;
	inline float16_t(const float16_t&) = default;
	inline float16_t& operator=(const float16_t&) = default;
	uint16_t x;
};

struct float16_t2
{
	float16_t x, y;

	inline float16_t2() = default;
	inline float16_t2(const float16_t2&) = default;
	inline float16_t2& operator=(const float16_t2&) = default;
	inline float16_t2(const float16_t& x, const float16_t& y) : x(x), y(y)
	{
		
	}
};

struct float16_t4
{
	float16_t x, y, z, w;
	inline float16_t4(const float16_t2& xy, const float16_t2& zw)
	{
		*((float16_t2*)&x) = xy;
		*((float16_t2*)&z) = zw;
	}
	inline float16_t4(float16_t x, float16_t y, float16_t z, float16_t w) : x(x), y(y), z(z), w(w) {

	}
	
	inline float16_t4() = default;
	inline float16_t4(const float16_t4&) = default;
	inline float16_t4& operator=(const float16_t4&) = default;

};