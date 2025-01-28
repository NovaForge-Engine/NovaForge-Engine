#include "UIRenderPass.h"

#include "Packing.h"
#include "Utils.hpp"

void UIRenderPass::Init()
{
}

void UIRenderPass::Draw()
{
}

void UIRenderPass::Shutdown()
{
}

void UIRenderPass::PrepareFrame()
{
}

bool UIRenderPass::Init(InitParams params)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	float contentScale = 1.0f;

	ImGui_ImplGlfw_InitForOther(params.window->GetGLFWWindow(), true);

	// elizoorg 21.11.2024
	// TODO: m_DpiMode should also be provided to this function which should be
	// stored into window class
	/*if (m_DpiMode != 0)
	{
	    GLFWmonitor* monitor = glfwGetPrimaryMonitor();

	    float unused;
	    glfwGetMonitorContentScale(monitor, &contentScale, &unused);
	}*/

	ImGuiStyle& style = ImGui::GetStyle();
	style.FrameBorderSize = 1;
	style.WindowBorderSize = 1;
	style.ScaleAllSizes(contentScale);

	ImGuiIO& io = ImGui::GetIO();
	io.BackendFlags |=
		ImGuiBackendFlags_HasMouseCursors; // We can honor GetMouseCursor()
	                                       // values (optional)
	io.BackendFlags |=
		ImGuiBackendFlags_HasSetMousePos; // We can honor io.WantSetMousePos
	                                      // requests (optional, rarely used)
	io.IniFilename = nullptr;

	m_MouseCursors[ImGuiMouseCursor_Arrow] =
		glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	m_MouseCursors[ImGuiMouseCursor_TextInput] =
		glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
	m_MouseCursors[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(
		GLFW_ARROW_CURSOR); // FIXME: GLFW doesn't have this.
	m_MouseCursors[ImGuiMouseCursor_ResizeNS] =
		glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
	m_MouseCursors[ImGuiMouseCursor_ResizeEW] =
		glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
	m_MouseCursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(
		GLFW_ARROW_CURSOR); // FIXME: GLFW doesn't have this.
	m_MouseCursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(
		GLFW_ARROW_CURSOR); // FIXME: GLFW doesn't have this.
	m_MouseCursors[ImGuiMouseCursor_Hand] =
		glfwCreateStandardCursor(GLFW_HAND_CURSOR);

	const nri::DeviceDesc& deviceDesc = params.NRI.GetDeviceDesc(params.device);

	{
		nri::DescriptorRangeDesc descriptorRanges[] = {
			{0, 1, nri::DescriptorType::TEXTURE,
		     nri::StageBits::FRAGMENT_SHADER},
			{0, 1, nri::DescriptorType::SAMPLER,
		     nri::StageBits::FRAGMENT_SHADER},
		};

		nri::DescriptorSetDesc descriptorSet = {0, descriptorRanges,
		                                        GetCountOf(descriptorRanges)};

		nri::RootConstantDesc rootConstants = {};
		rootConstants.registerIndex = 0;
		rootConstants.size = 16;
		rootConstants.shaderStages = nri::StageBits::ALL;

		nri::PipelineLayoutDesc pipelineLayoutDesc = {};
		pipelineLayoutDesc.descriptorSetNum = 1;
		pipelineLayoutDesc.descriptorSets = &descriptorSet;
		pipelineLayoutDesc.rootConstantNum = 1;
		pipelineLayoutDesc.rootConstants = &rootConstants;
		pipelineLayoutDesc.shaderStages =
			nri::StageBits::VERTEX_SHADER | nri::StageBits::FRAGMENT_SHADER;

		if (params.NRI.CreatePipelineLayout(params.device, pipelineLayoutDesc,
		                                    m_PipelineLayout) !=
		    nri::Result::SUCCESS)
			return false;



		ShaderCodeStorage shaderCodeStorage;
		nri::ShaderDesc shaders[] = {
			LoadShader(deviceDesc.graphicsAPI, "UI.vs", shaderCodeStorage),
			LoadShader(deviceDesc.graphicsAPI, "UI.fs", shaderCodeStorage),
		};

		ShaderCodeStorage shaderCodeStorage2;
		nri::ShaderDesc shaders2[] = {
			LoadShader(deviceDesc.graphicsAPI, "StaticUI.vs", shaderCodeStorage2),
			LoadShader(deviceDesc.graphicsAPI, "StaticUI.fs", shaderCodeStorage2),
		};

		nri::VertexStreamDesc vertexStreamDesc = {};
		vertexStreamDesc.bindingSlot = 0;
		vertexStreamDesc.stride = sizeof(ImDrawVertOpt);

		nri::VertexAttributeDesc vertexAttributeDesc[3] = {};
		{
			vertexAttributeDesc[0].format = nri::Format::RG32_SFLOAT;
			vertexAttributeDesc[0].streamIndex = 0;
			vertexAttributeDesc[0].offset = GetOffsetOf(&ImDrawVertOpt::pos);
			vertexAttributeDesc[0].d3d = {"POSITION", 0};
			vertexAttributeDesc[0].vk = {0};

			vertexAttributeDesc[1].format = nri::Format::RG16_UNORM;
			vertexAttributeDesc[1].streamIndex = 0;
			vertexAttributeDesc[1].offset = GetOffsetOf(&ImDrawVertOpt::uv);
			vertexAttributeDesc[1].d3d = {"TEXCOORD", 0};
			vertexAttributeDesc[1].vk = {1};

			vertexAttributeDesc[2].format = nri::Format::RGBA8_UNORM;
			vertexAttributeDesc[2].streamIndex = 0;
			vertexAttributeDesc[2].offset = GetOffsetOf(&ImDrawVertOpt::col);
			vertexAttributeDesc[2].d3d = {"COLOR", 0};
			vertexAttributeDesc[2].vk = {2};
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
		rasterizationDesc.cullMode = nri::CullMode::NONE;

		nri::ColorAttachmentDesc colorAttachmentDesc = {};
		colorAttachmentDesc.format = params.renderTargetFormat;
		colorAttachmentDesc.colorWriteMask = nri::ColorWriteBits::RGBA;
		colorAttachmentDesc.blendEnabled = true;
		colorAttachmentDesc.colorBlend = {nri::BlendFactor::SRC_ALPHA,
		                                  nri::BlendFactor::ONE_MINUS_SRC_ALPHA,
		                                  nri::BlendFunc::ADD};
		colorAttachmentDesc.alphaBlend = {nri::BlendFactor::ONE_MINUS_SRC_ALPHA,
		                                  nri::BlendFactor::ZERO,
		                                  nri::BlendFunc::ADD};

		nri::OutputMergerDesc outputMergerDesc = {};
		outputMergerDesc.colors = &colorAttachmentDesc;
		outputMergerDesc.colorNum = 1;

		nri::GraphicsPipelineDesc graphicsPipelineDesc = {};
		graphicsPipelineDesc.pipelineLayout = m_PipelineLayout;
		graphicsPipelineDesc.vertexInput = &vertexInputDesc;
		graphicsPipelineDesc.inputAssembly = inputAssemblyDesc;
		graphicsPipelineDesc.rasterization = rasterizationDesc;
		graphicsPipelineDesc.outputMerger = outputMergerDesc;
		graphicsPipelineDesc.shaders = shaders;
		graphicsPipelineDesc.shaderNum = GetCountOf(shaders);

		if (params.NRI.CreateGraphicsPipeline(
				params.device, graphicsPipelineDesc, m_Pipeline) !=
		    nri::Result::SUCCESS)
			return false;

		graphicsPipelineDesc.shaders = shaders2;
		graphicsPipelineDesc.shaderNum = GetCountOf(shaders2);

		if (params.NRI.CreateGraphicsPipeline(
				params.device, graphicsPipelineDesc, m_SecondPipeline) !=
		    nri::Result::SUCCESS)
			return false;

	}

	ImFontConfig fontConfig = {};
	fontConfig.SizePixels = floor(13.0f * contentScale);
	io.Fonts->AddFontDefault(&fontConfig);

	int32_t fontWidth = 0, fontHeight = 0;
	uint8_t* fontPixels = nullptr;
	io.Fonts->GetTexDataAsAlpha8(&fontPixels, &fontWidth, &fontHeight);

	constexpr nri::Format format = nri::Format::R8_UNORM;

	nri::TextureDesc textureDesc = {};
	textureDesc.type = nri::TextureType::TEXTURE_2D;
	textureDesc.format = format;
	textureDesc.width = (uint16_t)fontWidth;
	textureDesc.height = (uint16_t)fontHeight;
	textureDesc.mipNum = 1;
	textureDesc.usage = nri::TextureUsageBits::SHADER_RESOURCE;

	if (params.NRI.CreateTexture(params.device, textureDesc, m_FontTexture) !=
	    nri::Result::SUCCESS)
		return false;
	m_Textures.push_back(m_FontTexture);

	
		Texture UIttexture;

		std::string path = GetFullPath("interface.png", DataFolder::TEXTURES);
		if (!LoadTexture(path, UIttexture))
			return false;

		nri::TextureDesc textureDesc2 = {};
		textureDesc2.type = nri::TextureType::TEXTURE_2D;
		textureDesc2.usage = nri::TextureUsageBits::SHADER_RESOURCE;
		textureDesc2.format = UIttexture.GetFormat();
		textureDesc2.width = UIttexture.GetWidth();
		textureDesc2.height = UIttexture.GetHeight();
		textureDesc2.mipNum = UIttexture.GetMipNum();
		textureDesc2.layerNum = UIttexture.GetArraySize();
		nri::Texture* nri_texture;

		if (params.NRI.CreateTexture(params.device, textureDesc2,
	                                 nri_texture) ==
		    nri::Result::SUCCESS)

		{
		}
		else
		{
			std::cout << "cannot make a texture\n";
		}

		
		m_Textures.push_back(nri_texture);

	


	nri::ResourceGroupDesc resourceGroupDesc = {};
	resourceGroupDesc.memoryLocation = nri::MemoryLocation::DEVICE;
	resourceGroupDesc.textureNum = (uint32_t)m_Textures.size();
	resourceGroupDesc.textures = m_Textures.data();

	size_t baseAllocation = m_MemoryAllocations.size();
	const size_t allocationNum =
		params.helperInterface.CalculateAllocationNumber(params.device,
	                                                     resourceGroupDesc);
	m_MemoryAllocations.resize(baseAllocation + allocationNum);

	nri::Result result = params.helperInterface.AllocateAndBindMemory(
		params.device, resourceGroupDesc,
		m_MemoryAllocations.data() + baseAllocation);
	if (result != nri::Result::SUCCESS)
		return false;

	// Descriptor - texture
	nri::Texture2DViewDesc texture2DViewDesc2 = {
		m_FontTexture, nri::Texture2DViewType::SHADER_RESOURCE_2D, format};

	if (params.NRI.CreateTexture2DView(
			texture2DViewDesc2, m_FontShaderResource) != nri::Result::SUCCESS)
		return false;

	nri::Texture2DViewDesc texture2DViewDesc = {
		nri_texture, nri::Texture2DViewType::SHADER_RESOURCE_2D,
		UIttexture.GetFormat()};

	if (params.NRI.CreateTexture2DView(texture2DViewDesc, interfaceUI) !=
	    nri::Result::SUCCESS)
		return false;

	addValueToMap(2000, interfaceUI);

	Texture texture;
	LoadTextureFromMemory(format, fontWidth, fontHeight, fontPixels, texture);

	// Descriptor - sampler
	nri::SamplerDesc samplerDesc = {};
	samplerDesc.anisotropy = 1;
	samplerDesc.addressModes = {nri::AddressMode::REPEAT,
	                            nri::AddressMode::REPEAT};
	samplerDesc.filters.min =
		contentScale > 1.25f ? nri::Filter::NEAREST : nri::Filter::LINEAR;
	samplerDesc.filters.mag =
		contentScale > 1.25f ? nri::Filter::NEAREST : nri::Filter::LINEAR;
	samplerDesc.filters.mip =
		contentScale > 1.25f ? nri::Filter::NEAREST : nri::Filter::LINEAR;

	if (params.NRI.CreateSampler(params.device, samplerDesc, m_Sampler) !=
	    nri::Result::SUCCESS)
		return false;

	nri::CommandQueue* commandQueue = nullptr;

	


	params.NRI.GetCommandQueue(params.device, nri::CommandQueueType::GRAPHICS,
	                           commandQueue);
	{

		std::vector<nri::TextureUploadDesc> textureData(2);

		uint32_t subresourceNum = 0;
		subresourceNum += texture.GetArraySize() * texture.GetMipNum();
		subresourceNum += UIttexture.GetArraySize() * UIttexture.GetMipNum();

		std::vector<nri::TextureSubresourceUploadDesc> subresources(
			subresourceNum);
		nri::TextureSubresourceUploadDesc* subresourceBegin =
			subresources.data();

		for (uint32_t slice = 0; slice < texture.GetArraySize(); slice++)
		{

			for (uint32_t mip = 0; mip < texture.GetMipNum(); mip++)
			{
				texture.GetSubresource(
					subresourceBegin[slice * texture.GetMipNum() + mip], mip,
					slice);
			}

			textureData[0] = {};
			textureData[0].subresources = subresourceBegin;
			textureData[0].texture = m_Textures[0];
			textureData[0].after = {nri::AccessBits::SHADER_RESOURCE,
			                        nri::Layout::SHADER_RESOURCE};

			subresourceBegin += texture.GetArraySize() * texture.GetMipNum();
		}

			for (uint32_t slice = 0; slice < UIttexture.GetArraySize(); slice++)
		{

			for (uint32_t mip = 0; mip < UIttexture.GetMipNum(); mip++)
			{
				UIttexture.GetSubresource(
					subresourceBegin[slice * UIttexture.GetMipNum() + mip], mip,
					slice);
			}

			textureData[1] = {};
			textureData[1].subresources = subresourceBegin;
			textureData[1].texture = m_Textures[1];
			textureData[1].after = {nri::AccessBits::SHADER_RESOURCE,
			                        nri::Layout::SHADER_RESOURCE};

			subresourceBegin += texture.GetArraySize() * texture.GetMipNum();
		}



		if (params.helperInterface.UploadData(*commandQueue, textureData.data(), 
			textureData.size(),
		                                      nullptr,
		                                      0) != nri::Result::SUCCESS)
			return false;
	}

	


	// Descriptor pool
	{
		nri::DescriptorPoolDesc descriptorPoolDesc = {};
		descriptorPoolDesc.descriptorSetMaxNum = 3;
		descriptorPoolDesc.textureMaxNum = 3;
		descriptorPoolDesc.samplerMaxNum = 3;

		if (params.NRI.CreateDescriptorPool(params.device, descriptorPoolDesc,
		                                    m_DescriptorPool) !=
		    nri::Result::SUCCESS)
			return false;
	}

	// Descriptor set
	{
		if (params.NRI.AllocateDescriptorSets(
				*m_DescriptorPool, *m_PipelineLayout, 0, &m_DescriptorSet, 1,
				0) != nri::Result::SUCCESS)
			return false;

		nri::DescriptorRangeUpdateDesc descriptorRangeUpdateDesc[] = {
			{&m_FontShaderResource, 1}, {&m_Sampler, 1}};

		params.NRI.UpdateDescriptorRanges(*m_DescriptorSet, 0,
		                                  GetCountOf(descriptorRangeUpdateDesc),
		                                  descriptorRangeUpdateDesc);

		if (params.NRI.AllocateDescriptorSets(
				*m_DescriptorPool, *m_PipelineLayout, 0, &otherDescriptorSet, 1,
				0) != nri::Result::SUCCESS)
			return false;

				if (params.NRI.AllocateDescriptorSets(
				*m_DescriptorPool, *m_PipelineLayout, 0, &thirdDescriptorSet, 1,
				0) != nri::Result::SUCCESS)
			return false;

			nri::DescriptorRangeUpdateDesc descriptorRangeUpdateDesc2[] = {
					{&interfaceUI, 1}, {&m_Sampler, 1}};

			params.NRI.UpdateDescriptorRanges(
				*thirdDescriptorSet, 0, GetCountOf(descriptorRangeUpdateDesc2),
				descriptorRangeUpdateDesc2);


	}

	



	return true;
}

void UIRenderPass::Draw(const nri::CoreInterface& NRI,
                        const nri::StreamerInterface& streamerInterface,
                        nri::Streamer& streamer,
                        nri::CommandBuffer& commandBuffer, float sdrScale,
                        bool isSrgb)
{
	float consts[4];
	consts[0] = 1.0f / ImGui::GetIO().DisplaySize.x;
	consts[1] = 1.0f / ImGui::GetIO().DisplaySize.y;
	consts[2] = sdrScale;
	consts[3] = isSrgb ? 1.0f : 0.0f;

	Annotation annotation(NRI, commandBuffer, "UI");

	NRI.CmdSetDescriptorPool(commandBuffer, *m_DescriptorPool);
	NRI.CmdSetPipelineLayout(commandBuffer, *m_PipelineLayout);
	NRI.CmdSetPipeline(commandBuffer, *m_Pipeline);
	NRI.CmdSetRootConstants(commandBuffer, 0, consts, sizeof(consts));


	nri::Buffer* geometryBuffer =
		streamerInterface.GetStreamerDynamicBuffer(streamer);
	NRI.CmdSetIndexBuffer(commandBuffer, *geometryBuffer, m_IbOffset,
	                      sizeof(ImDrawIdx) == 2 ? nri::IndexType::UINT16
	                                             : nri::IndexType::UINT32);
	NRI.CmdSetVertexBuffers(commandBuffer, 0, 1, &geometryBuffer, &m_VbOffset);

	const nri::Viewport viewport = {
		0.0f, 0.0f, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y,
		0.0f, 1.0f};
	NRI.CmdSetViewports(commandBuffer, &viewport, 1);

	const ImDrawData& drawData = *ImGui::GetDrawData();
	int32_t vertexOffset = 0;
	uint32_t indexOffset = 0;
	for (int32_t n = 0; n < drawData.CmdListsCount; n++)
	{
		const ImDrawList& drawList = *drawData.CmdLists[n];
		for (int32_t i = 0; i < drawList.CmdBuffer.Size; i++)
		{
			const ImDrawCmd& drawCmd = drawList.CmdBuffer[i];
			if (drawCmd.UserCallback)
				drawCmd.UserCallback(&drawList, &drawCmd);
			else
			{
				nri::Rect rect = {
					(int16_t)drawCmd.ClipRect.x, (int16_t)drawCmd.ClipRect.y,
					(nri::Dim_t)(drawCmd.ClipRect.z - drawCmd.ClipRect.x),
					(nri::Dim_t)(drawCmd.ClipRect.w - drawCmd.ClipRect.y)};

				if (rect.width != 0 && rect.height != 0)
				{
					// We can apply texture here
					nri::DescriptorSet* descriptorSet = nullptr;
					if (drawCmd.GetTexID() != 0)
					{
						//elizoorg 18.01.2025
						//FIXME: This descriptor set should be created from this descriptor pool, otherwise not working
						//descriptorSet = (nri::DescriptorSet*)drawCmd.GetTexID();
						//NRI.CmdSetDescriptorSet(commandBuffer, 0,
						//                        *descriptorSet, nullptr);
						NRI.CmdSetDescriptorSet(
								commandBuffer, 0, *otherDescriptorSet, nullptr);
						uint32_t desc = (uint32_t)drawCmd.GetTexID();
						nri::Descriptor* test = nullptr;
						if (dutyMap.contains(desc))
						{
							test = (nri::Descriptor*)dutyMap[desc];
						}
						if (test == interfaceUI)
						{
							spdlog::info("we're trying to draw texture");
							NRI.CmdSetDescriptorSet(
								commandBuffer, 0, *thirdDescriptorSet, nullptr);
						}


							NRI.CmdSetPipeline(commandBuffer, *m_Pipeline);
				
							

					}
					else
					{
						NRI.CmdSetPipeline(commandBuffer, *m_SecondPipeline);
						NRI.CmdSetDescriptorSet(commandBuffer, 0,
						                        *m_DescriptorSet, nullptr);
					}
					NRI.CmdSetScissors(commandBuffer, &rect, 1);
					NRI.CmdDrawIndexed(
						commandBuffer,
						{drawCmd.ElemCount, 1, indexOffset, vertexOffset, 0});
				}
			}
			indexOffset += drawCmd.ElemCount;
		}
		vertexOffset += drawList.VtxBuffer.Size;
	}
}

void UIRenderPass::BeginUI()
{
	ImGuiIO& io = ImGui::GetIO();

	io.DisplaySize = ImVec2(1280.0f, 720.0f);
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiDockNodeFlags_PassthruCentralNode;

	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void UIRenderPass::EndUI(const nri::StreamerInterface& streamerInterface,
                         nri::Streamer& streamer, const nri::CoreInterface& NRI,
                         nri::HelperInterface& helperInterface,
                         nri::CommandQueue* commandBuffer)
{
	ImGui::EndFrame();
	ImGui::Render();
	const ImDrawData& drawData = *ImGui::GetDrawData();

	// Prepare
	uint32_t vertexDataSize = drawData.TotalVtxCount * sizeof(ImDrawVertOpt);
	vertexDataSize = Align(vertexDataSize, 16);
	uint32_t indexDataSize = drawData.TotalIdxCount * sizeof(ImDrawIdx);
	indexDataSize = Align(indexDataSize, 16);
	uint32_t totalDataSize = vertexDataSize + indexDataSize;
	if (!totalDataSize)
		return;

	if (m_UiData.size() < totalDataSize)
		m_UiData.resize(totalDataSize);

	// Repack geometry
	uint8_t* indexData = m_UiData.data();
	ImDrawVertOpt* vertexData = (ImDrawVertOpt*)(indexData + indexDataSize);

	for (int32_t n = 0; n < drawData.CmdListsCount; n++)
	{
		const ImDrawList& drawList = *drawData.CmdLists[n];

		for (int32_t i = 0; i < drawList.CmdBuffer.Size; i++)
		{
			const ImDrawCmd& drawCmd = drawList.CmdBuffer[i];
			if (drawCmd.GetTexID() != 0)
			{
				helperInterface.WaitForIdle(*commandBuffer);
				uint32_t desc = (uint32_t)drawCmd.GetTexID();
				nri::Descriptor* test;
				if (dutyMap.contains(desc))
				{
					test = (nri::Descriptor*)dutyMap[desc];
				
				}
				else
				{
					test = (nri::Descriptor*)drawCmd.GetTexID();
				}

				nri::DescriptorRangeUpdateDesc descriptorRangeUpdateDesc[] = {
					{&test, 1}, {&m_Sampler, 1}};
				//spdlog::info("desc {} ui desc {}",desc,(uint32_t)interfaceUI);
				if ((uint32_t)test == (uint32_t)interfaceUI)
				{
					//spdlog::info("Updated descriptor set 2000");
		/*			NRI.UpdateDescriptorRanges(
						*thirdDescriptorSet, 0,
						GetCountOf(descriptorRangeUpdateDesc),
						descriptorRangeUpdateDesc);*/
				
				}
				else
				{
					NRI.UpdateDescriptorRanges(
						*otherDescriptorSet, 0,
						GetCountOf(descriptorRangeUpdateDesc),
						descriptorRangeUpdateDesc);
				}
					

				

			}
		}

		for (int32_t i = 0; i < drawList.VtxBuffer.Size; i++)
		{
			const ImDrawVert* v = drawList.VtxBuffer.Data + i;

			ImDrawVertOpt opt;
			opt.pos[0] = v->pos.x;
			opt.pos[1] = v->pos.y;
			opt.uv = float2_to_unorm_16_16(glm::vec2(v->uv.x, v->uv.y));
			opt.col = v->col;

			memcpy(vertexData++, &opt, sizeof(opt));
		}

		size_t size = drawList.IdxBuffer.Size * sizeof(ImDrawIdx);
		memcpy(indexData, drawList.IdxBuffer.Data, size);
		indexData += size;
	}

	// Add update request
	nri::BufferUpdateRequestDesc bufferUpdateRequestDesc = {};
	bufferUpdateRequestDesc.data = m_UiData.data();
	bufferUpdateRequestDesc.dataSize = m_UiData.size();

	m_IbOffset = streamerInterface.AddStreamerBufferUpdateRequest(
		streamer, bufferUpdateRequestDesc);
	m_VbOffset = m_IbOffset + indexDataSize;



}

UIRenderPass::~UIRenderPass()
{
}

void UIRenderPass::addValueToMap(uint32_t key, nri::Descriptor* value)
{
	dutyMap[key] = value;
}

UIRenderPass::UIRenderPass()
{
}