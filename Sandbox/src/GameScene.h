#pragma once
#include <glm/glm.hpp>
#include<glm/gtc/quaternion.hpp>

struct GameObjects
{
	int id;
	int mesh_id;
	int material_id;
	glm::mat4 world_matrix;
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;
};