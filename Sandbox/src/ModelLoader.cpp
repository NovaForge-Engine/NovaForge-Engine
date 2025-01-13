#include "ModelLoader.h"
using namespace nova;

ModelLoader::ModelLoader()
{
}

ModelLoader::~ModelLoader()
{
}

bool ModelLoader::LoadModel(std::string filename)
{
	spdlog::info("Loading model {}", filename);
	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(
		filename, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);

	if (pScene == nullptr)
		return false;

	this->directory_ = filename.substr(0, filename.find_last_of("/\\"));
	std::cout << this->directory_ << std::endl;

	processNode(pScene->mRootNode, pScene);
	return true;

}

void ModelLoader::processNode(aiNode* node, const aiScene* scene)
{
	spdlog::info("Processing node {}", node->mName.C_Str());
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes_.push_back(this->processMesh(mesh, scene));
	}

	for (int i = 0; i < node->mNumChildren; i++)
	{
		this->processNode(node->mChildren[i], scene);
	}
}

Mesh ModelLoader::processMesh(aiMesh* mesh, const aiScene* scene)
{
	spdlog::info("Processing mesh {}", mesh->mName.C_Str());
	std::vector<Vertex> vertices;
	std::vector<uint8_t> indices;
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vert;
		vert.position[0] = mesh->mVertices[i].x;
		vert.position[1] = mesh->mVertices[i].y;
		vert.position[2] = mesh->mVertices[i].z;

		if (mesh->mTextureCoords[0]){
				float u = std::min(mesh->mTextureCoords[0][i].x, 65504.0f);
			float v = std::min(mesh->mTextureCoords[0][i].y, 65504.0f);
			vert.uv = float2_to_float16_t2(glm::vec2(u, v));
			}

		if (mesh->mNormals)
			{
			vert.N = float4_to_unorm<10, 10, 10, 2>(
				glm::vec4(glm::vec3(mesh->mNormals->x, mesh->mNormals->y,
			                        mesh->mNormals->z) *
			                      0.5f +
			                  0.5f,
			              0.0f));
			}

		vertices.push_back(std::move(vert));
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (uint8_t j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	if (mesh->mMaterialIndex >= 0)
	{
		spdlog::info("Processing material {}", mesh->mMaterialIndex);

		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<Texture> diffuseMaps = this->loadMaterialTextures(material,
			aiTextureType_DIFFUSE, "texture_diffuse",scene);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		std::vector<Texture> specularMaps = this->loadMaterialTextures(material,
			aiTextureType_SPECULAR, "texture_specular",scene);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		std::vector<Texture> normalMaps = this->loadMaterialTextures(material,
			aiTextureType_NORMALS, "texture_normal",scene);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		std::vector<Texture> heightMaps = this->loadMaterialTextures(material,
			aiTextureType_EMISSIVE, "texture_emissive",scene);
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

	}
	spdlog::info("Finished processing mesh {}", mesh->mName.C_Str());
	return Mesh(vertices, indices, textures);

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

		for (uint8_t j = 0; j < texturePaths.size(); j++)
		{
			
			if (std::strcmp(texturePaths[j].c_str(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded_[j]);
				skip =  true; 
				// A texture with the same filepath has already been
				// loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{
			const aiTexture* embeddedTexture =
				scene->GetEmbeddedTexture(str.C_Str());
			if (embeddedTexture != nullptr)
			{
				spdlog::info("We have embeddedTexture, we don't know what to do");
			}




			std::string filename = std::string(str.C_Str());
			filename = this->directory_ + "/" + filename;
			spdlog::info(
				"We're trying to understand what's going on with texture {}",
				filename);
			/*Texture texture; 
			bool res = LoadTexture(filename,texture);
			if (res)
			{
				spdlog::info("Texture loaded");
				textures.push_back(texture);
				texturePaths.push_back(filename);
			}
			else
			{
				spdlog::error("Failed to load texture {}", filename);
			}*/
			// Texture texture = Texture(filename, typeName);
			// textures.push_back(texture);
		}
	}




	return textures;
}


