#include "ModelLoader.h"
using namespace nova;

ModelLoader::ModelLoader()
{
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

	processNode(scene, pScene->mRootNode, pScene);

	spdlog::info("We're done with model {}", filename);
	return true;
}

void ModelLoader::processNode(nova::MeshRegistry& scene, aiNode* node,
                              const aiScene* assimp_scene)
{
	spdlog::info("Processing node {}", node->mName.C_Str());

	uint32_t meshOffset = scene.meshes.size();
	meshOffset = meshOffset + node->mNumMeshes;
	scene.meshes.resize(meshOffset);

	for (int i = 0; i < node->mNumMeshes; i++)
	{
		spdlog::info("Processing mesh {}", node->mMeshes[i]);
		aiMesh* mesh = assimp_scene->mMeshes[node->mMeshes[i]];
		this->processMesh(scene, mesh, assimp_scene);
	}

	for (int i = 0; i < node->mNumChildren; i++)
	{
		spdlog::info("Processing child {}", node->mChildren[i]->mName.C_Str());
		this->processNode(scene, node->mChildren[i], assimp_scene);
	}
}

bool ModelLoader::processMesh(nova::MeshRegistry& scene, aiMesh* mesh,
                              const aiScene* assimp_scene)
{
	spdlog::info("Processing mesh {}", mesh->mName.C_Str());
	std::vector<Vertex> vertices;
	std::vector<uint8_t> indices;
	std::vector<Texture> textures;

	uint32_t meshOffset = scene.meshes.size() - 1;

	size_t totalIndices = scene.indices.size();
	size_t totalVertices = scene.vertices.size();

	Mesh& final_mesh = scene.meshes[meshOffset];
	spdlog::info("Mesh offset {}", meshOffset);
	spdlog::info("Total vertices {}", totalVertices);
	spdlog::info("Total indices {}", totalIndices);

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
		spdlog::info("Processing material {}", mesh->mMaterialIndex);

		aiMaterial* material = assimp_scene->mMaterials[mesh->mMaterialIndex];

		std::vector<Texture> diffuseMaps = this->loadMaterialTextures(
			material, aiTextureType_DIFFUSE, "texture_diffuse", assimp_scene);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		std::vector<Texture> specularMaps = this->loadMaterialTextures(
			material, aiTextureType_SPECULAR, "texture_specular", assimp_scene);
		textures.insert(textures.end(), specularMaps.begin(),
		                specularMaps.end());
		std::vector<Texture> normalMaps = this->loadMaterialTextures(
			material, aiTextureType_NORMALS, "texture_normal", assimp_scene);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		std::vector<Texture> heightMaps = this->loadMaterialTextures(
			material, aiTextureType_EMISSIVE, "texture_emissive", assimp_scene);
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
	}
	spdlog::info("Finished processing mesh {}", mesh->mName.C_Str());
	return true;
}

std::vector<Texture> ModelLoader::loadMaterialTextures(aiMaterial* mat,
                                                       aiTextureType type,
                                                       std::string typeName,
                                                       const aiScene* scene)
{
	spdlog::info("Processing material textures {}", typeName);
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		spdlog::info("Loading texture {}", str.C_Str());
		spdlog::info("Texture type {}", typeName);
		bool skip = false;

		// for (uint8_t j = 0; j < texturepaths.size(); j++)
		//{
		//	spdlog::info("texture path {}" , j);
		//	if (std::strcmp(texturepaths[j].c_str(), str.c_str()) == 0)
		//	{
		//		//textures.push_back(textures_loaded_[j]);
		//		skip =  true;
		//		// a texture with the same filepath has already been
		//		// loaded, continue to next one. (optimization)
		//		break;
		//	}
		// }
		if (!skip)
		{
			const aiTexture* embeddedTexture =
				scene->GetEmbeddedTexture(str.C_Str());
			if (embeddedTexture != nullptr)
			{
				spdlog::info(
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
			spdlog::info("Fullpath of texture is {}" , filename);
			Texture* texture = new Texture;
			bool res = LoadTexture(filename, *texture);
			if (res)
			{
				spdlog::info("Texture loaded");
				textures_loaded_.push_back(texture);
				texturePaths.push_back(filename);
			}
			else
			{
				spdlog::error("Failed to load texture {}", filename);
			}
			// Texture texture = Texture(filename, typeName);
			// textures.push_back(texture);
		}
	}

	return textures;
}
