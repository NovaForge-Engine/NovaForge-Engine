

#include <iostream>
#include "PhysicsUtility.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#if defined _WIN32
#    define GLFW_EXPOSE_NATIVE_WIN32
#elif defined __linux__
#    define GLFW_EXPOSE_NATIVE_X11
	#include <csignal> // For SIGTRAP
#else
#    error "Unknown platform"
#endif

#include <GLFW/glfw3native.h>

#include <NRI.h>

#include <Extensions/NRIDeviceCreation.h>
#include <Extensions/NRIHelper.h>
#include <Extensions/NRILowLatency.h>
#include <Extensions/NRIMeshShader.h>
#include <Extensions/NRIRayTracing.h>
#include <Extensions/NRIResourceAllocator.h>
#include <Extensions/NRIStreamer.h>
#include <Extensions/NRISwapChain.h>

#include <vector>
#include <array>
#include "Utils.hpp"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include <utility>

#include <Jolt/Jolt.h>

#include "PhysicsEngine.h"

Assimp::Importer importer;


struct Vertex {
	float position[2];

	float uv[2];
};

struct ConstantBufferLayout {
	float color[3];
	float scale;
};

static const Vertex g_VertexData[] = {
	{-0.71f, -0.50f, 0.0f, 0.0f},
	{0.00f, 0.71f, 1.0f, 1.0f},
	{0.71f, -0.50f, 0.0f, 1.0f} };

static const uint16_t g_IndexData[] = { 0, 1, 2 };



static void GLFW_ErrorCallback(int32_t error, const char* message) {
	printf("GLFW error[%d]: %s\n", error, message);
#if _WIN32
	DebugBreak();
#else
	raise(SIGTRAP);
#endif
}

using namespace std;

nri::CoreInterface NRI;

bool m_DebugAPI = false;
bool m_DebugNRI = false;
constexpr bool D3D11_COMMANDBUFFER_EMULATION = false;
constexpr uint32_t DEFAULT_MEMORY_ALIGNMENT = 16;
constexpr uint32_t BUFFERED_FRAME_MAX_NUM = 2;
constexpr uint32_t SWAP_CHAIN_TEXTURE_NUM = 2;
constexpr nri::SPIRVBindingOffsets SPIRV_BINDING_OFFSETS = { 100, 200, 300, 400 }; // just ShaderMake defaults for simplicity
nri::AllocationCallbacks m_AllocationCallbacks = {};
uint8_t m_VsyncInterval = 0;
uint32_t m_DpiMode = 0;


struct BackBuffer {
	nri::Descriptor* colorAttachment;
	nri::Texture* texture;
};


struct Frame
{
	nri::CommandAllocator* commandAllocator;
	nri::CommandBuffer* commandBuffer;
	nri::Descriptor* constantBufferView;
	nri::DescriptorSet* constantBufferDescriptorSet;
	uint64_t constantBufferViewOffset;
};

struct NRIInterface : public nri::CoreInterface,
	public nri::HelperInterface,
	public nri::StreamerInterface,
	public nri::SwapChainInterface
{
};


#define NRI_ABORT_ON_FAILURE(result) \
    if ((result) != nri::Result::SUCCESS) \
        exit(1);

class Sample {
private:
	NRIInterface NRI = {};
	nri::Device* m_Device = nullptr;
	nri::Streamer* m_Streamer = nullptr;
	nri::SwapChain* m_SwapChain = nullptr;
	nri::CommandQueue* m_CommandQueue = nullptr;
	nri::Fence* m_FrameFence = nullptr;
	nri::DescriptorPool* m_DescriptorPool = nullptr;
	nri::PipelineLayout* m_PipelineLayout = nullptr;
	nri::Pipeline* m_Pipeline = nullptr;
	nri::DescriptorSet* m_TextureDescriptorSet = nullptr;
	nri::Descriptor* m_TextureShaderResource = nullptr;
	nri::Descriptor* m_Sampler = nullptr;
	nri::Buffer* m_ConstantBuffer = nullptr;
	nri::Buffer* m_GeometryBuffer = nullptr;
	nri::Texture* m_Texture = nullptr;

	std::array<Frame, BUFFERED_FRAME_MAX_NUM> m_Frames = {};
	std::vector<BackBuffer> m_SwapChainBuffers;
	std::vector<nri::Memory*> m_MemoryAllocations;


	uint32_t m_RenderOutputWidth = 1920;
	uint32_t m_RenderOutputHeight = 1080;
	uint32_t m_RenderWindowWidth;
	uint32_t m_RenderWindowHeight;

	uint64_t m_GeometryOffset = 0;
	uint32_t frameIndex = 0;

	float m_Transparency = 1.0f;
	float m_Scale = 1.0f;

	nri::Window m_NRIWindow = {};
	GLFWwindow* m_Window = nullptr;
public:
	bool Initialize(nri::GraphicsAPI graphicsAPI);
	bool Create(int _argc, char** _argv);
	Sample();
	~Sample();
	void RenderFrame();

};

Sample::~Sample() {

}

void Sample::RenderFrame()
{
	nri::Dim_t windowWidth = (nri::Dim_t)m_RenderWindowWidth;
	nri::Dim_t windowHeight = (nri::Dim_t)m_RenderWindowHeight;
	nri::Dim_t halfWidth = windowWidth / 2;
	nri::Dim_t halfHeight = windowHeight / 2;


	const uint32_t bufferedFrameIndex = frameIndex % BUFFERED_FRAME_MAX_NUM;
	const Frame& frame = m_Frames[bufferedFrameIndex];

	if (frameIndex >= BUFFERED_FRAME_MAX_NUM) {
		NRI.Wait(*m_FrameFence, 1 + frameIndex - BUFFERED_FRAME_MAX_NUM);
		NRI.ResetCommandAllocator(*frame.commandAllocator);
	}

	const uint32_t currentTextureIndex = NRI.AcquireNextSwapChainTexture(*m_SwapChain);
	BackBuffer& currentBackBuffer = m_SwapChainBuffers[currentTextureIndex];


	ConstantBufferLayout* commonConstants = (ConstantBufferLayout*)NRI.MapBuffer(*m_ConstantBuffer, frame.constantBufferViewOffset, sizeof(ConstantBufferLayout));
	if (commonConstants) {
		commonConstants->color[0] = 0.8f;
		commonConstants->color[1] = 0.5f;
		commonConstants->color[2] = 0.1f;
		commonConstants->scale = m_Scale;

		NRI.UnmapBuffer(*m_ConstantBuffer);
	}

	nri::TextureBarrierDesc textureBarrierDescs = {};
	textureBarrierDescs.texture = currentBackBuffer.texture;
	textureBarrierDescs.after = { nri::AccessBits::COLOR_ATTACHMENT, nri::Layout::COLOR_ATTACHMENT };
	textureBarrierDescs.layerNum = 1;
	textureBarrierDescs.mipNum = 1;

	// Record
	nri::CommandBuffer* commandBuffer = frame.commandBuffer;
	NRI.BeginCommandBuffer(*commandBuffer, m_DescriptorPool);
	{
		nri::BarrierGroupDesc barrierGroupDesc = {};
		barrierGroupDesc.textureNum = 1;
		barrierGroupDesc.textures = &textureBarrierDescs;
		NRI.CmdBarrier(*commandBuffer, barrierGroupDesc);

		nri::AttachmentsDesc attachmentsDesc = {};
		attachmentsDesc.colorNum = 1;
		attachmentsDesc.colors = &currentBackBuffer.colorAttachment;

		constexpr nri::Color32f COLOR_0 = { 1.0f, 1.0f, 0.0f, 1.0f };

		NRI.CmdBeginRendering(*commandBuffer, attachmentsDesc);
		{
			{
				Annotation annotation(NRI, *commandBuffer, "Clears");

				nri::ClearDesc clearDesc = {};
				clearDesc.planes = nri::PlaneBits::COLOR;
				clearDesc.value.color.f = COLOR_0;

				NRI.CmdClearAttachments(*commandBuffer, &clearDesc, 1, nullptr, 0);
			}

			{
				Annotation annotation(NRI, *commandBuffer, "Triangle");

				const nri::Viewport viewport = { 0.0f, 0.0f, (float)windowWidth, (float)windowHeight, 0.0f, 1.0f };
				NRI.CmdSetViewports(*commandBuffer, &viewport, 1);

				NRI.CmdSetPipelineLayout(*commandBuffer, *m_PipelineLayout);
				NRI.CmdSetPipeline(*commandBuffer, *m_Pipeline);
				NRI.CmdSetRootConstants(*commandBuffer, 0, &m_Transparency, 4);
				NRI.CmdSetIndexBuffer(*commandBuffer, *m_GeometryBuffer, 0, nri::IndexType::UINT16);
				NRI.CmdSetVertexBuffers(*commandBuffer, 0, 1, &m_GeometryBuffer, &m_GeometryOffset);
				NRI.CmdSetDescriptorSet(*commandBuffer, 0, *frame.constantBufferDescriptorSet, nullptr);
				NRI.CmdSetDescriptorSet(*commandBuffer, 1, *m_TextureDescriptorSet, nullptr);

				nri::Rect scissor = { 0, 0, windowWidth, windowHeight };
				NRI.CmdSetScissors(*commandBuffer, &scissor, 1);
				NRI.CmdDrawIndexed(*commandBuffer, { 3, 1, 0, 0, 0 });

				scissor = { (int16_t)halfWidth, (int16_t)halfHeight, halfWidth, halfHeight };
				NRI.CmdSetScissors(*commandBuffer, &scissor, 1);
				NRI.CmdDraw(*commandBuffer, { 3, 1, 0, 0 });
			}

			{
	
			}
		}
		NRI.CmdEndRendering(*commandBuffer);

		textureBarrierDescs.before = textureBarrierDescs.after;
		textureBarrierDescs.after = { nri::AccessBits::UNKNOWN, nri::Layout::PRESENT };

		NRI.CmdBarrier(*commandBuffer, barrierGroupDesc);
	}
	NRI.EndCommandBuffer(*commandBuffer);

	{ // Submit
		nri::QueueSubmitDesc queueSubmitDesc = {};
		queueSubmitDesc.commandBuffers = &frame.commandBuffer;
		queueSubmitDesc.commandBufferNum = 1;

		NRI.QueueSubmit(*m_CommandQueue, queueSubmitDesc);
	}

	// Present
	NRI.QueuePresent(*m_SwapChain);

	{ // Signaling after "Present" improves D3D11 performance a bit
		nri::FenceSubmitDesc signalFence = {};
		signalFence.fence = m_FrameFence;
		signalFence.value = 1 + frameIndex;

		nri::QueueSubmitDesc queueSubmitDesc = {};
		queueSubmitDesc.signalFences = &signalFence;
		queueSubmitDesc.signalFenceNum = 1;

		NRI.QueueSubmit(*m_CommandQueue, queueSubmitDesc);
	}

	frameIndex++;


}


Sample::Sample()
{
}


bool Sample::Initialize(nri::GraphicsAPI graphicsApi) {
	nri::AdapterDesc bestAdapterDesc = {};
	uint32_t adapterDecscNum = 1;
	NRI_ABORT_ON_FAILURE(nri::nriEnumerateAdapters(&bestAdapterDesc, adapterDecscNum));

	nri::DeviceCreationDesc deviceCreationDesc = {};
	deviceCreationDesc.graphicsAPI = graphicsApi;
	deviceCreationDesc.enableGraphicsAPIValidation = m_DebugAPI;
	deviceCreationDesc.enableNRIValidation = m_DebugNRI;
	deviceCreationDesc.enableD3D11CommandBufferEmulation =
		D3D11_COMMANDBUFFER_EMULATION;
	deviceCreationDesc.spirvBindingOffsets = SPIRV_BINDING_OFFSETS;
	deviceCreationDesc.adapterDesc = &bestAdapterDesc;
	deviceCreationDesc.allocationCallbacks = m_AllocationCallbacks;
	NRI_ABORT_ON_FAILURE(nri::nriCreateDevice(deviceCreationDesc, m_Device));

	NRI_ABORT_ON_FAILURE(nri::nriGetInterface(*m_Device, NRI_INTERFACE(nri::CoreInterface), (nri::CoreInterface*)&NRI));
	NRI_ABORT_ON_FAILURE(nri::nriGetInterface(*m_Device, NRI_INTERFACE(nri::HelperInterface), (nri::HelperInterface*)&NRI));
	NRI_ABORT_ON_FAILURE(nri::nriGetInterface(*m_Device, NRI_INTERFACE(nri::StreamerInterface), (nri::StreamerInterface*)&NRI));
	NRI_ABORT_ON_FAILURE(nri::nriGetInterface(*m_Device, NRI_INTERFACE(nri::SwapChainInterface), (nri::SwapChainInterface*)&NRI));

	nri::StreamerDesc streamerDesc = {};
	streamerDesc.dynamicBufferMemoryLocation = nri::MemoryLocation::HOST_UPLOAD;
	streamerDesc.dynamicBufferUsageBits = nri::BufferUsageBits::VERTEX_BUFFER |
		nri::BufferUsageBits::INDEX_BUFFER;
	streamerDesc.constantBufferMemoryLocation =
		nri::MemoryLocation::HOST_UPLOAD;
	streamerDesc.frameInFlightNum = BUFFERED_FRAME_MAX_NUM;


	NRI_ABORT_ON_FAILURE(NRI.CreateStreamer(*m_Device, streamerDesc, m_Streamer));




	// Command queue
	NRI_ABORT_ON_FAILURE(NRI.GetCommandQueue(
		*m_Device, nri::CommandQueueType::GRAPHICS, m_CommandQueue));

	// Fences
	NRI_ABORT_ON_FAILURE(NRI.CreateFence(*m_Device, 0, m_FrameFence));


	nri::Format swapChainFormat;
	{
		nri::SwapChainDesc swapChainDesc = {};
		swapChainDesc.window = m_NRIWindow;
		swapChainDesc.commandQueue = m_CommandQueue;
		swapChainDesc.format = nri::SwapChainFormat::BT709_G22_8BIT;
		swapChainDesc.verticalSyncInterval = m_VsyncInterval;
		swapChainDesc.width = (uint16_t)m_RenderOutputWidth;
		swapChainDesc.height = (uint16_t)m_RenderOutputHeight;
		swapChainDesc.textureNum = SWAP_CHAIN_TEXTURE_NUM;
		NRI_ABORT_ON_FAILURE(
			NRI.CreateSwapChain(*m_Device, swapChainDesc, m_SwapChain));

		uint32_t swapChainTextureNum;
		nri::Texture* const* swapChainTextures =
			NRI.GetSwapChainTextures(*m_SwapChain, swapChainTextureNum);
		swapChainFormat = NRI.GetTextureDesc(*swapChainTextures[0]).format;

		for (uint32_t i = 0; i < swapChainTextureNum; i++)
		{
			nri::Texture2DViewDesc textureViewDesc = {
				swapChainTextures[i], nri::Texture2DViewType::COLOR_ATTACHMENT,
				swapChainFormat };

			nri::Descriptor* colorAttachment;
			NRI_ABORT_ON_FAILURE(
				NRI.CreateTexture2DView(textureViewDesc, colorAttachment));

			const BackBuffer backBuffer = { colorAttachment,
										   swapChainTextures[i] };
			m_SwapChainBuffers.push_back(backBuffer);
		}
	}




	for (Frame& frame : m_Frames)
	{
		NRI_ABORT_ON_FAILURE(NRI.CreateCommandAllocator(
			*m_CommandQueue, frame.commandAllocator));
		NRI_ABORT_ON_FAILURE(NRI.CreateCommandBuffer(*frame.commandAllocator,
			frame.commandBuffer));
	}


	const nri::DeviceDesc& deviceDesc = NRI.GetDeviceDesc(*m_Device);
	ShaderCodeStorage shaderCodeStorage;
	{
		nri::DescriptorRangeDesc descriptorRangeConstant[1];
		descriptorRangeConstant[0] = {
			0, 1, nri::DescriptorType::CONSTANT_BUFFER, nri::StageBits::ALL };

		nri::DescriptorRangeDesc descriptorRangeTexture[2];
		descriptorRangeTexture[0] = { 0, 1, nri::DescriptorType::TEXTURE,
									 nri::StageBits::FRAGMENT_SHADER };
		descriptorRangeTexture[1] = { 0, 1, nri::DescriptorType::SAMPLER,
									 nri::StageBits::FRAGMENT_SHADER };

		nri::DescriptorSetDesc descriptorSetDescs[] = {
			{0, descriptorRangeConstant,
			 GetCountOf(descriptorRangeConstant)},
			{1, descriptorRangeTexture,
			 GetCountOf(descriptorRangeTexture)},
		};

		nri::RootConstantDesc rootConstant = { 1, sizeof(float),
											  nri::StageBits::FRAGMENT_SHADER };

		nri::PipelineLayoutDesc pipelineLayoutDesc = {};
		pipelineLayoutDesc.descriptorSetNum =
			GetCountOf(descriptorSetDescs);
		pipelineLayoutDesc.descriptorSets = descriptorSetDescs;
		pipelineLayoutDesc.rootConstantNum = 1;
		pipelineLayoutDesc.rootConstants = &rootConstant;
		pipelineLayoutDesc.shaderStages =
			nri::StageBits::VERTEX_SHADER | nri::StageBits::FRAGMENT_SHADER;

		NRI_ABORT_ON_FAILURE(NRI.CreatePipelineLayout(
			*m_Device, pipelineLayoutDesc, m_PipelineLayout));

		nri::VertexStreamDesc vertexStreamDesc = {};
		vertexStreamDesc.bindingSlot = 0;
		vertexStreamDesc.stride = sizeof(Vertex);


		nri::VertexAttributeDesc vertexAttributeDesc[2] = {};
		{
			vertexAttributeDesc[0].format = nri::Format::RG32_SFLOAT;
			vertexAttributeDesc[0].streamIndex = 0;
			vertexAttributeDesc[0].offset =
				GetOffsetOf(&Vertex::position);
			vertexAttributeDesc[0].d3d = { "POSITION", 0 };
			vertexAttributeDesc[0].vk.location = { 0 };

			vertexAttributeDesc[1].format = nri::Format::RG32_SFLOAT;
			vertexAttributeDesc[1].streamIndex = 0;
			vertexAttributeDesc[1].offset = GetOffsetOf(&Vertex::uv);
			vertexAttributeDesc[1].d3d = { "TEXCOORD", 0 };
			vertexAttributeDesc[1].vk.location = { 1 };
		}

		nri::VertexInputDesc vertexInputDesc = {};
		vertexInputDesc.attributes = vertexAttributeDesc;
		vertexInputDesc.attributeNum =
			(uint8_t)GetCountOf(vertexAttributeDesc);
		vertexInputDesc.streams = &vertexStreamDesc;
		vertexInputDesc.streamNum = 1;

		nri::InputAssemblyDesc inputAssemblyDesc = {};
		inputAssemblyDesc.topology = nri::Topology::TRIANGLE_LIST;

		nri::RasterizationDesc rasterizationDesc = {};
		rasterizationDesc.viewportNum = 1;
		rasterizationDesc.fillMode = nri::FillMode::SOLID;
		rasterizationDesc.cullMode = nri::CullMode::NONE;

		nri::ColorAttachmentDesc colorAttachmentDesc = {};
		colorAttachmentDesc.format = swapChainFormat;
		colorAttachmentDesc.colorWriteMask = nri::ColorWriteBits::RGBA;
		colorAttachmentDesc.blendEnabled = true;
		colorAttachmentDesc.colorBlend = { nri::BlendFactor::SRC_ALPHA,
										  nri::BlendFactor::ONE_MINUS_SRC_ALPHA,
										  nri::BlendFunc::ADD };

		nri::OutputMergerDesc outputMergerDesc = {};
		outputMergerDesc.colors = &colorAttachmentDesc;
		outputMergerDesc.colorNum = 1;


		nri::ShaderDesc shaderStages[] = {
				LoadShader(deviceDesc.graphicsAPI, "Triangle.vs",
								  shaderCodeStorage),
				LoadShader(deviceDesc.graphicsAPI, "Triangle.fs",
								  shaderCodeStorage),
		};



		nri::GraphicsPipelineDesc graphicsPipelineDesc = {};
		graphicsPipelineDesc.pipelineLayout = m_PipelineLayout;
		graphicsPipelineDesc.vertexInput = &vertexInputDesc;
		graphicsPipelineDesc.inputAssembly = inputAssemblyDesc;
		graphicsPipelineDesc.rasterization = rasterizationDesc;
		graphicsPipelineDesc.outputMerger = outputMergerDesc;
		graphicsPipelineDesc.shaders = shaderStages;
		graphicsPipelineDesc.shaderNum = GetCountOf(shaderStages);

		NRI_ABORT_ON_FAILURE(NRI.CreateGraphicsPipeline(*m_Device, graphicsPipelineDesc, m_Pipeline));


	}

	{ // Descriptor pool
		nri::DescriptorPoolDesc descriptorPoolDesc = {};
		descriptorPoolDesc.descriptorSetMaxNum = BUFFERED_FRAME_MAX_NUM + 1;
		descriptorPoolDesc.constantBufferMaxNum = BUFFERED_FRAME_MAX_NUM;
		descriptorPoolDesc.textureMaxNum = 1;
		descriptorPoolDesc.samplerMaxNum = 1;

		NRI_ABORT_ON_FAILURE(NRI.CreateDescriptorPool(
			*m_Device, descriptorPoolDesc, m_DescriptorPool));
	}

	Texture texture;

	std::string path =
		GetFullPath("wood.dds", DataFolder::TEXTURES);
	if (!LoadTexture(path, texture))
		return false;


	// Resources
	const uint32_t constantBufferSize = Align((uint32_t)sizeof(ConstantBufferLayout), deviceDesc.constantBufferOffsetAlignment);
	const uint64_t indexDataSize = sizeof(g_IndexData);
	const uint64_t indexDataAlignedSize = Align(indexDataSize, 16);
	const uint64_t vertexDataSize = sizeof(g_VertexData);


	{
		// Texture
		nri::TextureDesc textureDesc = {};
		textureDesc.type = nri::TextureType::TEXTURE_2D;
		textureDesc.usage = nri::TextureUsageBits::SHADER_RESOURCE;
		textureDesc.format = texture.GetFormat();
		textureDesc.width = texture.GetWidth();
		textureDesc.height = texture.GetHeight();
		textureDesc.mipNum = texture.GetMipNum();

		NRI_ABORT_ON_FAILURE(NRI.CreateTexture(*m_Device, textureDesc, m_Texture));

		{ // Constant buffer
			nri::BufferDesc bufferDesc = {};
			bufferDesc.size = constantBufferSize * BUFFERED_FRAME_MAX_NUM;
			bufferDesc.usage = nri::BufferUsageBits::CONSTANT_BUFFER;
			NRI_ABORT_ON_FAILURE(NRI.CreateBuffer(*m_Device, bufferDesc, m_ConstantBuffer));
		}

		{ // Geometry buffer
			nri::BufferDesc bufferDesc = {};
			bufferDesc.size = indexDataAlignedSize + vertexDataSize;
			bufferDesc.usage = nri::BufferUsageBits::VERTEX_BUFFER | nri::BufferUsageBits::INDEX_BUFFER;
			NRI_ABORT_ON_FAILURE(NRI.CreateBuffer(*m_Device, bufferDesc, m_GeometryBuffer));
		}
		m_GeometryOffset = indexDataAlignedSize;
	}


	nri::ResourceGroupDesc resourceGroupDesc = {};
	resourceGroupDesc.memoryLocation = nri::MemoryLocation::HOST_UPLOAD;
	resourceGroupDesc.bufferNum = 1;
	resourceGroupDesc.buffers = &m_ConstantBuffer;

	m_MemoryAllocations.resize(1, nullptr);
	NRI_ABORT_ON_FAILURE(NRI.AllocateAndBindMemory(*m_Device, resourceGroupDesc, m_MemoryAllocations.data()));


	resourceGroupDesc.memoryLocation = nri::MemoryLocation::DEVICE;
	resourceGroupDesc.bufferNum = 1;
	resourceGroupDesc.buffers = &m_GeometryBuffer;
	resourceGroupDesc.textureNum = 1;
	resourceGroupDesc.textures = &m_Texture;


	m_MemoryAllocations.resize(1 + NRI.CalculateAllocationNumber(*m_Device, resourceGroupDesc), nullptr);
	NRI_ABORT_ON_FAILURE(NRI.AllocateAndBindMemory(*m_Device, resourceGroupDesc, m_MemoryAllocations.data() + 1));

	{ // Descriptors
	   // Texture
		nri::Texture2DViewDesc texture2DViewDesc = { m_Texture, nri::Texture2DViewType::SHADER_RESOURCE_2D, texture.GetFormat() };
		NRI_ABORT_ON_FAILURE(NRI.CreateTexture2DView(texture2DViewDesc, m_TextureShaderResource));

		// Sampler
		nri::SamplerDesc samplerDesc = {};
		samplerDesc.addressModes = { nri::AddressMode::MIRRORED_REPEAT, nri::AddressMode::MIRRORED_REPEAT };
		samplerDesc.filters = { nri::Filter::LINEAR, nri::Filter::LINEAR, nri::Filter::LINEAR };
		samplerDesc.anisotropy = 4;
		samplerDesc.mipMax = 16.0f;
		NRI_ABORT_ON_FAILURE(NRI.CreateSampler(*m_Device, samplerDesc, m_Sampler));

		// Constant buffer
		for (uint32_t i = 0; i < BUFFERED_FRAME_MAX_NUM; i++) {
			nri::BufferViewDesc bufferViewDesc = {};
			bufferViewDesc.buffer = m_ConstantBuffer;
			bufferViewDesc.viewType = nri::BufferViewType::CONSTANT;
			bufferViewDesc.offset = i * constantBufferSize;
			bufferViewDesc.size = constantBufferSize;
			NRI_ABORT_ON_FAILURE(NRI.CreateBufferView(bufferViewDesc, m_Frames[i].constantBufferView));

			m_Frames[i].constantBufferViewOffset = bufferViewDesc.offset;
		}
	}

	{ // Descriptor sets
		// Texture
		NRI_ABORT_ON_FAILURE(NRI.AllocateDescriptorSets(*m_DescriptorPool, *m_PipelineLayout, 1,
			&m_TextureDescriptorSet, 1, 0));

		nri::DescriptorRangeUpdateDesc descriptorRangeUpdateDescs[2] = {};
		descriptorRangeUpdateDescs[0].descriptorNum = 1;
		descriptorRangeUpdateDescs[0].descriptors = &m_TextureShaderResource;

		descriptorRangeUpdateDescs[1].descriptorNum = 1;
		descriptorRangeUpdateDescs[1].descriptors = &m_Sampler;
		NRI.UpdateDescriptorRanges(*m_TextureDescriptorSet, 0, GetCountOf(descriptorRangeUpdateDescs), descriptorRangeUpdateDescs);

		// Constant buffer
		for (Frame& frame : m_Frames) {
			NRI_ABORT_ON_FAILURE(NRI.AllocateDescriptorSets(*m_DescriptorPool, *m_PipelineLayout, 0, &frame.constantBufferDescriptorSet, 1, 0));

			nri::DescriptorRangeUpdateDesc descriptorRangeUpdateDesc = { &frame.constantBufferView, 1 };
			NRI.UpdateDescriptorRanges(*frame.constantBufferDescriptorSet, 0, 1, &descriptorRangeUpdateDesc);
		}
	}


	{ // Upload data
		std::vector<uint8_t> geometryBufferData(indexDataAlignedSize + vertexDataSize);
		memcpy(&geometryBufferData[0], g_IndexData, indexDataSize);
		memcpy(&geometryBufferData[indexDataAlignedSize], g_VertexData, vertexDataSize);

		std::array<nri::TextureSubresourceUploadDesc, 16> subresources;
		for (uint32_t mip = 0; mip < texture.GetMipNum(); mip++)
			texture.GetSubresource(subresources[mip], mip);

		nri::TextureUploadDesc textureData = {};
		textureData.subresources = subresources.data();
		textureData.texture = m_Texture;
		textureData.after = { nri::AccessBits::SHADER_RESOURCE, nri::Layout::SHADER_RESOURCE };

		nri::BufferUploadDesc bufferData = {};
		bufferData.buffer = m_GeometryBuffer;
		bufferData.data = &geometryBufferData[0];
		bufferData.dataSize = geometryBufferData.size();
		bufferData.after = { nri::AccessBits::INDEX_BUFFER | nri::AccessBits::VERTEX_BUFFER };

		NRI_ABORT_ON_FAILURE(NRI.UploadData(*m_CommandQueue, &textureData, 1, &bufferData, 1));
	}
	return true;
}

bool Sample::Create(int _argc, char** _argv)
{

	glfwSetErrorCallback(GLFW_ErrorCallback);


	if (!glfwInit()) {
		return false;
	}


	GLFWmonitor* monitor = glfwGetPrimaryMonitor();

	float contentScale = 1.0f;
	if (m_DpiMode != 0) {
		float unused;
		glfwGetMonitorContentScale(monitor, &contentScale, &unused);
		printf("DPI scale %.1f%% (%s)\n", contentScale * 100.0f, m_DpiMode == 2 ? "quality" : "performance");
	}
	m_RenderWindowWidth = (uint32_t)floor(m_RenderOutputWidth * contentScale);
	m_RenderWindowHeight = (uint32_t)floor(m_RenderOutputHeight * contentScale);

	const GLFWvidmode* vidmode = glfwGetVideoMode(monitor);
	const uint32_t screenW = (uint32_t)vidmode->width;
	const uint32_t screenH = (uint32_t)vidmode->height;

	m_RenderWindowWidth = min(m_RenderWindowWidth, screenW);
	m_RenderWindowHeight = min(m_RenderWindowHeight, screenH);

	// Rendering output size
	m_RenderOutputWidth = min(m_RenderOutputWidth, m_RenderWindowWidth);
	m_RenderOutputHeight = min(m_RenderOutputHeight, m_RenderWindowHeight);

	if (m_DpiMode == 2) {
		m_RenderOutputWidth = m_RenderWindowWidth;
		m_RenderOutputHeight = m_RenderWindowHeight;

	}

	bool decorated = m_RenderWindowWidth != screenW && m_RenderWindowHeight != screenH;

	printf("Creating %swindow (%u, %u)\n", decorated ? "" : "borderless ", m_RenderWindowWidth, m_RenderWindowHeight);

	int32_t x = (screenW - m_RenderWindowWidth) >> 1;
	int32_t y = (screenH - m_RenderWindowHeight) >> 1;

	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_DECORATED, decorated ? 1 : 0);
	glfwWindowHint(GLFW_RESIZABLE, 0);
	glfwWindowHint(GLFW_POSITION_X, x);
	glfwWindowHint(GLFW_POSITION_Y, y);

	char windowName[256];
	snprintf(windowName, sizeof(windowName), "%s [%s]", "MyBestRender", "D3D12");

	m_Window = glfwCreateWindow(m_RenderWindowWidth, m_RenderWindowHeight, windowName, NULL, NULL);
	if (!m_Window) {
		glfwTerminate();
		return false;
	}

#if _WIN32
	m_NRIWindow.windows.hwnd = glfwGetWin32Window(m_Window);
#elif __linux__
	m_NRIWindow.x11.dpy = glfwGetX11Display();
	m_NRIWindow.x11.window = glfwGetX11Window(m_Window);
#endif


	//elizoorg 01.11.2024
	//TODO: Add glfw window callbacks
	return false;
}

class Test
{
public:
	Test();

	void Update();
	ID TryCreateBody();

	PhysicsEngine* GetPhys();

private:
	PhysicsEngine* physEngine;
};

Test::Test()
	: physEngine(PhysicsEngine::Get())
{
	physEngine->Initialize();
	physEngine->SetCollisionRule(CollisionLayer::PLAYER, CollisionLayer::MOVING, true);
	physEngine->SetCollisionRule(CollisionLayer::PLAYER, CollisionLayer::NON_MOVING, true);
}


void Test::Update()
{
	physEngine->UpdatePhysics();
}


ID Test::TryCreateBody()
{
	JPH::BoxShapeSettings test(JPH::Vec3(1, 1, 1));
	JPH::BodyCreationSettings bodySettings(
		test.Create().Get(), JPH::Vec3(0, 0, 0), JPH::Quat(0, 0, 0, 1),
		JPH::EMotionType::Dynamic,
		ToJoltLayer(CollisionLayer::MOVING));
	
	return physEngine->CreateBody(bodySettings, 50000.0f);
}


PhysicsEngine* Test::GetPhys()
{
	return physEngine;
}


int main(int argc, char** argv) {
	nri::GraphicsAPI api = nri::GraphicsAPI::VK;

	Sample sample;
	Test test;
	auto collisions = collisionLayersCount;
	auto bpLayers = broadPhaseLayersCount;
	ID testBodyId = test.TryCreateBody();

	sample.Create(argc, argv);
	bool startup = sample.Initialize(api);
	while (startup) {
		sample.RenderFrame();
		test.Update();

		JPH::Vec3 location = test.GetPhys()->GetBodyTransform(testBodyId).first;
		std::cout << location.GetX() << " " << location.GetY() << " " << location.GetZ() << std::endl;
	}
	return 0;
}

