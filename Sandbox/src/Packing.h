#pragma once
#include "fmath/f16.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
// For value_ptr
// � 2021 NVIDIA Corporation

uint32_t float2_to_unorm_16_16(const glm::vec2& v);
float16_t2 float2_to_float16_t2(const glm::vec2& v);

template<uint32_t Rbits, uint32_t Gbits, uint32_t Bbits, uint32_t Abits>
uint32_t float4_to_unorm(const glm::vec4& v)
{
	static_assert(Rbits + Gbits + Bbits + Abits <= 32,
	              "Sum of all bit must be <= 32");

	const uint32_t Rmask = (1 << Rbits) - 1;
	const uint32_t Gmask = (1 << Gbits) - 1;
	const uint32_t Bmask = (1 << Bbits) - 1;
	const uint32_t Amask = (1 << Abits) - 1;

	const uint32_t Gshift = Rbits;
	const uint32_t Bshift = Gshift + Gbits;
	const uint32_t Ashift = Bshift + Bbits;

	const glm::vec4 scale =
		glm::vec4(float(Rmask), float(Gmask), float(Bmask), float(Amask));

	glm::vec4 t = v * scale;
	uint32_t p = static_cast<uint32_t>(t.x) & Rmask;
	p |= (static_cast<uint32_t>(t.y) & Gmask) << Gshift;
	p |= (static_cast<uint32_t>(t.z) & Bmask) << Bshift;
	p |= (static_cast<uint32_t>(t.w) & Amask) << Ashift;

	return p;
}