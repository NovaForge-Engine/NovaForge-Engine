#pragma once
#include "IRenderPass.h"
#include <NRI.h>
#include <Extensions/NRIHelper.h>
#include <Extensions/NRIStreamer.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <vector>

struct ImDrawVertOpt
{
	float pos[2];
	uint32_t uv;
	uint32_t col;
};


class UIRenderPass: public IRenderPass {
public:
	void Init() override;
	void Draw() override;
	void Shutdown() override;
	void PrepareFrame() override;

	struct InitParams
	{
		nri::CoreInterface& NRI;
		nri::HelperInterface& helperInterface;
		nri::Device& device;
		nri::Format renderTargetFormat;
	};
	
	bool Init(InitParams params);

	

	void Draw(const nri::CoreInterface& NRI,
	          const nri::StreamerInterface& streamerInterface,
	          nri::Streamer& streamer, nri::CommandBuffer& commandBuffer,
	          float sdrScale, bool isSrgb);

	void BeginUI();
	void EndUI(const nri::StreamerInterface& streamerInterface,
	           nri::Streamer& streamer);

	UIRenderPass();
	~UIRenderPass();

private:

	std::vector<uint8_t> m_UiData;
	nri::DescriptorPool* m_DescriptorPool = nullptr;
	nri::DescriptorSet* m_DescriptorSet = nullptr;
	nri::Descriptor* m_FontShaderResource = nullptr;
	nri::Descriptor* m_Sampler = nullptr;
	nri::Pipeline* m_Pipeline = nullptr;
	nri::PipelineLayout* m_PipelineLayout = nullptr;
	nri::Texture* m_FontTexture = nullptr;
	nri::Memory* m_FontTextureMemory = nullptr;
	GLFWcursor* m_MouseCursors[ImGuiMouseCursor_COUNT] = {};
	double m_TimePrev = 0.0;
	uint64_t m_IbOffset = 0;
	uint64_t m_VbOffset = 0;



};

