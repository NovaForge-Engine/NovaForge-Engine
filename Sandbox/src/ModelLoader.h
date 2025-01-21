#pragma once
#include <iostream>
#include <string>



#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>

#include "Utils.hpp"

#include "fmath/f16.hpp"
#include "Packing.h"
#include "MeshRegistry.hpp"

#include <spdlog/spdlog.h>
#define SPDLOG_FMT_EXTERNAL

namespace nova
{

	class ModelLoader
	{
	public:
		ModelLoader();
		~ModelLoader();

		bool LoadModel(nova::MeshRegistry& scene, std::string filename);
		void processNode(nova::MeshRegistry& scene, aiNode* node,
		                 const aiScene* assimp_scene);
		bool processMesh(nova::MeshRegistry& scene, aiMesh* mesh,
		                 const aiScene* assimp_scene);

		bool loadMaterialTextures(aiMaterial* mat,
		                                          aiTextureType type,
		                                          std::string typeName,
		                                          const aiScene* assimp_scene,unsigned int materialIndex);

		uint32_t materialOffset=0;

		std::vector<Mesh> meshes_;
		std::vector<std::string> texturePaths;

		std::string directory_;

		std::vector<Texture*> textures_loaded_;

		int last_material_index = 0;

		std::vector<Material> materials_;
	};

} // namespace nova