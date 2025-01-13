#pragma once
#include "IRenderPass.h"
#include "NRI.h"
#include "Utils.hpp"


#include "Settings.h"

#include <NRI.h>
#include <Extensions/NRIHelper.h>
#include <Extensions/NRIStreamer.h>
#include <Extensions/NRISwapChain.h>
#include <NRIDescs.h>


#include "Window.h"

struct CustomVertex
{
	float position[2];

	float uv[2];
};

struct ConstantBufferLayout
{
	float color[3];
	float scale;
};

static const CustomVertex g_VertexData[] = {{-0.71f, -0.50f, 0.0f, 0.0f},
                                      {0.00f, 0.71f, 1.0f, 1.0f},
                                      {0.71f, -0.50f, 0.0f, 1.0f}};

static const uint16_t g_IndexData[] = {0, 1, 2};


class MainRenderPass: public IRenderPass{
public:
	void Init() override;
	void Draw() override;
	void Shutdown() override;
	void PrepareFrame() override;

	struct InitParams
	{
		nri::CoreInterface& NRI;
		nri::HelperInterface& helperInterface;
		nri::Device& m_Device;
		nri::Format renderTargetFormat;
		nri::CommandQueue* commandQueue;
	};
	bool Init(InitParams params);
	void Draw(const nri::CoreInterface& NRI,
	          const nri::StreamerInterface& streamerInterface,
	          const nri::SwapChainInterface& swapChainInterface,
	          const nri::StreamerInterface& streamer,
	          nri::CommandBuffer& commandBuffer, const Frame& frame,
	          const BackBuffer& currentBackBuffer,
	          const uint32_t currentTextureIndex,
	          const uint32_t m_RenderWindowWidth,
	          const uint32_t m_RenderWindowHeight);
	
		void BeginUI();
	void EndUI();
    MainRenderPass();
    ~MainRenderPass();
    private:
	float m_Transparency = 1.0f;
	float m_Scale = 1.0f;

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
