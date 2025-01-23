#pragma once
#include "Utils.hpp"
#include "assimp/Vertex.h"

#include <vector>
namespace nova
{

	class MeshRegistry
	{
	public:
		MeshRegistry();
		~MeshRegistry();

	public:
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture*> textures;
		std::vector<Mesh> meshes;

		std::vector<Material> materials_;
	};

} // namespace nova