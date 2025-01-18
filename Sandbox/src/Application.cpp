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

	window = new nova::Window();

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
	bool result;

	result = loader.LoadModel(
		scene, GetFullPath("Sponza/models/sponza.obj", DataFolder::SCENES));
	result = loader.LoadModel(
		scene, GetFullPath("cat2/12221_Cat_v1_l3.obj", DataFolder::SCENES));

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
	spdlog::info("MainRenderPass initialized: {}", result);

	UIRenderPass::InitParams uiPassParams = {.NRI = NRI,
	                                         .helperInterface = NRI,
	                                         .device = *m_Device,
	                                         .renderTargetFormat =
	                                             swapChainFormat,
	                                         .window = window};

	result = uiRenderPass.Init(uiPassParams);
	spdlog::info("UIRenderPass initialized: {}", result);

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
		ImGui::Begin("Engine Interface");
		mainRenderPass.BeginUI();
		ImGui::StyleColorsDark();
		// ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Scene"))
				{
				}
				if (ImGui::MenuItem("Open Scene"))
				{
				}
				if (ImGui::MenuItem("Save Scene"))
				{
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Undo"))
				{
				}
				if (ImGui::MenuItem("Redo"))
				{
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
		ImGui::DockSpace(ImGui::GetID("DockSpace"));
		ImGui::End();

		ImGui::CollapsingHeader("Scene Management");
		ImGui::Text("Current Scene: Scene1");
		if (ImGui::Button("Load Scene"))
		{
		}

		if (ImGui::Button("Save Scene"))
		{
		}

		ImGui::Separator();

		static const char* objects[] = {"Sphere", "Cube", "Light"};
		static int object_current_idx = 0;
		ImGui::ListBox("Objects", &object_current_idx, objects,
		               IM_ARRAYSIZE(objects), 4);

		ImGui::CollapsingHeader("Object Inspector");
		{
			ImGui::Text("Selected Object: %s", objects[object_current_idx]);

			static float position[3] = {0.0f, 0.0f, 0.0f};
			ImGui::InputFloat3("Position", position);

			static float rotation[3] = {0.0f, 0.0f, 0.0f};
			ImGui::InputFloat3("Rotation", rotation);

			static float scale[3] = {1.0f, 1.0f, 1.0f};
			ImGui::InputFloat3("Scale", scale);

			if (ImGui::Button("Delete Object"))
			{
			}
		}

		ImGui::CollapsingHeader("Material Editor");
		{
			static char materialName[50] = "New Material";
			ImGui::InputText("Material Name", materialName,
			                 IM_ARRAYSIZE(materialName));

			static float color[3] = {1.0f, 0.5f, 0.5f};
			ImGui::ColorEdit3("Color", color);

			if (ImGui::Button("Create Material"))
			{
			}
		}

		ImGui::CollapsingHeader("Camera Settings");
		{
			static float cameraPosition[3] = {0.0f, 5.0f, -10.0f};
			ImGui::InputFloat3("Position", cameraPosition);

			static float cameraRotation[3] = {0.0f, 0.0f, 0.0f};
			ImGui::InputFloat3("Rotation", cameraRotation);

			static float cameraFov = 45.0f;
			ImGui::SliderFloat("Field of View", &cameraFov, 1.0f, 90.0f);
		}
	}
	ApplyTheme(ImGuiTheme_SoDark_AccentRed);

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