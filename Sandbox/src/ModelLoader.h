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
#include "Scene.hpp"

#include <spdlog/spdlog.h>


namespace nova
{

	class ModelLoader
	{
	public:
		ModelLoader();
		~ModelLoader();

		bool LoadModel(nova::Scene& scene, std::string filename);
		void processNode(nova::Scene& scene, aiNode* node,
		                 const aiScene* assimp_scene);
		bool processMesh(nova::Scene& scene, aiMesh* mesh,
		                 const aiScene* assimp_scene);

		std::vector<Texture> loadMaterialTextures(aiMaterial* mat,
		                                          aiTextureType type,
		                                          std::string typeName,
		                                          const aiScene* assimp_scene);

		std::vector<Mesh> meshes_;
		std::vector<std::string> texturePaths;

		std::string directory_;

		std::vector<Texture*> textures_loaded_;
	};

} // namespace nova