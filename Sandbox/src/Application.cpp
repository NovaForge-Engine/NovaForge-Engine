#include "Application.h"
#include <Extensions/NRIDeviceCreation.h>


Application::Application() 
	: physicsEngine(PhysicsEngine::Get()), scriptEngine(ScriptEngine::Get())
{
}

Application::~Application()
{
}

bool Application::Init(nri::GraphicsAPI graphicsAPI)
{
	window = new Window();

	window->Initialize(1280, 720);


	nri::AdapterDesc bestAdapterDesc = {};
	uint32_t adapterDecscNum = 1;
	if(
		nri::nriEnumerateAdapters(&bestAdapterDesc, adapterDecscNum)!= nri::Result::SUCCESS)
		{
		return false;
		};

	nri::DeviceCreationDesc deviceCreationDesc = {};
	deviceCreationDesc.graphicsAPI = graphicsAPI;
	deviceCreationDesc.enableGraphicsAPIValidation = m_DebugAPI;
	deviceCreationDesc.enableNRIValidation = m_DebugNRI;
	deviceCreationDesc.enableD3D11CommandBufferEmulation =
		D3D11_COMMANDBUFFER_EMULATION;
	deviceCreationDesc.spirvBindingOffsets = SPIRV_BINDING_OFFSETS;
	deviceCreationDesc.adapterDesc = &bestAdapterDesc;
	deviceCreationDesc.allocationCallbacks = m_AllocationCallbacks;

	
	if (nri::nriCreateDevice(deviceCreationDesc, m_Device) != nri::Result::SUCCESS)
	{
		return false;
	}

	if (nri::nriGetInterface(
			*m_Device, "nri::CoreInterface", sizeof(nri::CoreInterface),
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
	streamerDesc.frameInFlightNum = BUFFERED_FRAME_MAX_NUM;

	if ((NRI.CreateStreamer(*m_Device, streamerDesc, m_Streamer)) !=
	    nri::Result::SUCCESS)
		return false;
	;

	// Command queue
	if (NRI.GetCommandQueue(*m_Device, nri::CommandQueueType::GRAPHICS,
	                         m_CommandQueue) != nri::Result::SUCCESS)
		return false;
	;

	// Fences
	if (NRI.CreateFence(*m_Device, 0, m_FrameFence) != nri::Result::SUCCESS)
		return false;
	;

	nri::Format swapChainFormat;
	{
		nri::SwapChainDesc swapChainDesc = {};
		swapChainDesc.window = window->GetNRIWindow();
		swapChainDesc.commandQueue = m_CommandQueue;
		swapChainDesc.format = nri::SwapChainFormat::BT709_G22_8BIT;
		swapChainDesc.verticalSyncInterval = window->m_VsyncInterval;
		swapChainDesc.width = (uint16_t)window->m_RenderOutputWidth;
		swapChainDesc.height = (uint16_t)window->m_RenderOutputHeight;
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
			;

			const BackBuffer backBuffer = {colorAttachment,
			                               swapChainTextures[i]};
			window->m_SwapChainBuffers[i] = backBuffer;
		}
	}

	for (Frame& frame : window->GetFrames())
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
	MainRenderPass::InitParams passParams = {
		.NRI = NRI,
		.helperInterface =NRI,
		.m_Device = *m_Device,
		.renderTargetFormat = swapChainFormat,
		.commandQueue = m_CommandQueue,
	};
	result = mainRenderPass.Init(passParams);
	spdlog::info("MainRenderPass initialized: {}", result);

	UIRenderPass::InitParams uiPassParams = {
		.NRI = NRI,
		.helperInterface = NRI,
		.device = *m_Device,
		.renderTargetFormat = swapChainFormat
	};

	result = uiRenderPass.Init(uiPassParams);
	spdlog::info("UIRenderPass initialized: {}", result);

	physicsEngine->Init();

	scriptEngine->Init();

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

	//(@Tenzy21) Note: Updating physics at the end of the frame. Everything else is updating above
	physicsEngine->Update();
	
	scriptEngine->OnMonoUpdate();
}

void Application::Draw()
{
	const uint32_t bufferedFrameIndex = frameIndex % BUFFERED_FRAME_MAX_NUM;
	const Frame& frame = window->GetFrames()[bufferedFrameIndex];
	nri::CommandBuffer *buffer = frame.commandBuffer;

	if (frameIndex >= BUFFERED_FRAME_MAX_NUM) {
		NRI.Wait(*m_FrameFence, 1 + frameIndex - BUFFERED_FRAME_MAX_NUM);
		NRI.ResetCommandAllocator(*frame.commandAllocator);
	}
	uiRenderPass.BeginUI();

	scriptEngine->OnMonoDrawGui();

	uiRenderPass.EndUI(NRI,*m_Streamer);
	NRI.CopyStreamerUpdateRequests(*m_Streamer);


	mainRenderPass.PrepareFrame();
	uiRenderPass.PrepareFrame();


	mainRenderPass.Draw(NRI,NRI,NRI,NRI,*buffer,frame,window->m_SwapChainBuffers[bufferedFrameIndex],bufferedFrameIndex,window->m_RenderWindowWidth,window->m_RenderWindowHeight);


	nri::AttachmentsDesc attachmentsDesc = {};
	attachmentsDesc.colorNum = 1;
	attachmentsDesc.colors = &window->m_SwapChainBuffers[bufferedFrameIndex].colorAttachment;
	 NRI.CmdBeginRendering(*buffer, attachmentsDesc);
	uiRenderPass.Draw(NRI,NRI,*m_Streamer,*buffer,1.0f,true);
	 NRI.CmdEndRendering(*buffer);

	 NRI.EndCommandBuffer(*buffer);

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
