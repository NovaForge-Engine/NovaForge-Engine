#pragma once
#include "Utils.hpp"
#include "assimp/Vertex.h"

#include <vector>
namespace nova
{

	class Scene
	{
	public:
		Scene();
		~Scene();

	public:
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture*> textures;
		std::vector<Mesh> meshes;
	};

} // namespace nova