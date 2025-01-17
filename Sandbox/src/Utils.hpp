#pragma once

#undef OPAQUE
#undef TRANSPARENT

#include "fmath/f16.hpp"
#include "glm/detail/func_packing_simd.inl"

#include <Extensions/NRIHelper.h>
#include <NRI.h>
#include <array>
#include <detex.h>
#include <string>
#include <vector>
struct Texture;

typedef std::vector<std::vector<uint8_t>> ShaderCodeStorage;

inline const char* GetShaderExt(nri::GraphicsAPI graphicsAPI)
{
	if (graphicsAPI == nri::GraphicsAPI::D3D12)
		return ".dxil";
	return ".spirv";
};

struct Shader
{
	const char* ext;
	nri::StageBits stage;
};

constexpr std::array<Shader, 13> gShaderExts = {{
	{"", nri::StageBits::NONE},
	{".vs.", nri::StageBits::VERTEX_SHADER},
	{".tcs.", nri::StageBits::TESS_CONTROL_SHADER},
	{".tes.", nri::StageBits::TESS_EVALUATION_SHADER},
	{".gs.", nri::StageBits::GEOMETRY_SHADER},
	{".fs.", nri::StageBits::FRAGMENT_SHADER},
	{".cs.", nri::StageBits::COMPUTE_SHADER},
	{".rgen.", nri::StageBits::RAYGEN_SHADER},
	{".rmiss.", nri::StageBits::MISS_SHADER},
	{"<noimpl>", nri::StageBits::INTERSECTION_SHADER},
	{".rchit.", nri::StageBits::CLOSEST_HIT_SHADER},
	{".rahit.", nri::StageBits::ANY_HIT_SHADER},
	{"<noimpl>", nri::StageBits::CALLABLE_SHADER},
}};

template<typename T, typename U> constexpr uint32_t GetOffsetOf(U T::* member)
{
	return (uint32_t)((char*)&((T*)nullptr->*member) - (char*)nullptr);
}

template<typename T, uint32_t N> constexpr uint32_t GetCountOf(T const (&)[N])
{
	return N;
}

template<typename T, typename A> constexpr T Align(T x, A alignment)
{
	return (T)((size_t(x) + (size_t)alignment - 1) & ~((size_t)alignment - 1));
}

template<typename T> constexpr size_t GetByteSizeOf(const std::vector<T>& v)
{
	return v.size() * sizeof(decltype(v.back()));
}

struct Annotation
{
	const nri::CoreInterface& m_NRI;
	nri::CommandBuffer& m_CommandBuffer;

	inline Annotation(const nri::CoreInterface& NRI,
	                  nri::CommandBuffer& commandBuffer, const char* name)
		: m_NRI(NRI), m_CommandBuffer(commandBuffer)
	{
		m_NRI.CmdBeginAnnotation(m_CommandBuffer, name, nri::BGRA_UNUSED);
	}

	inline ~Annotation() { m_NRI.CmdEndAnnotation(m_CommandBuffer); }
};

enum DataFolder
{
	NONE,
	SHADERS,
	TEXTURES,
	SCENES,
	MODELS,
	SOUNDS,
	ANIMS,
};

enum class AlphaMode : uint32_t
{
	OPAQUE,
	PREMULTIPLIED,
	TRANSPARENT,
	OFF // alpha is 0 everywhere
};

typedef void* Mip;

static struct FormatMapping
{
	uint32_t detexFormat;
	nri::Format nriFormat;
} formatTable[] = {
	// Uncompressed formats.
	{DETEX_PIXEL_FORMAT_RGB8, nri::Format::UNKNOWN},
	{DETEX_PIXEL_FORMAT_RGBA8, nri::Format::RGBA8_UNORM},
	{DETEX_PIXEL_FORMAT_R8, nri::Format::R8_UNORM},
	{DETEX_PIXEL_FORMAT_SIGNED_R8, nri::Format::R8_SNORM},
	{DETEX_PIXEL_FORMAT_RG8, nri::Format::RG8_UNORM},
	{DETEX_PIXEL_FORMAT_SIGNED_RG8, nri::Format::RG8_SNORM},
	{DETEX_PIXEL_FORMAT_R16, nri::Format::R16_UNORM},
	{DETEX_PIXEL_FORMAT_SIGNED_R16, nri::Format::R16_SNORM},
	{DETEX_PIXEL_FORMAT_RG16, nri::Format::RG16_UNORM},
	{DETEX_PIXEL_FORMAT_SIGNED_RG16, nri::Format::RG16_SNORM},
	{DETEX_PIXEL_FORMAT_RGB16, nri::Format::UNKNOWN},
	{DETEX_PIXEL_FORMAT_RGBA16, nri::Format::RGBA16_UNORM},
	{DETEX_PIXEL_FORMAT_FLOAT_R16, nri::Format::R16_SFLOAT},
	{DETEX_PIXEL_FORMAT_FLOAT_RG16, nri::Format::RG16_SFLOAT},
	{DETEX_PIXEL_FORMAT_FLOAT_RGB16, nri::Format::UNKNOWN},
	{DETEX_PIXEL_FORMAT_FLOAT_RGBA16, nri::Format::RGBA16_SFLOAT},
	{DETEX_PIXEL_FORMAT_FLOAT_R32, nri::Format::R32_SFLOAT},
	{DETEX_PIXEL_FORMAT_FLOAT_RG32, nri::Format::RG32_SFLOAT},
	{DETEX_PIXEL_FORMAT_FLOAT_RGB32, nri::Format::RGB32_SFLOAT},
	{DETEX_PIXEL_FORMAT_FLOAT_RGBA32, nri::Format::RGBA32_SFLOAT},
	{DETEX_PIXEL_FORMAT_A8, nri::Format::UNKNOWN},
	// Compressed formats.
	{DETEX_TEXTURE_FORMAT_BC1, nri::Format::BC1_RGBA_UNORM},
	{DETEX_TEXTURE_FORMAT_BC1A, nri::Format::UNKNOWN},
	{DETEX_TEXTURE_FORMAT_BC2, nri::Format::BC2_RGBA_UNORM},
	{DETEX_TEXTURE_FORMAT_BC3, nri::Format::BC3_RGBA_UNORM},
	{DETEX_TEXTURE_FORMAT_RGTC1, nri::Format::BC4_R_UNORM},
	{DETEX_TEXTURE_FORMAT_SIGNED_RGTC1, nri::Format::BC4_R_SNORM},
	{DETEX_TEXTURE_FORMAT_RGTC2, nri::Format::BC5_RG_UNORM},
	{DETEX_TEXTURE_FORMAT_SIGNED_RGTC2, nri::Format::BC5_RG_SNORM},
	{DETEX_TEXTURE_FORMAT_BPTC_FLOAT, nri::Format::BC6H_RGB_UFLOAT},
	{DETEX_TEXTURE_FORMAT_BPTC_SIGNED_FLOAT, nri::Format::BC6H_RGB_SFLOAT},
	{DETEX_TEXTURE_FORMAT_BPTC, nri::Format::BC7_RGBA_UNORM},
	{DETEX_TEXTURE_FORMAT_ETC1, nri::Format::UNKNOWN},
	{DETEX_TEXTURE_FORMAT_ETC2, nri::Format::UNKNOWN},
	{DETEX_TEXTURE_FORMAT_ETC2_PUNCHTHROUGH, nri::Format::UNKNOWN},
	{DETEX_TEXTURE_FORMAT_ETC2_EAC, nri::Format::UNKNOWN},
	{DETEX_TEXTURE_FORMAT_EAC_R11, nri::Format::UNKNOWN},
	{DETEX_TEXTURE_FORMAT_EAC_SIGNED_R11, nri::Format::UNKNOWN},
	{DETEX_TEXTURE_FORMAT_EAC_RG11, nri::Format::UNKNOWN},
	{DETEX_TEXTURE_FORMAT_EAC_SIGNED_RG11, nri::Format::UNKNOWN}};

struct Texture
{
	std::string name;
	Mip* mips = nullptr;
	AlphaMode alphaMode = AlphaMode::OPAQUE;
	nri::Format format = nri::Format::UNKNOWN;
	uint16_t width = 0;
	uint16_t height = 0;
	uint16_t depth = 0;
	uint8_t mipNum = 0;
	uint16_t layerNum = 0;

	~Texture();

	bool IsBlockCompressed() const;
	void GetSubresource(nri::TextureSubresourceUploadDesc& subresource,
	                    uint32_t mipIndex, uint32_t arrayIndex = 0) const;

	inline void OverrideFormat(nri::Format fmt) { this->format = fmt; }

	inline uint16_t GetArraySize() const { return layerNum; }

	inline uint8_t GetMipNum() const { return mipNum; }

	inline uint16_t GetWidth() const { return width; }

	inline uint16_t GetHeight() const { return height; }

	inline uint16_t GetDepth() const { return depth; }

	inline nri::Format GetFormat() const { return format; }
};

std::string GetFullPath(const std::string& localPath, DataFolder dataFolder);
const char* GetFileName(const std::string& path);
bool LoadFile(std::string& path, std::vector<uint8_t>& data);
nri::ShaderDesc LoadShader(nri::GraphicsAPI api, const std::string& shaderName,
                           ShaderCodeStorage& storage,
                           const char* entryPointName = nullptr);
static nri::Format GetFormatNRI(uint32_t detexFormat);
static nri::Format MakeSRGBFormat(nri::Format format);

static void PostProcessTexture(const std::string& name, Texture& texture,
                               bool computeAvgColorAndAlphaMode,
                               detexTexture** dTexture, int mipNum);
bool LoadTexture(const std::string& path, Texture& texture,
                 bool computeAvgColorAndAlphaMode = false);
void LoadTextureFromMemory(nri::Format format, uint32_t width, uint32_t height,
                           const uint8_t* pixels, Texture& texture);

bool LoadTextureFromMemory(const std::string& name, const uint8_t* data,
                           int dataSize, Texture& texture,
                           bool computeAvgColorAndAlphaMode);

#define NRI_ABORT_ON_FAILURE(result)                                           \
	if ((result) != nri::Result::SUCCESS)                                      \
		exit(1);

#define NRI_ABORT_ON_FALSE(result)                                             \
	if (!(result))                                                             \
		exit(1);

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
	uint32_t vertexOffset;
	uint32_t indexOffset;
	uint32_t indexNum;
	uint32_t vertexNum;
};