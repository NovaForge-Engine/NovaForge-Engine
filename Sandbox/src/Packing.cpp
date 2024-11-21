#include "Packing.h"

uint32_t float2_to_unorm_16_16(const glm::vec2& v)
{
	glm::vec4 t(v.x, v.y, 0.0f, 0.0f);
	t *= 65535.0f;
	glm::ivec4 i = glm::ivec4(t);

	uint32_t p = i[0];
	p |= i[1] << 16;

	return p;
}