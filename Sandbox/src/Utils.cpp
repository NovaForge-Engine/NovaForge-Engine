#include "Utils.hpp"
#include <string>
#include <iostream>
#include <detex.h>
#include <stb_image.h>
#include <cassert>

#include <glm/vec4.hpp>


std::string GetFullPath(const std::string& localPath, DataFolder dataFolder) {
	std::string path = "_Data/"; // it's a symbolic link
	if (dataFolder == DataFolder::SHADERS)
		path = "_Shaders/"; // special folder with generated files
	else if (dataFolder == DataFolder::TEXTURES)
		path += "Textures/";
	else if (dataFolder == DataFolder::SCENES)
		path += "Scenes/";
	return path + localPath;
}


const char* GetFileName(const std::string& path) {
	const size_t slashPos = path.find_last_of("\\/");
	if (slashPos != std::string::npos)
		return path.c_str() + slashPos + 1;

	return "";
}



bool LoadFile(std::string& path, std::vector<uint8_t>& data) {
	FILE* file = fopen(path.c_str(), "rb");

	if (file == nullptr) {
		printf("ERROR: File '%s' is not found!\n", path.c_str());
		data.clear();
		return false;
	}

	printf("Loading file '%s'...\n", GetFileName(path));

	fseek(file, 0, SEEK_END);
	const size_t size = ftell(file); // 32-bit size
	fseek(file, 0, SEEK_SET);

	data.resize(size);
	const size_t readSize = fread(&data[0], size, 1, file);
	fclose(file);

	return !data.empty() && readSize == 1;
}



nri::ShaderDesc LoadShader(nri::GraphicsAPI api, const std::string& shaderName, ShaderCodeStorage& storage, const char* entryPointName) {
	const char* ext = GetShaderExt(api);
	std::string fullPath = GetFullPath(shaderName + ext, DataFolder::SHADERS);
	std::cout << fullPath << std::endl;
	nri::ShaderDesc shaderDesc = {};
	size_t i = 1;
	for (; i < gShaderExts.size(); i++) {
		if (fullPath.rfind(gShaderExts[i].ext) != std::string::npos) {
			storage.push_back(std::vector<uint8_t>());
			std::vector<uint8_t>& code = storage.back();

			if (LoadFile(fullPath, code)) {
				shaderDesc.stage = gShaderExts[i].stage;
				shaderDesc.bytecode = code.data();
				shaderDesc.size = code.size();
				shaderDesc.entryPointName = entryPointName;
			}
			break;
		}
	}
	if (i == gShaderExts.size()) {
		printf("ERROR: Shader '%s' has invalid shader extension!\n", shaderName.c_str());
		NRI_ABORT_ON_FALSE(false);
	}

	return shaderDesc;

}

inline detexTexture** ToTexture(Mip* mips) {
	return (detexTexture**)mips;
}

inline detexTexture* ToMip(Mip mip) {
	return (detexTexture*)mip;
}




Texture::~Texture() {
	detexFreeTexture(ToTexture(mips), mipNum);
}

void Texture::GetSubresource(nri::TextureSubresourceUploadDesc& subresource, uint32_t mipIndex, uint32_t arrayIndex) const {
	// TODO: 3D images are not supported, "subresource.slices" needs to be allocated to store pointers to all slices of the current mipmap
	assert(GetDepth() == 1);


	(void)(arrayIndex); // TODO: unused

	detexTexture* mip = ToMip(mips[mipIndex]);

	int rowPitch, slicePitch;
	detexComputePitch(mip->format, mip->width, mip->height, &rowPitch, &slicePitch);

	subresource.slices = mip->data;
	subresource.sliceNum = 1;
	subresource.rowPitch = (uint32_t)rowPitch;
	subresource.slicePitch = (uint32_t)slicePitch;
}

bool Texture::IsBlockCompressed() const {
	return detexFormatIsCompressed(ToMip(mips[0])->format);
}



static nri::Format GetFormatNRI(uint32_t detexFormat) {
	for (auto& entry : formatTable) {
		if (entry.detexFormat == detexFormat)
			return entry.nriFormat;
	}

	return nri::Format::UNKNOWN;
}

static nri::Format MakeSRGBFormat(nri::Format format) {
	switch (format) {
	case nri::Format::RGBA8_UNORM:
		return nri::Format::RGBA8_SRGB;

	case nri::Format::BC1_RGBA_UNORM:
		return nri::Format::BC1_RGBA_SRGB;

	case nri::Format::BC2_RGBA_UNORM:
		return nri::Format::BC2_RGBA_SRGB;

	case nri::Format::BC3_RGBA_UNORM:
		return nri::Format::BC3_RGBA_SRGB;

	case nri::Format::BC7_RGBA_UNORM:
		return nri::Format::BC7_RGBA_SRGB;

	default:
		return format;
	}
}


static void PostProcessTexture(const std::string& name, Texture& texture, bool computeAvgColorAndAlphaMode, detexTexture** dTexture, int mipNum) {


	texture.mips = (Mip*)dTexture;
	texture.name = name;
	texture.format = GetFormatNRI(dTexture[0]->format);
	texture.width = (uint16_t)dTexture[0]->width;
	texture.height = (uint16_t)dTexture[0]->height;
	texture.mipNum = (uint8_t)mipNum;


	// TODO: detex doesn't support cubemaps and 3D textures
	texture.layerNum = 1;
	texture.depth = 1;

	texture.alphaMode = AlphaMode::OPAQUE;
	if (computeAvgColorAndAlphaMode) {
		// Alpha mode
		if (texture.format == nri::Format::BC1_RGBA_UNORM || texture.format == nri::Format::BC1_RGBA_SRGB) {
			bool hasTransparency = false;
			for (int i = mipNum - 1; i >= 0 && !hasTransparency; i--) {
				const size_t size = detexTextureSize(dTexture[i]->width_in_blocks, dTexture[i]->height_in_blocks, dTexture[i]->format);
				const uint8_t* bc1 = dTexture[i]->data;

				for (size_t j = 0; j < size && !hasTransparency; j += 8) {
					const uint16_t* c = (uint16_t*)bc1;
					if (c[0] <= c[1]) {
						const uint32_t bits = *(uint32_t*)(bc1 + 4);
						for (uint32_t k = 0; k < 32 && !hasTransparency; k += 2)
							hasTransparency = ((bits >> k) & 0x3) == 0x3;
					}
					bc1 += 8;
				}
			}

			if (hasTransparency)
				texture.alphaMode = AlphaMode::PREMULTIPLIED;
		}

		// Decompress last mip
		std::vector<uint8_t> image;
		detexTexture* lastMip = dTexture[mipNum - 1];
		uint8_t* rgba8 = lastMip->data;
		if (lastMip->format != DETEX_PIXEL_FORMAT_RGBA8) {
			// Convert to RGBA8 if the texture is compressed
			image.resize(lastMip->width * lastMip->height * detexGetPixelSize(DETEX_PIXEL_FORMAT_RGBA8));
			detexDecompressTextureLinear(lastMip, &image[0], DETEX_PIXEL_FORMAT_RGBA8);
			rgba8 = &image[0];
		}

		// Average color
		glm::vec4 avgColor(0.0f);
		const size_t pixelNum = lastMip->width * lastMip->height;
		for (size_t i = 0; i < pixelNum; i++)
		{
			uint32_t color = (*(uint32_t*)(rgba8 + i * 4));
			glm::vec4 col
			{
				color >> 24, 
				(color >> 16) & 0xFF, 
				(color >> 8) & 0xFF, 
				color & 0xFF
			};
			avgColor += col;
		}
		
		avgColor /= float(pixelNum);

		if (texture.alphaMode != AlphaMode::PREMULTIPLIED && avgColor.w < 254.0f / 255.0f)
			texture.alphaMode = AlphaMode::TRANSPARENT;

		if (texture.alphaMode == AlphaMode::TRANSPARENT && avgColor.w == 0.0f) {
			printf("WARNING: Texture '%s' is fully transparent!\n", name.c_str());
			texture.alphaMode = AlphaMode::OFF;
		}
	}



}

bool LoadTexture(const std::string& path, Texture& texture, bool computeAvgColorAndAlphaMode) {
	printf("Loading texture '%s'...\n", GetFileName(path));

	detexTexture** dTexture = nullptr;
	int mipNum = 0;

	if (!detexLoadTextureFileWithMipmaps(path.c_str(), 32, &dTexture, &mipNum)) {
		printf("ERROR: Can't load texture '%s'\n", path.c_str());

		return false;
	}

	PostProcessTexture(path, texture, computeAvgColorAndAlphaMode, dTexture, mipNum);

	return true;
}