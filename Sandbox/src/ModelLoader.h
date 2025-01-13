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

#include <spdlog/spdlog.h>

namespace nova
{

struct UnpackedVertex
{
	float vertices[3];
	float uv[2];
	float normals[3];
	float tangents[4];
};


struct Vertex
{
	float position[3];
	float16_t2 uv;
	uint32_t N;
	uint32_t T;
};

struct Mesh
{
	std::vector<Vertex> vertices;
	std::vector<uint8_t> indices;
	std::vector<Texture> textures;

};



class ModelLoader
{
public:
	ModelLoader();
	~ModelLoader();

	bool LoadModel(std::string filename);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	std::vector<Texture> loadMaterialTextures(aiMaterial* mat,
	                                           aiTextureType type,
	                                           std::string typeName,
	                                           const aiScene* scene);


	std::vector<Mesh> meshes_;
	std::vector<std::string> texturePaths;



	std::string directory_;

	std::vector<Texture> textures_loaded_;


};

} // namespace nova