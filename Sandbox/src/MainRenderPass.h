#pragma once
#include "IRenderPass.h"
#include "NRI.h"
#include "Utils.hpp"


constexpr uint32_t BUFFERED_FRAME_MAX_NUM = 2;

struct Vertex
{
	float position[2];

	float uv[2];
};

struct ConstantBufferLayout
{
	float color[3];
	float scale;
};

static const Vertex g_VertexData[] = {{-0.71f, -0.50f, 0.0f, 0.0f},
                                      {0.00f, 0.71f, 1.0f, 1.0f},
                                      {0.71f, -0.50f, 0.0f, 1.0f}};

static const uint16_t g_IndexData[] = {0, 1, 2};


class MainRenderPass: public IRenderPass{
public:
	void Init() override;
	void Draw() override;
	void Shutdown() override;
	void PrepareFrame() override;
	bool Init(const nri::CoreInterface& NRI,
	          const nri::HelperInterface& helperInterface,
	          nri::Device& m_Device,
	          nri::Format renderTargetFormat);


	
    MainRenderPass();
    ~MainRenderPass();


    private:


	nri::PipelineLayout* m_PipelineLayout = nullptr;
	nri::Pipeline* m_Pipeline = nullptr;
	nri::DescriptorSet* m_TextureDescriptorSet = nullptr;
	nri::Descriptor* m_TextureShaderResource = nullptr;
	nri::Descriptor* m_Sampler = nullptr;
	nri::Buffer* m_ConstantBuffer = nullptr;
	nri::Buffer* m_GeometryBuffer = nullptr;
	nri::Texture* m_Texture = nullptr;
	nri::DescriptorPool* m_DescriptorPool = nullptr;

	std::vector<nri::Descriptor*> m_Descriptors;
	std::vector<nri::DescriptorSet*> m_DescriptorSets;

	std::vector<nri::Memory*> m_MemoryAllocations;

	uint64_t m_GeometryOffset = 0;

};
