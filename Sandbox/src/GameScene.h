#pragma once
#include <glm/glm.hpp>
#include<glm/gtc/quaternion.hpp>
#include <spdlog/spdlog.h>
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

class GameScene
{

	static GameScene* instance;




public:
	std::vector< GameObjects* > objects;

	static GameScene* Get();
	uint32_t last_id=0;
	void AddObject();
	void RemoveObject(int t);
	void SetObject(int t, int mesh_id, int material_id, glm::mat4 world_matrix, glm::vec3 position, glm::quat rotation, glm::vec3 scale);
	void SetPosition(int t, glm::vec3 position);
	void SetRotation(int t, glm::quat rotation);
	void SetScale(int t, glm::vec3 scale);
	void SetWorldMatrix(int t, glm::mat4 world_matrix);
	void SetMesh(int t, int mesh_id);
	void SetMaterial(int t, int material_id);

};



