#include "MainRenderPass.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "spdlog/spdlog.h"

constexpr uint32_t BuFFERED_FRAME_MAX_NUM = 2;
constexpr float CLEAR_DEPTH = 0.0f;

bool MainRenderPass::Init(InitParams& params)
{

	const nri::DeviceDesc& deviceDesc =
		params.NRI.GetDeviceDesc(params.m_Device);

	m_Scene = *params.scene;
	m_DepthFormat =
		nri::GetSupportedDepthFormat(params.NRI, params.m_Device, 24, true);

	ShaderCodeStorage shaderCodeStorage;
	{
		nri::DescriptorRangeDesc descriptorRangeConstant[2];
		descriptorRangeConstant[0] = {0, 1, nri::DescriptorType::CONSTANT_BUFFER, nri::StageBits::ALL};
		descriptorRangeConstant[1] = {0, 1, nri::DescriptorType::SAMPLER,nri::StageBits::FRAGMENT_SHADER};

		nri::DescriptorRangeDesc materialDescriptorRange[1];
		materialDescriptorRange[0] = {0, 4, nri::DescriptorType::TEXTURE,
		                              nri::StageBits::FRAGMENT_SHADER};


		nri::DescriptorSetDesc descriptorSetDescs[] = {
			{0, descriptorRangeConstant, GetCountOf(descriptorRangeConstant)},
			{1, materialDescriptorRange, GetCountOf(materialDescriptorRange)},
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
		                                    m_PipelineLayout) !=
		    nri::Result::SUCCESS)
		{
			return false;
		};

		nri::VertexStreamDesc vertexStreamDesc = {};
		vertexStreamDesc.bindingSlot = 0;
		vertexStreamDesc.stride = sizeof(Vertex);

		nri::VertexAttributeDesc vertexAttributeDesc[4] = {};
		{
			vertexAttributeDesc[0].format = nri::Format::RGB32_SFLOAT;
			vertexAttributeDesc[0].offset = GetOffsetOf(&Vertex::position);
			vertexAttributeDesc[0].d3d = {"POSITION", 0};
			vertexAttributeDesc[0].vk = {0};

			vertexAttributeDesc[1].format = nri::Format::RG16_SFLOAT;
			vertexAttributeDesc[1].offset = GetOffsetOf(&Vertex::uv);
			vertexAttributeDesc[1].d3d = {"TEXCOORD", 0};
			vertexAttributeDesc[1].vk = {1};

			vertexAttributeDesc[2].format = nri::Format::R10_G10_B10_A2_UNORM;
			vertexAttributeDesc[2].offset = GetOffsetOf(&Vertex::N);
			vertexAttributeDesc[2].d3d = {"NORMAL", 0};
			vertexAttributeDesc[2].vk = {2};

			vertexAttributeDesc[3].format = nri::Format::R10_G10_B10_A2_UNORM;
			vertexAttributeDesc[3].offset = GetOffsetOf(&Vertex::T);
			vertexAttributeDesc[3].d3d = {"TANGENT", 0};
			vertexAttributeDesc[3].vk = {3};
		}

		nri::VertexInputDesc vertexInputDesc = {};
		vertexInputDesc.attributes = vertexAttributeDesc;
		vertexInputDesc.attributeNum = (uint8_t)GetCountOf(vertexAttributeDesc);
		vertexInputDesc.streams = &vertexStreamDesc;
		vertexInputDesc.streamNum = 1;

		nri::InputAssemblyDesc inputAssemblyDesc = {};
		inputAssemblyDesc.topology = nri::Topology::TRIANGLE_LIST;

		nri::RasterizationDesc rasterizationDesc = {};
		rasterizationDesc.fillMode = nri::FillMode::SOLID;
		rasterizationDesc.cullMode = nri::CullMode::FRONT;
		rasterizationDesc.frontCounterClockwise = true;

		nri::ColorAttachmentDesc colorAttachmentDesc = {};
		colorAttachmentDesc.format = params.renderTargetFormat;
		colorAttachmentDesc.colorWriteMask = nri::ColorWriteBits::RGBA;

		nri::OutputMergerDesc outputMergerDesc = {};
		outputMergerDesc.colors = &colorAttachmentDesc;
		outputMergerDesc.colorNum = 1;
		outputMergerDesc.depthStencilFormat = m_DepthFormat;
		outputMergerDesc.depth.write = true;
		outputMergerDesc.depth.compareFunc = nri::CompareFunc::LESS;

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
				params.m_Device, graphicsPipelineDesc, m_Pipeline) !=
		    nri::Result::SUCCESS)
		{
			return false;
		};
	}

	
	// Load texture
	Texture texture;

	std::string path = GetFullPath("Cat_diffuse.jpg", DataFolder::TEXTURES);
	if (!LoadTexture(path, texture))
		return false;

	int materialNum = params.scene->materials_.size();
	int textureNum = params.scene->textures.size();


	// Resources
	const uint32_t constantBufferSize =
		Align((uint32_t)sizeof(ConstantBufferLayout),
	          deviceDesc.constantBufferOffsetAlignment);
	nri::Texture* depthTexture = nullptr;
	{

		{
			for (const Texture* textureData : params.scene->textures)
			{
				nri::TextureDesc textureDesc = {};
				textureDesc.type = nri::TextureType::TEXTURE_2D;
				textureDesc.usage = nri::TextureUsageBits::SHADER_RESOURCE;
				textureDesc.format = textureData->GetFormat();
				textureDesc.width = textureData->GetWidth();
				textureDesc.height = textureData->GetHeight();
				textureDesc.mipNum = textureData->GetMipNum();
				textureDesc.layerNum = textureData->GetArraySize();
				nri::Texture* texture;

				if (params.NRI.CreateTexture(params.m_Device, textureDesc,
				                             texture) == nri::Result::SUCCESS)

				{
					m_Textures.push_back(texture);
				}
				else
				{
					std::cout << "cannot make a texture\n";
				}
			}
		}
		{
			// Texture
			nri::TextureDesc textureDesc = {};
			textureDesc.type = nri::TextureType::TEXTURE_2D;
			textureDesc.usage = nri::TextureUsageBits::SHADER_RESOURCE;
			textureDesc.format = texture.GetFormat();
			textureDesc.width = texture.GetWidth();
			textureDesc.height = texture.GetHeight();
			textureDesc.mipNum = texture.GetMipNum();
			nri::Texture* texture;
			if (params.NRI.CreateTexture(params.m_Device, textureDesc,
			                             texture) != nri::Result::SUCCESS)
			{
				return false;
			};
			m_Textures.push_back(texture);
		}

		// FIXME: Hardcoded values

		{
			nri::TextureDesc textureDesc = {};
			textureDesc.type = nri::TextureType::TEXTURE_2D;
			textureDesc.usage = nri::TextureUsageBits::DEPTH_STENCIL_ATTACHMENT;
			textureDesc.format = m_DepthFormat;
			textureDesc.width = (uint16_t)1920;
			textureDesc.height = (uint16_t)1080;
			textureDesc.mipNum = 1;

			if (params.NRI.CreateTexture(params.m_Device, textureDesc,
			                             depthTexture) != nri::Result::SUCCESS)
			{
				return false;
			};

			m_Textures.push_back(depthTexture);
		}

		{
			m_Textures.push_back(params.outputTexture);
		}

		{ // Constant buffer
			nri::BufferDesc bufferDesc = {};
			bufferDesc.size = constantBufferSize * BUFFERED_FRAME_MAX_NUM;
			bufferDesc.usage = nri::BufferUsageBits::CONSTANT_BUFFER;
			nri::Buffer* buffer;
			if (params.NRI.CreateBuffer(params.m_Device, bufferDesc, buffer) !=
			    nri::Result::SUCCESS)
			{
				return false;
			};
			m_Buffers.push_back(buffer);

			// Index buffer
			bufferDesc.size = GetByteSizeOf(m_Scene.indices);
			bufferDesc.usage = nri::BufferUsageBits::INDEX_BUFFER;
			if (params.NRI.CreateBuffer(params.m_Device, bufferDesc, buffer) !=
			    nri::Result::SUCCESS)
			{
				return false;
			};
			m_Buffers.push_back(buffer);
			// Vertex buffer
			bufferDesc.size = GetByteSizeOf(m_Scene.vertices);
			bufferDesc.usage = nri::BufferUsageBits::VERTEX_BUFFER;
			if (params.NRI.CreateBuffer(params.m_Device, bufferDesc, buffer) !=
			    nri::Result::SUCCESS)
			{
				return false;
			};
			m_Buffers.push_back(buffer);
		}
	}

	{
		// From this we should make a arrays of nri::Texture and upload them
		// So, idk why we can't make Texture?
		nri::ResourceGroupDesc resourceGroupDesc = {};

		resourceGroupDesc.bufferNum = 1;
		resourceGroupDesc.buffers = &m_Buffers[0];
		resourceGroupDesc.memoryLocation = nri::MemoryLocation::HOST_UPLOAD;

		size_t baseAllocation = m_MemoryAllocations.size();
		m_MemoryAllocations.resize(baseAllocation + 1);

		if (params.helperInterface.AllocateAndBindMemory(
				params.m_Device, resourceGroupDesc,
				m_MemoryAllocations.data() + baseAllocation) !=
		    nri::Result::SUCCESS)
		{
			return false;
		};

		resourceGroupDesc.bufferNum = 2;
		resourceGroupDesc.buffers = &m_Buffers[1];
		resourceGroupDesc.textures = m_Textures.data();
		resourceGroupDesc.memoryLocation = nri::MemoryLocation::DEVICE;
		resourceGroupDesc.textureNum = (uint32_t)m_Textures.size();

		baseAllocation = m_MemoryAllocations.size();
		const size_t allocationNum =
			params.helperInterface.CalculateAllocationNumber(params.m_Device,
		                                                     resourceGroupDesc);
		m_MemoryAllocations.resize(baseAllocation + allocationNum);
		if (params.helperInterface.AllocateAndBindMemory(
				params.m_Device, resourceGroupDesc,
				m_MemoryAllocations.data() + baseAllocation) !=
		    nri::Result::SUCCESS)
		{
			return false;
		};
	}

	nri::Descriptor* constantBufferViews[BUFFERED_FRAME_MAX_NUM];
	{ // Descriptors
	  // Texture
		nri::Texture2DViewDesc texture2DViewDesc = {
			m_Textures[0], nri::Texture2DViewType::SHADER_RESOURCE_2D,
			texture.GetFormat()};
		if (params.NRI.CreateTexture2DView(texture2DViewDesc,
		                                   m_TextureShaderResource) !=
		    nri::Result::SUCCESS)
		{
			return false;
		};

		params.NRI.SetDebugName(params.outputTexture, "SceneTexture");

		// Output Texture
		nri::Descriptor* output;
		nri::Texture2DViewDesc outputTexture2DViewDesc = {
			params.outputTexture,
			nri::Texture2DViewType::COLOR_ATTACHMENT,
			params.renderTargetFormat,
			0,
			1,
			0,
			1};
		if (params.NRI.CreateTexture2DView(outputTexture2DViewDesc, output) !=
		    nri::Result::SUCCESS)
		{
			return false;
		};
		outputDesc = output;

		nri::Descriptor* output2;

		nri::Texture2DViewDesc outputTexture2DViewDesc2 = {
			params.outputTexture,
			nri::Texture2DViewType::SHADER_RESOURCE_2D,
			params.renderTargetFormat,
			0,
			1,
			0,
			1};
		if (params.NRI.CreateTexture2DView(outputTexture2DViewDesc2, output2) !=
		    nri::Result::SUCCESS)
		{
			return false;
		};

		outputDesc2 = output2;

		{
			m_Descriptors.resize(textureNum);
			for (int i = 0; i < textureNum; i++)
			{
				Texture& texture = *params.scene->textures[i];

				nri::Texture2DViewDesc texture2DViewDesc = {
					m_Textures[i], nri::Texture2DViewType::SHADER_RESOURCE_2D,
					texture.GetFormat()};
				if (params.NRI.CreateTexture2DView(texture2DViewDesc,
				                                   m_Descriptors[i]) !=
				    nri::Result::SUCCESS)
				{
					spdlog::error("cannot make a texture descriptor");
					return false;
				}
			}
		}

		// Sampler
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

		// Constant buffer
		for (uint32_t i = 0; i < BUFFERED_FRAME_MAX_NUM; i++)
		{
			nri::BufferViewDesc bufferViewDesc = {};
			bufferViewDesc.buffer = m_Buffers[CONSTANT_BUFFER];
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

		{
			// Depth buffer
			nri::Texture2DViewDesc texture2DViewDesc = {
				depthTexture, nri::Texture2DViewType::DEPTH_STENCIL_ATTACHMENT,
				m_DepthFormat};

			if (params.NRI.CreateTexture2DView(texture2DViewDesc,
			                                   m_DepthAttachment) !=
			    nri::Result::SUCCESS)
			{
				return false;
			};
			m_Descriptors.push_back(m_DepthAttachment);
		}
	}

	{ // Descriptor pool
		nri::DescriptorPoolDesc descriptorPoolDesc = {};
		descriptorPoolDesc.descriptorSetMaxNum = BUFFERED_FRAME_MAX_NUM + materialNum;
		descriptorPoolDesc.textureMaxNum = materialNum*4;
		descriptorPoolDesc.samplerMaxNum = BUFFERED_FRAME_MAX_NUM;
		descriptorPoolDesc.constantBufferMaxNum = BUFFERED_FRAME_MAX_NUM;

		if (params.NRI.CreateDescriptorPool(params.m_Device, descriptorPoolDesc,
		                                    m_DescriptorPool) !=
		    nri::Result::SUCCESS)
		{
			return false;
		};
	}


	{ // Descriptor sets
		//Global

		m_DescriptorSets.resize(BUFFERED_FRAME_MAX_NUM +
		                        materialNum);
		// Constant buffer
		if (params.NRI.AllocateDescriptorSets(
				*m_DescriptorPool, *m_PipelineLayout, 0, &m_DescriptorSets[0],
				2, 0) != nri::Result::SUCCESS)
		{
			return false;
		};

		for (uint32_t i = 0; i < BUFFERED_FRAME_MAX_NUM; i++)
		{

			nri::DescriptorRangeUpdateDesc descriptorRangeUpdateDesc[2] = {};
			descriptorRangeUpdateDesc[0].descriptorNum = 1;
			descriptorRangeUpdateDesc[0].descriptors = &constantBufferViews[i];
			descriptorRangeUpdateDesc[1].descriptorNum = 1;
			descriptorRangeUpdateDesc[1].descriptors = &m_Sampler;
			params.NRI.UpdateDescriptorRanges(
				*m_DescriptorSets[i], 0, GetCountOf(descriptorRangeUpdateDesc),
				descriptorRangeUpdateDesc);
		};
	}
	// Material
	{
		if (params.NRI.AllocateDescriptorSets(
				*m_DescriptorPool, *m_PipelineLayout, 1, &m_DescriptorSets[BUFFERED_FRAME_MAX_NUM],
				materialNum, 0) != nri::Result::SUCCESS)
		{
			return false;
		};


		for (uint32_t i = 0; i < materialNum;i++)
		{
			const Material& material = params.scene->materials_[i];
			nri::Descriptor* materialTextures[4] = {
				m_Descriptors[material.ambientTexIndex],
				m_Descriptors[material.diffuseTexIndex],
				m_Descriptors[material.heightTexIndex],
				m_Descriptors[material.OpacityTexIndex]};

			nri::DescriptorRangeUpdateDesc descriptorRangeUpdateDesc = {};
			descriptorRangeUpdateDesc.descriptorNum  = GetCountOf(materialTextures);
			descriptorRangeUpdateDesc.descriptors = materialTextures;
			params.NRI.UpdateDescriptorRanges(
				*m_DescriptorSets[i + BUFFERED_FRAME_MAX_NUM], 0, 1,
				&descriptorRangeUpdateDesc);
		}
	}




	{ // Upload data
		std::vector<nri::TextureUploadDesc> textureData(textureNum + 2);

		uint32_t subresourceNum = 0;
		for (uint32_t i = 0; i < textureNum; i++)
		{
			const Texture& texture = *m_Scene.textures[i];
			subresourceNum += texture.GetArraySize() * texture.GetMipNum();
		}

		std::vector<nri::TextureSubresourceUploadDesc> subresources(
			subresourceNum);
		nri::TextureSubresourceUploadDesc* subresourceBegin =
			subresources.data();


		uint32_t i = 0;
		for (;i<textureNum;i++)
		{
			const Texture& texture = *m_Scene.textures[i];
			for (uint32_t slice = 0; slice < texture.GetArraySize(); slice++)
			{

				for (uint32_t mip = 0; mip < texture.GetMipNum(); mip++)
				{
					texture.GetSubresource(subresourceBegin[slice * texture.GetMipNum() + mip],
						mip, slice);
				}

				textureData[i] = {};
				textureData[i].subresources = subresourceBegin;
				textureData[i].texture = m_Textures[i];
				textureData[i].after = {nri::AccessBits::SHADER_RESOURCE,
				                        nri::Layout::SHADER_RESOURCE};

				 subresourceBegin += texture.GetArraySize() * texture.GetMipNum();

			}
		}

		textureData[i] = {};
		textureData[i].subresources = nullptr;
		textureData[i].texture = depthTexture;
		textureData[i].after = {nri::AccessBits::DEPTH_STENCIL_ATTACHMENT_WRITE,
		                        nri::Layout::DEPTH_STENCIL_ATTACHMENT};
		i++;

		nri::BufferUploadDesc bufferData[] = {
			{m_Scene.vertices.data(),
		     GetByteSizeOf(m_Scene.vertices),
		     m_Buffers[VERTEX_BUFFER],
		     0,
		     {nri::AccessBits::VERTEX_BUFFER}},
			{m_Scene.indices.data(),
		     GetByteSizeOf(m_Scene.indices),
		     m_Buffers[INDEX_BUFFER],
		     0,
		     {nri::AccessBits::INDEX_BUFFER}}};

		if (params.helperInterface.UploadData(
				*params.commandQueue, textureData.data(), i,
				bufferData, GetCountOf(bufferData)) != nri::Result::SUCCESS)
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
                          nri::CommandBuffer& commandBuffer,
                          const nova::Frame& frame,
                          const nova::BackBuffer& currentBackBuffer,
                          const uint32_t currentTextureIndex,
                          const uint32_t m_RenderWindowWidth,
	const uint32_t m_RenderWindowHeight, nri::Descriptor* outputTextureDesc,
	nri::Texture* outputTexture)
{
	nri::Dim_t windowWidth = (nri::Dim_t)m_RenderWindowWidth;
	nri::Dim_t windowHeight = (nri::Dim_t)m_RenderWindowHeight;
	nri::Dim_t halfWidth = windowWidth / 2;
	nri::Dim_t halfHeight = windowHeight / 2;

	ConstantBufferLayout* commonConstants =
		(ConstantBufferLayout*)NRI.MapBuffer(*m_Buffers[CONSTANT_BUFFER],
	                                         frame.constantBufferViewOffset,
	                                         sizeof(ConstantBufferLayout));
	//spdlog::info("Constant buffer offset {}", frame.constantBufferViewOffset);
	m_ProjectionMatrix = glm::perspective(
		glm::radians(90.0f), (float)windowWidth / (float)windowHeight, 0.1f,
		1000.0f);


	m_ViewMatrix =
		glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));


	if (commonConstants)
	{
		commonConstants->projectionMatrix = m_ProjectionMatrix;

		commonConstants->viewMatrix = m_ViewMatrix;

		NRI.UnmapBuffer(*m_Buffers[CONSTANT_BUFFER]);
	}

	// Record
	NRI.BeginCommandBuffer(commandBuffer, m_DescriptorPool);
	{
		{ // Copy upload requests to destinations
			Annotation annotation(NRI, commandBuffer, "Streamer");

			// TODO: is barrier from "SHADER_RESOURCE" to "COPY_DESTINATION"
			// needed here for "Buffer::InstanceData"?

			//streamerInterface.CmdUploadStreamerUpdateRequests(
				//commandBuffer, streamer);
		}

		nri::TextureBarrierDesc textureBarrierDescs = {};
		textureBarrierDescs.texture = outputTexture;

		textureBarrierDescs.after = {nri::AccessBits::COLOR_ATTACHMENT,
		                             nri::Layout::COLOR_ATTACHMENT};
		textureBarrierDescs.layerNum = 1;
		textureBarrierDescs.mipNum = 1;

		nri::BarrierGroupDesc barrierGroupDesc = {};
		barrierGroupDesc.textureNum = 1;
		barrierGroupDesc.textures = &textureBarrierDescs;

		NRI.CmdBarrier(commandBuffer, barrierGroupDesc);

		nri::AttachmentsDesc attachmentsDesc = {};
		attachmentsDesc.colorNum = 1;
		attachmentsDesc.colors = &outputDesc;
		attachmentsDesc.depthStencil = m_DepthAttachment;

		constexpr nri::Color32f COLOR_0 = {1.0f, 1.0f, 0.0f, 1.0f};

		NRI.CmdBeginRendering(commandBuffer, attachmentsDesc);
		{
			{
				Annotation annotation(NRI, commandBuffer, "Clears");

				nri::ClearDesc clearDescs[2] = {};
				clearDescs[0].planes = nri::PlaneBits::COLOR;
				clearDescs[0].value.color.f = COLOR_0;
				clearDescs[1].planes = nri::PlaneBits::DEPTH;
				clearDescs[1].value.depthStencil.depth = 1;

				NRI.CmdClearAttachments(commandBuffer, clearDescs,
				                        GetCountOf(clearDescs), nullptr, 0);
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

				NRI.CmdSetDescriptorSet(commandBuffer, 0, *m_DescriptorSets[0],
				                        nullptr);
				

				nri::Rect scissor = {(int16_t)halfWidth, (int16_t)halfHeight,
				                     halfWidth, halfHeight};
				NRI.CmdSetScissors(commandBuffer, &scissor, 1);
				// NRI.CmdDraw(commandBuffer, {3, 1, 0, 0});
				scissor = {0, 0, windowWidth, windowHeight};

				NRI.CmdSetScissors(commandBuffer, &scissor, 1);

				for (const Mesh& mesh : m_Scene.meshes)
				{
					NRI.CmdSetIndexBuffer(
						commandBuffer, *m_Buffers[INDEX_BUFFER], 0,
						sizeof(uint32_t) == 2 ? nri::IndexType::UINT16
											  : nri::IndexType::UINT32);
					constexpr uint64_t offset = 0;


					nri::DescriptorSet* descriptorSet =
						m_DescriptorSets[BUFFERED_FRAME_MAX_NUM +
					                     mesh.materialNum];
					NRI.CmdSetDescriptorSet(commandBuffer, 1, *descriptorSet,
					                        nullptr);

					NRI.CmdSetVertexBuffers(commandBuffer, 0, 1,
					                        &m_Buffers[VERTEX_BUFFER], &offset);
					NRI.CmdDrawIndexed(commandBuffer,
					                   {mesh.indexNum, 1, mesh.indexOffset,
					                    (int32_t)mesh.vertexOffset, 0});
				}
			}

			{
			}
		}
		NRI.CmdEndRendering(commandBuffer);

		textureBarrierDescs.before = textureBarrierDescs.after;
		textureBarrierDescs.after = {nri::AccessBits::UNKNOWN,
		                             nri::Layout::SHADER_RESOURCE};

		barrierGroupDesc.textures = &textureBarrierDescs;

		NRI.CmdBarrier(commandBuffer, barrierGroupDesc);


	}
}

void MainRenderPass::PrepareFrame()
{
}

void MainRenderPass::Shutdown()
{
}

MainRenderPass::MainRenderPass()
{
}

MainRenderPass::~MainRenderPass()
{
}

void MainRenderPass::BeginUI()
{
}

void MainRenderPass::EndUI()
{
}
