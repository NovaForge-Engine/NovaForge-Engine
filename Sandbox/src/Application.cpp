#include "Application.h"

#include "ImguiThemes.h"

#include <Extensions/NRIDeviceCreation.h>

Application::Application() : physicsEngine(PhysicsEngine::Get())
{
}

Application::~Application()
{
}

bool Application::Init(int argc, char** argv)
{

	cmdline::parser cmdLine;

	InitCmdLineDefault(cmdLine);
	InitCmdLine(cmdLine);

	bool parseStatus = cmdLine.parse(argc, argv);

	if (cmdLine.exist("help"))
	{
		std::cout << cmdLine.usage() << std::endl;
		return false;
	}

	if (!parseStatus)
	{
		spdlog::error(
			"Failed to parse command line arguments. Error: {} , Usage {}",
			cmdLine.error(), cmdLine.usage());
		return false;
	}

	ReadCmdLineDefault(cmdLine);
	ReadCmdLine(cmdLine);

	window = new nova::NovaWindow();

	window->Initialize(1280, 720, cmdLine.get<std::string>("api"));

	nri::AdapterDesc bestAdapterDesc = {};
	uint32_t adapterDecscNum = 1;
	if (nriEnumerateAdapters(&bestAdapterDesc, adapterDecscNum) !=
	    nri::Result::SUCCESS)
	{
		return false;
	}

	auto graphicsAPI = nri::GraphicsAPI::D3D12;
	if (cmdLine.get<std::string>("api") == "VULKAN" ||
	    cmdLine.get<std::string>("api") == "VK")
		graphicsAPI = nri::GraphicsAPI::VK;

	nri::DeviceCreationDesc deviceCreationDesc = {};
	deviceCreationDesc.graphicsAPI = graphicsAPI;
	deviceCreationDesc.enableGraphicsAPIValidation = m_DebugAPI;
	deviceCreationDesc.enableNRIValidation = m_DebugNRI;
	deviceCreationDesc.enableD3D11CommandBufferEmulation =
		D3D11_COMMANDBUFFER_EMULATION;
	deviceCreationDesc.spirvBindingOffsets = SPIRV_BINDING_OFFSETS;
	deviceCreationDesc.adapterDesc = &bestAdapterDesc;
	deviceCreationDesc.allocationCallbacks = m_AllocationCallbacks;

	if (nriCreateDevice(deviceCreationDesc, m_Device) != nri::Result::SUCCESS)
	{
		return false;
	}

	if (nri::nriGetInterface(*m_Device, "nri::CoreInterface",
	                         sizeof(nri::CoreInterface),
	                         (nri::CoreInterface*)&NRI) != nri::Result::SUCCESS)
	{
		return false;
	}
	if (nri::nriGetInterface(
			*m_Device, "nri::HelperInterface", sizeof(nri::HelperInterface),
			(nri::HelperInterface*)&NRI) != nri::Result::SUCCESS)
		return false;

	if (nri::nriGetInterface(
			*m_Device, "nri::StreamerInterface", sizeof(nri::StreamerInterface),
			(nri::StreamerInterface*)&NRI) != nri::Result::SUCCESS)
		return false;

	if (nri::nriGetInterface(*m_Device, "nri::SwapChainInterface",
	                         sizeof(nri::SwapChainInterface),
	                         (nri::SwapChainInterface*)&NRI) !=
	    nri::Result::SUCCESS)
		return false;

	nri::StreamerDesc streamerDesc = {};
	streamerDesc.dynamicBufferMemoryLocation = nri::MemoryLocation::HOST_UPLOAD;
	streamerDesc.dynamicBufferUsageBits = nri::BufferUsageBits::VERTEX_BUFFER |
	                                      nri::BufferUsageBits::INDEX_BUFFER;
	streamerDesc.constantBufferMemoryLocation =
		nri::MemoryLocation::HOST_UPLOAD;
	streamerDesc.constantBufferSize = DYNAMIC_CONSTANT_BUFFER_SIZE;
	streamerDesc.frameInFlightNum = BUFFERED_FRAME_MAX_NUM;

	if ((NRI.CreateStreamer(*m_Device, streamerDesc, m_Streamer)) !=
	    nri::Result::SUCCESS)
		return false;

	// Command queue
	if (NRI.GetCommandQueue(*m_Device, nri::CommandQueueType::GRAPHICS,
	                        m_CommandQueue) != nri::Result::SUCCESS)
		return false;

	// Fences
	if (NRI.CreateFence(*m_Device, 0, m_FrameFence) != nri::Result::SUCCESS)
		return false;

	nri::Format swapChainFormat;
	{
		nri::SwapChainDesc swapChainDesc = {};
		swapChainDesc.window = window->GetNRIWindow();
		swapChainDesc.commandQueue = m_CommandQueue;
		swapChainDesc.format = nri::SwapChainFormat::BT709_G22_8BIT;
		swapChainDesc.verticalSyncInterval = window->m_VsyncInterval;
		swapChainDesc.width =
			static_cast<uint16_t>(window->m_RenderOutputWidth);
		swapChainDesc.height =
			static_cast<uint16_t>(window->m_RenderOutputHeight);
		swapChainDesc.textureNum = SWAP_CHAIN_TEXTURE_NUM;
		if ((NRI.CreateSwapChain(*m_Device, swapChainDesc, m_SwapChain)) !=
		    nri::Result::SUCCESS)
			return false;

		uint32_t swapChainTextureNum;
		nri::Texture* const* swapChainTextures =
			NRI.GetSwapChainTextures(*m_SwapChain, swapChainTextureNum);
		swapChainFormat = NRI.GetTextureDesc(*swapChainTextures[0]).format;

		for (uint32_t i = 0; i < swapChainTextureNum; i++)
		{
			nri::Texture2DViewDesc textureViewDesc = {
				swapChainTextures[i], nri::Texture2DViewType::COLOR_ATTACHMENT,
				swapChainFormat};

			nri::Descriptor* colorAttachment;
			if ((NRI.CreateTexture2DView(textureViewDesc, colorAttachment)) !=
			    nri::Result::SUCCESS)
				return false;

			const nova::BackBuffer backBuffer = {colorAttachment,
			                                     swapChainTextures[i]};
			window->m_SwapChainBuffers[i] = backBuffer;
		}
	}

	for (nova::Frame& frame : window->GetFrames())
	{
		if (NRI.CreateCommandAllocator(*m_CommandQueue,
		                               frame.commandAllocator) !=
		    nri::Result::SUCCESS)
			return false;

		if (NRI.CreateCommandBuffer(*frame.commandAllocator,
		                            frame.commandBuffer) !=
		    nri::Result::SUCCESS)
			return false;
	}
	bool result = true;

	result = loader.LoadModel(scene, GetFullPath("Sponza/models/sponza.obj", DataFolder::SCENES));
	result = loader.LoadModel(scene, GetFullPath("cat2/12221_Cat_v1_l3.obj", DataFolder::SCENES));

	if (!result)
	{
		spdlog::error("Failed to load Sponza model");
		return false;
	}

	MainRenderPass::InitParams passParams = {.NRI = NRI,
	                                         .helperInterface = NRI,
	                                         .m_Device = *m_Device,
	                                         .renderTargetFormat =
	                                             swapChainFormat,
	                                         .commandQueue = m_CommandQueue,
	                                         .scene = &scene};
	result = mainRenderPass.Init(passParams);

	appState.outputTexture = mainRenderPass.m_TextureShaderResource;

	spdlog::info("MainRenderPass initialized: {}", result);

	UIRenderPass::InitParams uiPassParams = {.NRI = NRI,
	                                         .helperInterface = NRI,
	                                         .device = *m_Device,
	                                         .renderTargetFormat =
	                                             swapChainFormat,
	                                         .window = window};

	result = uiRenderPass.Init(uiPassParams);
	spdlog::info("UIRenderPass initialized: {}", result);

	dockingParams.layoutReset = true;

	physicsEngine->Init();

	// loader.LoadModel(GetFullPath("PZ18.ma", DataFolder::SCENES));
	// loader.LoadModel(GetFullPath("PZ19.ma", DataFolder::SCENES));
	// loader.LoadModel(GetFullPath("fhead.stl", DataFolder::SCENES));

	return true;
}

void Application::Shutdown()
{
	physicsEngine->Terminate();
	mainRenderPass.Shutdown();
	uiRenderPass.Shutdown();
}

void Application::Update()
{
	glfwPollEvents();

	if (glfwWindowShouldClose(window->GetGLFWWindow()))
	{
		shouldClose = false;
	}

	//(@Tenzy21) Note: Updating physics at the end of the frame. Everything else
	//is updating above
	physicsEngine->Update();
}

void Application::Draw()
{
	const uint32_t bufferedFrameIndex = frameIndex % BUFFERED_FRAME_MAX_NUM;
	spdlog::info("Drawing frame {}", bufferedFrameIndex);
	const nova::Frame& frame = window->GetFrames()[bufferedFrameIndex];
	nri::CommandBuffer* commandBuffer = frame.commandBuffer;

	if (frameIndex >= BUFFERED_FRAME_MAX_NUM)
	{
		NRI.Wait(*m_FrameFence, 1 + frameIndex - BUFFERED_FRAME_MAX_NUM);
		NRI.ResetCommandAllocator(*frame.commandAllocator);
	}

	uiRenderPass.BeginUI();
	{
		//(@Tenzy21)TODO: Add ImGuiWindowFlags_MenuBar flag into main window
		static bool pOpen = true;
		ImGuiID mainDockspaceId = ImGui::GetID("MainDockSpace");
		ImGuiViewport* viewport = ImGui::GetMainViewport();

		ImGui::DockSpaceOverViewport(mainDockspaceId, viewport);
		SplitIdsHelper::SetSplitId("MainDockSpace", mainDockspaceId);

		if (dockingParams.layoutReset) {
			dockingParams.layoutReset = false;
			ImGui::SetWindowPos(ImVec2(0, 0));
			ImGui::SetWindowSize(ImGui::GetIO().DisplaySize);
			
			auto layout = CreateDefaultLayout(appState);
			dockingParams = layout[0];

			ImGui::DockBuilderRemoveNodeChildNodes(mainDockspaceId);
			for (const auto& dockingSplit : dockingParams.dockingSplits) {
				DockingDetails::DoSplit(dockingSplit);
			}
			for (const auto& dockableWindow : dockingParams.dockableWindows) {
				ImGui::DockBuilderDockWindow(dockableWindow.label.c_str(), SplitIdsHelper::GetSplitId(dockableWindow.dockSpaceName));
			}
			ImGui::DockBuilderFinish(mainDockspaceId);
			ApplyTheme(ImGuiTheme::ImGuiTheme_SoDark_AccentRed);
		}

		for (auto& dockableWindow : dockingParams.dockableWindows) {
			if (dockableWindow.windowSize.x > 0.f)
				ImGui::SetNextWindowSize(dockableWindow.windowSize, dockableWindow.windowSizeCondition);
			if (dockableWindow.windowPosition.x > 0.f)
				ImGui::SetNextWindowPos(dockableWindow.windowPosition, dockableWindow.windowPositionCondition);

			bool not_collapsed = true;
			if (dockableWindow.canBeClosed) {
				not_collapsed = ImGui::Begin(dockableWindow.label.c_str(), &dockableWindow.isVisible, dockableWindow.imGuiWindowFlags);
			}
			else {
				not_collapsed = ImGui::Begin(dockableWindow.label.c_str(), nullptr, dockableWindow.imGuiWindowFlags);
			}
			if (not_collapsed && dockableWindow.GuiFunction) {
				dockableWindow.GuiFunction();
			}
			ImGui::End();
		}
	}

	uiRenderPass.EndUI(NRI, *m_Streamer, NRI, NRI, m_CommandQueue);
	NRI.CopyStreamerUpdateRequests(*m_Streamer);

	mainRenderPass.PrepareFrame();
	uiRenderPass.PrepareFrame();

	const uint32_t currentTextureIndex =
		NRI.AcquireNextSwapChainTexture(*m_SwapChain);
	spdlog::info("Current texture index {}", currentTextureIndex);

	nova::BackBuffer& backBuffer =
		window->m_SwapChainBuffers[currentTextureIndex];
	{ // MainPass
		mainRenderPass.Draw(NRI, NRI, NRI, NRI, *commandBuffer, frame,
		                    backBuffer, bufferedFrameIndex,
		                    window->m_RenderWindowWidth,
		                    window->m_RenderWindowHeight);
	}

	{ // UI
		nri::AttachmentsDesc attachmentsDesc = {};
		attachmentsDesc.colorNum = 1;
		attachmentsDesc.colors = &backBuffer.colorAttachment;

		NRI.CmdBeginRendering(*commandBuffer, attachmentsDesc);
		{
			uiRenderPass.Draw(NRI, NRI, *m_Streamer, *commandBuffer, 1.0f,
			                  true);
		}
		NRI.CmdEndRendering(*commandBuffer);

		nri::TextureBarrierDesc textureBarrierDescs = {};
		textureBarrierDescs.texture = backBuffer.texture;
		textureBarrierDescs.after = {nri::AccessBits::COLOR_ATTACHMENT,
		                             nri::Layout::COLOR_ATTACHMENT};
		textureBarrierDescs.layerNum = 1;
		textureBarrierDescs.mipNum = 1;

		nri::BarrierGroupDesc barrierGroupDesc = {};
		barrierGroupDesc.textureNum = 1;
		barrierGroupDesc.textures = &textureBarrierDescs;

		textureBarrierDescs.before = textureBarrierDescs.after;
		textureBarrierDescs.after = {nri::AccessBits::UNKNOWN,
		                             nri::Layout::PRESENT};

		NRI.CmdBarrier(*commandBuffer, barrierGroupDesc);
	}
	NRI.EndCommandBuffer(*commandBuffer);

	{
		// Submit
		nri::QueueSubmitDesc queueSubmitDesc = {};
		queueSubmitDesc.commandBuffers = &frame.commandBuffer;
		queueSubmitDesc.commandBufferNum = 1;

		NRI.QueueSubmit(*m_CommandQueue, queueSubmitDesc);
	}

	// Present
	NRI.QueuePresent(*m_SwapChain);

	{
		// Signaling after "Present" improves D3D11 performance a bit
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

void Application::InitCmdLineDefault(cmdline::parser& cmdLine)
{
#if _WIN32
	std::string graphicsAPI = "D3D12";
#else
	std::string graphicsAPI = "VULKAN";
#endif
	cmdLine.add("help", '?', "print this message");
	cmdLine.add<std::string>("api", 'a', "graphics API: D3D12 or VULKAN", false,
	                         graphicsAPI,
	                         cmdline::oneof<std::string>("D3D12", "VULKAN"));
	cmdLine.add("debugAPI", 0, "enable graphics API validation layer");
	cmdLine.add("debugNRI", 0, "enable NRI validation layer");
}

void Application::ReadCmdLineDefault(cmdline::parser& cmdLine)
{
	m_DebugAPI = cmdLine.exist("debugAPI");
	m_DebugNRI = cmdLine.exist("debugNRI");
}
