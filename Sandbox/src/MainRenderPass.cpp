#include "MainRenderPass.h"

constexpr uint32_t BuFFERED_FRAME_MAX_NUM = 2;

bool MainRenderPass::Init(InitParams params)
{

	const nri::DeviceDesc& deviceDesc = params.NRI.GetDeviceDesc(params.m_Device);
	ShaderCodeStorage shaderCodeStorage;
	{
		nri::DescriptorRangeDesc descriptorRangeConstant[1];
		descriptorRangeConstant[0] = {
			0, 1, nri::DescriptorType::CONSTANT_BUFFER, nri::StageBits::ALL};

		nri::DescriptorRangeDesc descriptorRangeTexture[2];
		descriptorRangeTexture[0] = {0, 1, nri::DescriptorType::TEXTURE,
		                             nri::StageBits::FRAGMENT_SHADER};
		descriptorRangeTexture[1] = {0, 1, nri::DescriptorType::SAMPLER,
		                             nri::StageBits::FRAGMENT_SHADER};

		nri::DescriptorSetDesc descriptorSetDescs[] = {
			{0, descriptorRangeConstant, GetCountOf(descriptorRangeConstant)},
			{1, descriptorRangeTexture, GetCountOf(descriptorRangeTexture)},
		};

		nri::RootConstantDesc rootConstant = {1, sizeof(float),
		                                      nri::StageBits::FRAGMENT_SHADER};

		nri::PipelineLayoutDesc pipelineLayoutDesc = {};
		pipelineLayoutDesc.descriptorSetNum = GetCountOf(descriptorSetDescs);
		pipelineLayoutDesc.descriptorSets = descriptorSetDescs;
		pipelineLayoutDesc.rootConstantNum = 1;
		pipelineLayoutDesc.rootConstants = &rootConstant;
		pipelineLayoutDesc.shaderStages =
			nri::StageBits::VERTEX_SHADER | nri::StageBits::FRAGMENT_SHADER;

		if (params.NRI.CreatePipelineLayout(params.m_Device, pipelineLayoutDesc,
		                             m_PipelineLayout) != nri::Result::SUCCESS)
		{
			return false;
		};

		nri::VertexStreamDesc vertexStreamDesc = {};
		vertexStreamDesc.bindingSlot = 0;
		vertexStreamDesc.stride = sizeof(Vertex);

		nri::VertexAttributeDesc vertexAttributeDesc[2] = {};
		{
			vertexAttributeDesc[0].format = nri::Format::RG32_SFLOAT;
			vertexAttributeDesc[0].streamIndex = 0;
			vertexAttributeDesc[0].offset = GetOffsetOf(&Vertex::position);
			vertexAttributeDesc[0].d3d = {"POSITION", 0};
			vertexAttributeDesc[0].vk.location = {0};

			vertexAttributeDesc[1].format = nri::Format::RG32_SFLOAT;
			vertexAttributeDesc[1].streamIndex = 0;
			vertexAttributeDesc[1].offset = GetOffsetOf(&Vertex::uv);
			vertexAttributeDesc[1].d3d = {"TEXCOORD", 0};
			vertexAttributeDesc[1].vk.location = {1};
		}

		nri::VertexInputDesc vertexInputDesc = {};
		vertexInputDesc.attributes = vertexAttributeDesc;
		vertexInputDesc.attributeNum = (uint8_t)GetCountOf(vertexAttributeDesc);
		vertexInputDesc.streams = &vertexStreamDesc;
		vertexInputDesc.streamNum = 1;

		nri::InputAssemblyDesc inputAssemblyDesc = {};
		inputAssemblyDesc.topology = nri::Topology::TRIANGLE_LIST;

		nri::RasterizationDesc rasterizationDesc = {};
		rasterizationDesc.viewportNum = 1;
		rasterizationDesc.fillMode = nri::FillMode::SOLID;
		rasterizationDesc.cullMode = nri::CullMode::NONE;

		nri::ColorAttachmentDesc colorAttachmentDesc = {};
		colorAttachmentDesc.format = params.renderTargetFormat;
		colorAttachmentDesc.colorWriteMask = nri::ColorWriteBits::RGBA;
		colorAttachmentDesc.blendEnabled = true;
		colorAttachmentDesc.colorBlend = {nri::BlendFactor::SRC_ALPHA,
		                                  nri::BlendFactor::ONE_MINUS_SRC_ALPHA,
		                                  nri::BlendFunc::ADD};

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

		if (params.NRI.CreateGraphicsPipeline(
				params.m_Device, graphicsPipelineDesc,
		                               m_Pipeline) != nri::Result::SUCCESS)
		{
			return false;
		};
	}

	{ // Descriptor pool
		nri::DescriptorPoolDesc descriptorPoolDesc = {};
		descriptorPoolDesc.descriptorSetMaxNum = BUFFERED_FRAME_MAX_NUM + 1;
		descriptorPoolDesc.constantBufferMaxNum = BUFFERED_FRAME_MAX_NUM;
		descriptorPoolDesc.textureMaxNum = 1;
		descriptorPoolDesc.samplerMaxNum = 1;

		if (params.NRI.CreateDescriptorPool(params.m_Device, descriptorPoolDesc,
		                             m_DescriptorPool) != nri::Result::SUCCESS)
		{
			return false;
		};
	}
	//Load texture
	Texture texture;

	std::string path = GetFullPath("wood.dds", DataFolder::TEXTURES);
	if (!LoadTexture(path, texture))
		return false;

	 //Resources
	const uint32_t constantBufferSize =
		Align((uint32_t)sizeof(ConstantBufferLayout),
	          deviceDesc.constantBufferOffsetAlignment);
	const uint64_t indexDataSize = sizeof(g_IndexData);
	const uint64_t indexDataAlignedSize = Align(indexDataSize, 16);
	const uint64_t vertexDataSize = sizeof(g_VertexData);

	{
		 //Texture
		nri::TextureDesc textureDesc = {};
		textureDesc.type = nri::TextureType::TEXTURE_2D;
		textureDesc.usage = nri::TextureUsageBits::SHADER_RESOURCE;
		textureDesc.format = texture.GetFormat();
		textureDesc.width = texture.GetWidth();
		textureDesc.height = texture.GetHeight();
		textureDesc.mipNum = texture.GetMipNum();

		if (params.NRI.CreateTexture(params.m_Device, textureDesc, m_Texture) !=
		    nri::Result::SUCCESS){
			return false;
			};

		{ // Constant buffer
			nri::BufferDesc bufferDesc = {};
			bufferDesc.size = constantBufferSize * BUFFERED_FRAME_MAX_NUM;
			bufferDesc.usage = nri::BufferUsageBits::CONSTANT_BUFFER;
			if (params.NRI.CreateBuffer(params.m_Device, bufferDesc,
			                            m_ConstantBuffer) !=
			    nri::Result::SUCCESS)
			{
				return false;
			};
		}

		{ // Geometry buffer
			nri::BufferDesc bufferDesc = {};
			bufferDesc.size = indexDataAlignedSize + vertexDataSize;
			bufferDesc.usage = nri::BufferUsageBits::VERTEX_BUFFER |
			                   nri::BufferUsageBits::INDEX_BUFFER;
			if (params.NRI.CreateBuffer(params.m_Device, bufferDesc,
			                     m_GeometryBuffer) !=
			    nri::Result::SUCCESS)
			{
				return false;
			};
		}
		m_GeometryOffset = indexDataAlignedSize;
	}

	nri::ResourceGroupDesc resourceGroupDesc = {};
	resourceGroupDesc.memoryLocation = nri::MemoryLocation::HOST_UPLOAD;
	resourceGroupDesc.bufferNum = 1;
	resourceGroupDesc.buffers = &m_ConstantBuffer;

	m_MemoryAllocations.resize(1, nullptr);
	if (params.helperInterface.AllocateAndBindMemory(
			params.m_Device, resourceGroupDesc,
	                              m_MemoryAllocations.data()) !=
	    nri::Result::SUCCESS)
	{
		return false;
	};

	resourceGroupDesc.memoryLocation = nri::MemoryLocation::DEVICE;
	resourceGroupDesc.bufferNum = 1;
	resourceGroupDesc.buffers = &m_GeometryBuffer;
	resourceGroupDesc.textureNum = 1;
	resourceGroupDesc.textures = &m_Texture;

	m_MemoryAllocations.resize(
		1 + params.helperInterface.CalculateAllocationNumber(params.m_Device,
	                                                         resourceGroupDesc),
		nullptr);
	if (params.helperInterface.AllocateAndBindMemory(
			params.m_Device, resourceGroupDesc,
	                                          m_MemoryAllocations.data() + 1) !=
	    nri::Result::SUCCESS)
	{
		return false;
	};



	nri::Descriptor* constantBufferViews[BUFFERED_FRAME_MAX_NUM];
	{ // Descriptors
	   //Texture
		nri::Texture2DViewDesc texture2DViewDesc = {
			m_Texture, nri::Texture2DViewType::SHADER_RESOURCE_2D,
			texture.GetFormat()};
		if (params.NRI.CreateTexture2DView(texture2DViewDesc,
		                            m_TextureShaderResource) !=
		    nri::Result::SUCCESS)
		{
			return false;
		};

		 //Sampler
		nri::SamplerDesc samplerDesc = {};
		samplerDesc.addressModes = {nri::AddressMode::MIRRORED_REPEAT,
		                            nri::AddressMode::MIRRORED_REPEAT};
		samplerDesc.filters = {nri::Filter::LINEAR, nri::Filter::LINEAR,
		                       nri::Filter::LINEAR};
		samplerDesc.anisotropy = 4;
		samplerDesc.mipMax = 16.0f;
		if (params.NRI.CreateSampler(params.m_Device, samplerDesc, m_Sampler) !=
		    nri::Result::SUCCESS)
			;

		 //Constant buffer
		for (uint32_t i = 0; i < BUFFERED_FRAME_MAX_NUM; i++)
		{
			nri::BufferViewDesc bufferViewDesc = {};
			bufferViewDesc.buffer = m_ConstantBuffer;
			bufferViewDesc.viewType = nri::BufferViewType::CONSTANT;
			bufferViewDesc.offset = i * constantBufferSize;
			bufferViewDesc.size = constantBufferSize;
			if (params.NRI.CreateBufferView(bufferViewDesc,
			                                constantBufferViews[i]) !=
			    nri::Result::SUCCESS)
			{
				return false;
			};

			m_Descriptors.push_back(constantBufferViews[i]);
		}
	}

	{ // Descriptor sets
		 //Texture
		if (params.NRI.AllocateDescriptorSets(
				*m_DescriptorPool, *m_PipelineLayout, 1,
		                               &m_TextureDescriptorSet, 1,
		                               0) != nri::Result::SUCCESS)
		{
			return false;
		};

		nri::DescriptorRangeUpdateDesc descriptorRangeUpdateDescs[2] = {};
		descriptorRangeUpdateDescs[0].descriptorNum = 1;
		descriptorRangeUpdateDescs[0].descriptors = &m_TextureShaderResource;

		descriptorRangeUpdateDescs[1].descriptorNum = 1;
		descriptorRangeUpdateDescs[1].descriptors = &m_Sampler;
		params.NRI.UpdateDescriptorRanges(
			*m_TextureDescriptorSet, 0,
		                           GetCountOf(descriptorRangeUpdateDescs),
		                           descriptorRangeUpdateDescs);
		m_DescriptorSets.resize(BUFFERED_FRAME_MAX_NUM);
		 //Constant buffer
		for (uint32_t i = 0; i < BUFFERED_FRAME_MAX_NUM; i++)
		{
			if (params.NRI.AllocateDescriptorSets(
					*m_DescriptorPool, *m_PipelineLayout,
			                               0, &m_DescriptorSets[i],
			                               1, 0) != nri::Result::SUCCESS)
			{
				return false;
			};

			nri::DescriptorRangeUpdateDesc descriptorRangeUpdateDesc = {
				&constantBufferViews[i], 1};
			params.NRI.UpdateDescriptorRanges(*m_DescriptorSets[i], 0, 1,
			                           &descriptorRangeUpdateDesc);
		}
	}

	{ // Upload data
		std::vector<uint8_t> geometryBufferData(indexDataAlignedSize +
		                                        vertexDataSize);
		memcpy(&geometryBufferData[0], g_IndexData, indexDataSize);
		memcpy(&geometryBufferData[indexDataAlignedSize], g_VertexData,
		       vertexDataSize);

		std::array<nri::TextureSubresourceUploadDesc, 16> subresources;
		for (uint32_t mip = 0; mip < texture.GetMipNum(); mip++)
			texture.GetSubresource(subresources[mip], mip);

		nri::TextureUploadDesc textureData = {};
		textureData.subresources = subresources.data();
		textureData.texture = m_Texture;
		textureData.after = {nri::AccessBits::SHADER_RESOURCE,
		                     nri::Layout::SHADER_RESOURCE};

		nri::BufferUploadDesc bufferData = {};
		bufferData.buffer = m_GeometryBuffer;
		bufferData.data = &geometryBufferData[0];
		bufferData.dataSize = geometryBufferData.size();
		bufferData.after = {nri::AccessBits::INDEX_BUFFER |
		                    nri::AccessBits::VERTEX_BUFFER};

		if (params.helperInterface.UploadData(*params.commandQueue,
		                                      &textureData, 1,
		                                      &bufferData,
		                                      1) != nri::Result::SUCCESS)
		{
			return false;
			};
	}


return true;
}

void MainRenderPass::Init()
{
}

void MainRenderPass::Draw()
{
}

void MainRenderPass::Draw(const nri::CoreInterface& NRI,
                          const nri::StreamerInterface& streamerInterface,
						  const nri::SwapChainInterface& swapChainInterface,
                          const nri::StreamerInterface& streamer,
                          nri::CommandBuffer& commandBuffer, const Frame& frame,
                          const BackBuffer& currentBackBuffer,
                          const uint32_t currentTextureIndex,const uint32_t m_RenderWindowWidth, const uint32_t m_RenderWindowHeight)
{
	 nri::Dim_t windowWidth = (nri::Dim_t)m_RenderWindowWidth;
	 nri::Dim_t windowHeight = (nri::Dim_t)m_RenderWindowHeight;
	 nri::Dim_t halfWidth = windowWidth / 2;
	 nri::Dim_t halfHeight = windowHeight / 2;



	 ConstantBufferLayout* commonConstants =
		(ConstantBufferLayout*)NRI.MapBuffer(*m_ConstantBuffer,
	                                          frame.constantBufferViewOffset,
	                                          sizeof(ConstantBufferLayout));
	 if (commonConstants)
	{
		commonConstants->color[0] = 0.8f;
		commonConstants->color[1] = 0.5f;
		commonConstants->color[2] = 0.1f;
		commonConstants->scale = m_Scale;

		NRI.UnmapBuffer(*m_ConstantBuffer);
	}

	 nri::TextureBarrierDesc textureBarrierDescs = {};
	 textureBarrierDescs.texture = currentBackBuffer.texture;
	 textureBarrierDescs.after = {nri::AccessBits::COLOR_ATTACHMENT,
	                              nri::Layout::COLOR_ATTACHMENT};
	 textureBarrierDescs.layerNum = 1;
	 textureBarrierDescs.mipNum = 1;

	 //Record
	 NRI.BeginCommandBuffer(commandBuffer, m_DescriptorPool);
	{
		nri::BarrierGroupDesc barrierGroupDesc = {};
		barrierGroupDesc.textureNum = 1;
		barrierGroupDesc.textures = &textureBarrierDescs;
		NRI.CmdBarrier(commandBuffer, barrierGroupDesc);

		nri::AttachmentsDesc attachmentsDesc = {};
		attachmentsDesc.colorNum = 1;
		attachmentsDesc.colors = &currentBackBuffer.colorAttachment;

		constexpr nri::Color32f COLOR_0 = {1.0f, 1.0f, 0.0f, 1.0f};

		NRI.CmdBeginRendering(commandBuffer, attachmentsDesc);
		{
			{
				Annotation annotation(NRI, commandBuffer, "Clears");

				nri::ClearDesc clearDesc = {};
				clearDesc.planes = nri::PlaneBits::COLOR;
				clearDesc.value.color.f = COLOR_0;

				NRI.CmdClearAttachments(commandBuffer, &clearDesc, 1, nullptr,
				                        0);
			}

			{
				Annotation annotation(NRI, commandBuffer, "Triangle");

				const nri::Viewport viewport = {
					0.0f, 0.0f, (float)windowWidth, (float)windowHeight,
					0.0f, 1.0f};
				NRI.CmdSetViewports(commandBuffer, &viewport, 1);

				NRI.CmdSetPipelineLayout(commandBuffer, *m_PipelineLayout);
				NRI.CmdSetPipeline(commandBuffer, *m_Pipeline);
				NRI.CmdSetRootConstants(commandBuffer, 0, &m_Transparency, 4);
				NRI.CmdSetIndexBuffer(commandBuffer, *m_GeometryBuffer, 0,
				                      nri::IndexType::UINT16);
				NRI.CmdSetVertexBuffers(commandBuffer, 0, 1, &m_GeometryBuffer,
				                        &m_GeometryOffset);
				NRI.CmdSetDescriptorSet(commandBuffer, 0, *m_DescriptorSets[0],
				                        nullptr);
				NRI.CmdSetDescriptorSet(commandBuffer, 1,
				                        *m_TextureDescriptorSet, nullptr);

				nri::Rect scissor = {0, 0, windowWidth, windowHeight};
				NRI.CmdSetScissors(commandBuffer, &scissor, 1);
				NRI.CmdDrawIndexed(commandBuffer, {3, 1, 0, 0, 0});

				scissor = {(int16_t)halfWidth, (int16_t)halfHeight, halfWidth,
				           halfHeight};
				NRI.CmdSetScissors(commandBuffer, &scissor, 1);
				NRI.CmdDraw(commandBuffer, {3, 1, 0, 0});
			}

			{
			}
		}
		NRI.CmdEndRendering(commandBuffer);

		textureBarrierDescs.before = textureBarrierDescs.after;
		textureBarrierDescs.after = {nri::AccessBits::UNKNOWN,
		                             nri::Layout::PRESENT};

		NRI.CmdBarrier(commandBuffer, barrierGroupDesc);
	}
}

void MainRenderPass::PrepareFrame()
{

}

void MainRenderPass::Shutdown(){

}

MainRenderPass::MainRenderPass(){

}

MainRenderPass::~MainRenderPass(){
    
}
