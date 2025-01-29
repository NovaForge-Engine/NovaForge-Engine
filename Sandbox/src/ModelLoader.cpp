#include "ModelLoader.h"
using namespace nova;

ModelLoader::ModelLoader()
{
	materials_.resize(10);

	Texture* texture = new Texture;
	bool res = LoadTexture(GetFullPath("black.png", DataFolder::TEXTURES), *texture);
	if (!res)
	{
		spdlog::error("can't load a texture inside model loader");
	}
	else
	{
		textures_loaded_.push_back(texture);
	}
	texture = new Texture;
	res = LoadTexture(GetFullPath("white.png", DataFolder::TEXTURES), *texture);
	if (!res)
	{
		spdlog::error("can't load a texture inside model loader");
	}
	else
	{
		textures_loaded_.push_back(texture);
	}
	texture = new Texture;
	res = LoadTexture(GetFullPath("flatnormal.png", DataFolder::TEXTURES), *texture);
	if (!res)
	{
		spdlog::error("can't load a texture inside model loader");
	}
	else
	{
		textures_loaded_.push_back(texture);
	}
	texture = new Texture;
	res = LoadTexture(GetFullPath("checkerboard3.dds", DataFolder::TEXTURES), *texture);
	if (!res)
	{
		spdlog::error("can't load a texture inside model loader");
	}
	else
	{
		textures_loaded_.push_back(texture);
	}

	texture = new Texture;
	res = LoadTexture(GetFullPath("bush-text.png", DataFolder::TEXTURES),
	                  *texture);
	if (!res)
	{
		spdlog::error("can't load a texture inside model loader");
	}
	else
	{
		textures_loaded_.push_back(texture);
	}

	texture = new Texture;
	res = LoadTexture(GetFullPath("land-text.png", DataFolder::TEXTURES),
	                  *texture);
	if (!res)
	{
		spdlog::error("can't load a texture inside model loader");
	}
	else
	{
		textures_loaded_.push_back(texture);
	}

	texture = new Texture;
	res = LoadTexture(GetFullPath("gg_text_selected.png", DataFolder::TEXTURES),
	                  *texture);
	if (!res)
	{
		spdlog::error("can't load a texture inside model loader");
	}
	else
	{
		textures_loaded_.push_back(texture);
	}

	texture = new Texture;
	res = LoadTexture(GetFullPath("gg_text.png", DataFolder::TEXTURES),
	                  *texture);
	if (!res)
	{
		spdlog::error("can't load a texture inside model loader");
	}
	else
	{
		textures_loaded_.push_back(texture);
	}

	texture = new Texture;
	res =
		LoadTexture(GetFullPath("gg_text_damaged.png", DataFolder::TEXTURES), *texture);
	if (!res)
	{
		spdlog::error("can't load a texture inside model loader");
	}
	else
	{
		textures_loaded_.push_back(texture);
	}
	texture = new Texture;
	res = LoadTexture(GetFullPath("gg_text_healed.png", DataFolder::TEXTURES),
	                  *texture);
	if (!res)
	{
		spdlog::error("can't load a texture inside model loader");
	}
	else
	{
		textures_loaded_.push_back(texture);
	}

		texture = new Texture;
	res = LoadTexture(GetFullPath("sabrecat_1.jpeg", DataFolder::TEXTURES),
	                  *texture);
	if (!res)
	{
		spdlog::error("can't load a texture inside model loader");
	}
	else
	{
		textures_loaded_.push_back(texture);
	}
	texture = new Texture;
	res = LoadTexture(GetFullPath("monster-damaged.png", DataFolder::TEXTURES),
	                  *texture);
	if (!res)
	{
		spdlog::error("can't load a texture inside model loader");
	}
	else
	{
		textures_loaded_.push_back(texture);
	}

			texture = new Texture;
	res = LoadTexture(GetFullPath("Ambulance_snow.png", DataFolder::TEXTURES),
	                  *texture);
	if (!res)
	{
		spdlog::error("can't load a texture inside model loader");
	}
	else
	{
		textures_loaded_.push_back(texture);
	}


	Material material = { 0, 1, 2, 3
	};
	materials_[0] = material;

	material = {4, 1, 2, 3};

	materials_[1] = material;

	material = {5, 1, 2, 3};

	materials_[2] = material;

	material = {6, 1, 2, 3};

	materials_[3] = material;

	material = {7, 1, 2, 3};

	materials_[4] = material;

	material = {8, 1, 2, 3};

	materials_[5] = material;

	
	material = {9, 1, 2, 3};

	materials_[6] = material;

	material = {10, 1, 2, 3};

	materials_[7] = material;

	
	material = {11, 1, 2, 3};

	materials_[8] = material;

	material = {12, 1, 2, 3};

	materials_[9] = material;

}

ModelLoader::~ModelLoader()
{
}

bool ModelLoader::LoadModel(nova::MeshRegistry& scene, std::string filename)
{
	spdlog::info("Loading model {}", filename);
	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(
		filename, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);

	if (pScene == nullptr)
		return false;

	this->directory_ = filename.substr(0, filename.find_last_of("/\\"));
	std::cout << this->directory_ << std::endl;

	spdlog::info("Nums of material is {}", pScene->mNumMaterials);


	materialOffset = materials_.size();
	uint32_t materialSize = pScene->mNumMaterials;

	materials_.resize(materialOffset + materialSize);
	




	processNode(scene, pScene->mRootNode, pScene);

	for (int i = materialOffset; i < materialSize + materialOffset; i++)
	{
		if (materials_[i].ambientTexIndex == (uint32_t)-1)
		{
			materials_[i] = Material{0, 1, 2, 3};
		}
	}
	//TODO: elizoorg 22.01.2025
	//Maybe we need to cut all materials with no textures and bind them to 0 material

	scene.materials_ = materials_;
	scene.textures= textures_loaded_;

	spdlog::info("We're done with model {}", filename);
	return true;
}

void ModelLoader::processNode(nova::MeshRegistry& scene, aiNode* node,
                              const aiScene* assimp_scene)
{
	//spdlog::info("Processing node {}", node->mName.C_Str());

	uint32_t meshOffset = scene.meshes.size();
	meshOffset = meshOffset + node->mNumMeshes;
	scene.meshes.resize(meshOffset);


	for (int i = 0; i < node->mNumMeshes; i++)
	{
		//spdlog::info("Processing mesh {}", node->mMeshes[i]);
		aiMesh* mesh = assimp_scene->mMeshes[node->mMeshes[i]];
		this->processMesh(scene, mesh, assimp_scene);
	}

	for (int i = 0; i < node->mNumChildren; i++)
	{
		//spdlog::info("Processing child {}", node->mChildren[i]->mName.C_Str());
		this->processNode(scene, node->mChildren[i], assimp_scene);
	}
}

bool ModelLoader::processMesh(nova::MeshRegistry& scene, aiMesh* mesh,
                              const aiScene* assimp_scene)
{
	//spdlog::info("Processing mesh {}", mesh->mName.C_Str());
	std::vector<Vertex> vertices;
	std::vector<uint8_t> indices;

	uint32_t meshOffset = scene.meshes.size() - 1;

	size_t totalIndices = scene.indices.size();
	size_t totalVertices = scene.vertices.size();



	Mesh& final_mesh = scene.meshes[meshOffset];
	//spdlog::info("Mesh offset {}", meshOffset);
	//spdlog::info("Total vertices {}", totalVertices);
	//spdlog::info("Total indices {}", totalIndices);

	final_mesh.vertexOffset = totalVertices;
	final_mesh.indexOffset = totalIndices;

	final_mesh.indexNum = mesh->mNumFaces * 3;
	final_mesh.vertexNum = mesh->mNumVertices;


	totalVertices += mesh->mNumVertices;
	uint32_t indexCount = 0;
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		indexCount += face.mNumIndices;
	}
	totalIndices += indexCount;

	scene.vertices.resize(totalVertices);

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex& vert = scene.vertices[final_mesh.vertexOffset + i];
		vert.position[0] = mesh->mVertices[i].x;
		vert.position[1] = mesh->mVertices[i].y;
		vert.position[2] = mesh->mVertices[i].z;

		if (mesh->mTextureCoords[0])
		{
			float u = std::min(mesh->mTextureCoords[0][i].x, 65504.0f);
			float v = std::min(mesh->mTextureCoords[0][i].y, 65504.0f);
			vert.uv = float2_to_float16_t2(glm::vec2(u, v));
		}

		if (mesh->mNormals)
		{
			vert.N = float4_to_unorm<10, 10, 10, 2>(
				glm::vec4(glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y,
			                        mesh->mNormals[i].z) *
			                      0.5f +
			                  0.5f,
			              0.0f));
		}
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (uint8_t j = 0; j < face.mNumIndices; j++)
			scene.indices.push_back(face.mIndices[j]);
	}
	if (mesh->mMaterialIndex >= 0)
	{

		final_mesh.materialNum = mesh->mMaterialIndex + materialOffset;
		//spdlog::info("final mesh material index is {} and {} and {}", final_mesh.materialNum,mesh->mMaterialIndex, materialOffset);

		aiMaterial* material = assimp_scene->mMaterials[mesh->mMaterialIndex];

		//spdlog::info("Processing material {} with name {} ", mesh->mMaterialIndex,
		//            material->GetName().C_Str());

		
		bool result = loadMaterialTextures(material, aiTextureType_DIFFUSE, 
									  "aiTextureType_DIFFUSE", assimp_scene,
								      mesh->mMaterialIndex);


		if (result)
		{
			materials_[mesh->mMaterialIndex + materialOffset]
				.diffuseTexIndex = textures_loaded_.size()-1;
		}
		else
		{
			if (materials_[mesh->mMaterialIndex + materialOffset]
			        .diffuseTexIndex == (uint32_t)-1)
				materials_[mesh->mMaterialIndex + materialOffset]
					.diffuseTexIndex =ESTATIC_TEXTURES::INVALID;
		}

		result = loadMaterialTextures(material, aiTextureType_AMBIENT,
		                              "aiTextureType_AMBIENT", assimp_scene,
		                              mesh->mMaterialIndex);

			if (result)
		{
				materials_[mesh->mMaterialIndex + materialOffset]
				.ambientTexIndex =
				textures_loaded_.size()-1;
		}
		else
		{
			if (materials_[mesh->mMaterialIndex + materialOffset]
			        .ambientTexIndex ==
			    (uint32_t)-1)
				materials_[mesh->mMaterialIndex + materialOffset]
					.ambientTexIndex =
				ESTATIC_TEXTURES::WHITE;
		}
	
		result = loadMaterialTextures(material, aiTextureType_HEIGHT,
		                              "aiTextureType_HEIGHT", assimp_scene,
		                              mesh->mMaterialIndex);

					if (result)
		{
			materials_[mesh->mMaterialIndex + materialOffset]
				.heightTexIndex =
				textures_loaded_.size()-1;
		}
		else
		{
			if (materials_[mesh->mMaterialIndex + materialOffset]
			        .heightTexIndex ==
			    (uint32_t)-1)
				materials_[mesh->mMaterialIndex + materialOffset]
					.heightTexIndex =
				ESTATIC_TEXTURES::FLATNORMAL;
		}

		result = loadMaterialTextures(material, aiTextureType_OPACITY,
		                              "aiTextureType_OPACITY", assimp_scene,
		                              mesh->mMaterialIndex);

		
		if (result)
		{
			materials_[mesh->mMaterialIndex + materialOffset].OpacityTexIndex =
				textures_loaded_.size() - 1;
		}
		else
			{
			if (materials_[mesh->mMaterialIndex + materialOffset]
			        .OpacityTexIndex ==
			    (uint32_t)-1)
					materials_[mesh->mMaterialIndex + materialOffset]
						.OpacityTexIndex =
					ESTATIC_TEXTURES::BLACK;
			}



	}
	//spdlog::info("Finished processing mesh {}", mesh->mName.C_Str());
	return true;
}

 bool ModelLoader::loadMaterialTextures(aiMaterial* mat,
                                                       aiTextureType type,
                                                       std::string typeName,
                                       const aiScene* scene,
                                       unsigned int materialIndex)

{
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		//spdlog::info("Loading texture {} with type {}", str.C_Str(), typeName);
		bool skip = false;

		int textureType;
		switch (type)
		{
			case aiTextureType_DIFFUSE:
				textureType = EMaterial::DIFFUSE;
				skip = materials_[materialIndex+materialOffset].diffuseTexIndex < (uint32_t) -1;
				break;
			case aiTextureType_OPACITY:
				skip =
					materials_[materialIndex + materialOffset].OpacityTexIndex <
					(uint32_t)-1;
				break;
			case aiTextureType_HEIGHT:
				skip =
					materials_[materialIndex + materialOffset].heightTexIndex <
					(uint32_t)-1;
				break;
			case aiTextureType_AMBIENT:
				skip =
					materials_[materialIndex + materialOffset].ambientTexIndex <
					(uint32_t)-1;
				break;
		}


	
		
		if (!skip)
		{
			const aiTexture* embeddedTexture =
				scene->GetEmbeddedTexture(str.C_Str());
			if (embeddedTexture != nullptr)
			{
				spdlog::error(
					"We have embeddedTexture, we don't know what to do");
			}

			std::string filename = std::string(str.C_Str());
			for (size_t i = 0; i < filename.length(); i++)
			{
				if ((int)filename[i] == 92)
				{
					filename[i] = '/';
				}
			}
			filename = this->directory_ + "/" + filename;
			//spdlog::info("Fullpath of texture is {}" , filename);
			Texture* texture = new Texture;
			bool res = LoadTexture(filename, *texture);
			if (res)
			{

				//spdlog::info("Texture loaded");
				textures_loaded_.push_back(texture);
				return true;
			}
			else
			{
				spdlog::error("Failed to load texture {}", filename);
				return false;
			}
		}
		return false;
	}

	return false;
}
