#include "GameScene.h"
#include <cassert>
GameScene* GameScene::instance = nullptr;
GameScene* GameScene::Get()
{
	if (!instance)
		instance = new GameScene();
	return instance;
}

void GameScene::AddObject()
{
	GameObjects * obj = new GameObjects(); //create new obj
	obj->id = last_id++;
	obj->mesh_id = 7;
	obj->material_id = 2;
	obj->world_matrix = glm::mat4(1.0f);
	obj->position = glm::vec3(0.0f);
	obj->scale = glm::vec3(1.0f);
	obj->rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	objects.push_back(obj);
}

void GameScene::RemoveObject(int t)
{
	assert(t < objects.size() + 1);
	objects.erase(objects.begin() + t);
}

void GameScene::SetObject(int t, int mesh_id, int material_id,
                          glm::mat4 world_matrix, glm::vec3 position,
                          glm::quat rotation, glm::vec3 scale)
{
	assert(t < objects.size() + 1);
	GameObjects* obj =objects[t]; 
	obj->id = last_id++;
	obj->mesh_id = mesh_id;
	obj->material_id = material_id;
	obj->world_matrix =world_matrix;
	obj->position = position;
	obj->scale = scale;
	obj->rotation = rotation;

}

void GameScene::SetPosition(int t, glm::vec3 position)
{
	if (t == 0)
		return;
	assert(t < objects.size() + 1);
	GameObjects* obj = objects[t-1]; 
	obj->position = position;
}

void GameScene::SetRotation(int t, glm::quat rotation)
{
	if (t == 0)
		return;
	assert(t < objects.size() + 1);
	GameObjects* obj = objects[t-1];
	obj->rotation = rotation;
}

void GameScene::SetScale(int t, glm::vec3 scale)
{
	if (t == 0)
		return;
	assert(t < objects.size() + 1);
	GameObjects* obj = objects[t - 1];
	obj->scale = scale;
}

void GameScene::SetWorldMatrix(int t, glm::mat4 world_matrix)
{
	if (t == 0) return;
	assert(t < objects.size()+1);
	GameObjects* obj = objects[t-1];
	obj->world_matrix = world_matrix;
}

void GameScene::SetMesh(int t, int mesh_id)
{
	if (t == 0)
		return;
	assert(t < objects.size() + 1);
	GameObjects* obj = objects[t - 1];
	obj->mesh_id = mesh_id;
}

void GameScene::SetMaterial(int t, int material_id)
{
	if (t == 0)
		return;
	assert(t < objects.size() + 1);
	GameObjects* obj = objects[t - 1];
	obj->material_id = material_id;
	spdlog::info("Material id {} {}",t, obj->material_id);
}
