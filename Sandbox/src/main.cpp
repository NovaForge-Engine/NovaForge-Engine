
#include <iostream>


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
#include <Extensions/NRIStreamer.h>
#include <Extensions/NRILowLatency.h>
#include <Extensions/NRIMeshShader.h>
#include <Extensions/NRIRayTracing.h>
#include <Extensions/NRIResourceAllocator.h>
#include <Extensions/NRISwapChain.h>

#include <vector>
#include <array>

#include "Utils.hpp"

#include "include/InputManager.h"
#include "include/GamepadManager.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include <utility>

#include <spdlog/spdlog.h>

Assimp::Importer importer;





static void GLFW_ErrorCallback(int32_t error, const char* message) {
	printf("GLFW error[%d]: %s\n", error, message);
#if _WIN32
	DebugBreak();
#else
	raise(SIGTRAP);
#endif
}

void helloCallback(NovaEngine::InputEvent event) {
	printf("Pressed the button!!!\n");
	std::cout << "SOsat' america" << std::endl;
}

using namespace std;

nri::CoreInterface NRI;

bool m_DebugAPI = false;
bool m_DebugNRI = false;
constexpr bool D3D11_COMMANDBUFFER_EMULATION = false;
constexpr uint32_t DEFAULT_MEMORY_ALIGNMENT = 16;

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

	uint64_t constantBufferViewOffset;
};

struct NRIInterface : public nri::CoreInterface,
	public nri::HelperInterface,
	public nri::StreamerInterface,
	public nri::SwapChainInterface
{
};



void keyCallback(GLFWwindow* window, int key, int scancode, int action,
                 int mods)
{
	std::cout << "132312" << std::endl;
	NovaEngine::InputManager::instance().keyboardCallback(window, key, scancode,
	                                                      action, mods);
	std::cout << "2321321" << std::endl;
}


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

	

	std::array<Frame, BUFFERED_FRAME_MAX_NUM> m_Frames = {};
	std::vector<BackBuffer> m_SwapChainBuffers;
	


	uint32_t m_RenderOutputWidth = 1920;
	uint32_t m_RenderOutputHeight = 1080;
	uint32_t m_RenderWindowWidth;
	uint32_t m_RenderWindowHeight;

	
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

	if (frameIndex >= BUFFERED_FRAME_MAX_NUM)
	{
		NRI.Wait(*m_FrameFence, 1 + frameIndex - BUFFERED_FRAME_MAX_NUM);
		NRI.ResetCommandAllocator(*frame.commandAllocator);
	}

	const uint32_t currentTextureIndex =
		NRI.AcquireNextSwapChainTexture(*m_SwapChain);
	BackBuffer& currentBackBuffer = m_SwapChainBuffers[currentTextureIndex];

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

		constexpr nri::Color32f COLOR_0 = {1.0f, 1.0f, 0.0f, 1.0f};

		NRI.CmdBeginRendering(*commandBuffer, attachmentsDesc);
		{
			{
				Annotation annotation(NRI, *commandBuffer, "Clears");

				nri::ClearDesc clearDesc = {};
				clearDesc.planes = nri::PlaneBits::COLOR;
				clearDesc.value.color.f = COLOR_0;

				NRI.CmdClearAttachments(*commandBuffer, &clearDesc, 1, nullptr,
				                        0);
			}

			{
				Annotation annotation(NRI, *commandBuffer, "Triangle");

				const nri::Viewport viewport = {
					0.0f, 0.0f, (float)windowWidth, (float)windowHeight,
					0.0f, 1.0f};
				NRI.CmdSetViewports(*commandBuffer, &viewport, 1);

				NRI.CmdSetPipelineLayout(*commandBuffer, *m_PipelineLayout);
				NRI.CmdSetPipeline(*commandBuffer, *m_Pipeline);
				NRI.CmdSetRootConstants(*commandBuffer, 0, &m_Transparency, 4);
				NRI.CmdSetIndexBuffer(*commandBuffer, *m_GeometryBuffer, 0,
				                      nri::IndexType::UINT16);
				NRI.CmdSetVertexBuffers(*commandBuffer, 0, 1, &m_GeometryBuffer,
				                        &m_GeometryOffset);
				NRI.CmdSetDescriptorSet(*commandBuffer, 0,
				                        *frame.constantBufferDescriptorSet,
				                        nullptr);
				NRI.CmdSetDescriptorSet(*commandBuffer, 1,
				                        *m_TextureDescriptorSet, nullptr);

				nri::Rect scissor = {0, 0, windowWidth, windowHeight};
				NRI.CmdSetScissors(*commandBuffer, &scissor, 1);
				NRI.CmdDrawIndexed(*commandBuffer, {3, 1, 0, 0, 0});

				scissor = {(int16_t)halfWidth, (int16_t)halfHeight, halfWidth,
				           halfHeight};
				NRI.CmdSetScissors(*commandBuffer, &scissor, 1);
				NRI.CmdDraw(*commandBuffer, {3, 1, 0, 0});
			}

			{
			}
		}
		NRI.CmdEndRendering(*commandBuffer);

		textureBarrierDescs.before = textureBarrierDescs.after;
		textureBarrierDescs.after = {nri::AccessBits::UNKNOWN,
		                             nri::Layout::PRESENT};

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


	return true;
}

bool Sample::Create(int _argc, char** _argv)
{

	glfwSetErrorCallback(GLFW_ErrorCallback);

#if __linux__
	glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
#endif

	glfwInit();
		


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

	glfwSetKeyCallback(m_Window, keyCallback);
	glfwSetMouseButtonCallback(m_Window, &NovaEngine::InputManager::mouseButtonCallback);
	glfwSetScrollCallback(m_Window, &NovaEngine::InputManager::mouseScrollCallback);
	glfwSetCursorPosCallback(m_Window, &NovaEngine::InputManager::mousePositionCallback);


	glfwSetJoystickCallback(
		&NovaEngine::GLFWJoystickConnectionCallback);

	NovaEngine::GamepadManager::instance().searchForActiveGamepads();


	NovaEngine::InputBinding bind("helloBind", NovaEngine::EventAxes::BUTTON, NovaEngine::EventType::STARTED);
	NovaEngine::InputManager::instance().addContext("Test");
	NovaEngine::InputManager::instance().switchContext("Test");

	NovaEngine::InputContext context = NovaEngine::InputManager::instance().getContext("Test");
	
	bind.addSubscriber(NovaEngine::InputDelegate{entt::connect_arg<&helloCallback>});

	context.addBinding(bind, NovaEngine::KeyboardSource::KEY_B);

	NovaEngine::InputContext& context2 =
		NovaEngine::InputManager::instance().getContext("Test");



	NovaEngine::InputManager::instance().getContext("Default").addBinding(bind, NovaEngine::KeyboardSource::KEY_A);

	context2.addBinding(bind, NovaEngine::KeyboardSource::KEY_C);

	return false;
}




int main(int argc, char** argv) {
	nri::GraphicsAPI api = nri::GraphicsAPI::D3D12;

	Sample sample;

	sample.Create(argc, argv);
	bool startup = sample.Initialize(api);
	
	NovaEngine::InputManager::instance().addContext("Test");
	NovaEngine::InputManager::instance().switchContext("Test");

	NovaEngine::InputContext& context2 =
		NovaEngine::InputManager::instance().getContext("Test");
	NovaEngine::InputBinding bind("helloBind", NovaEngine::EventAxes::BUTTON,
	                              NovaEngine::EventType::STARTED);
	bind.addSubscriber(
		NovaEngine::InputDelegate{entt::connect_arg<&helloCallback>});
	context2.addBinding(bind, NovaEngine::KeyboardSource::KEY_C);






		NovaEngine::InputBinding bind2("testbind", NovaEngine::EventAxes::SINGLE,
	                               NovaEngine::EventType::CHANGED);

			bind2.addSubscriber(
			NovaEngine::InputDelegate{entt::connect_arg<&helloCallback>});

	context2.addBinding(bind2, NovaEngine::GamepadSource::AXIS_LEFT_TRIGGER);


	std::cout << &NovaEngine::GamepadManager::instance << std::endl;



	while (startup) {
		sample.RenderFrame();


		//NovaEngine::GamepadManager::instance().searchForActiveGamepads();
		NovaEngine::GamepadManager::instance().pollGamepadEvents();
		
		glfwPollEvents();
	}



	return 0;
}

