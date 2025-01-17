#pragma once
#include "Packing.h"
#include "Scene.hpp"
#include "Utils.hpp"
#include "fmath/f16.hpp"

#include <assimp/Importer.hpp>
#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <iostream>
#include <spdlog/spdlog.h>
#include <string>

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