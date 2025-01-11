#pragma once

#include <NRI.h>

#include <Extensions/NRIDeviceCreation.h>
#include <Extensions/NRIHelper.h>
#include <Extensions/NRIStreamer.h>
#include <Extensions/NRILowLatency.h>
#include <Extensions/NRIMeshShader.h>
#include <Extensions/NRIRayTracing.h>
#include <Extensions/NRIResourceAllocator.h>
#include <Extensions/NRISwapChain.h>

#include "MainRenderPass.h"
#include "UIRenderPass.h"

#include "Window.h"
#include "NRIContext.h"

#include "Settings.h"
#include "PhysicsEngine.h"

#include "CmdLine.h"



constexpr bool D3D11_COMMANDBUFFER_EMULATION = false;
constexpr uint32_t DEFAULT_MEMORY_ALIGNMENT = 16;

class Application
{
public:
	Application();
	~Application();

	bool Init(int argc,char **argv);
	void Shutdown();
	void Update();
	void Draw();


	void InitCmdLine([[maybe_unused]] cmdline::parser& cmdLine) {}

	void ReadCmdLine([[maybe_unused]] cmdline::parser& cmdLine) {}

	void InitCmdLineDefault(cmdline::parser& cmdLine);
	void ReadCmdLineDefault(cmdline::parser& cmdLine);

	bool shouldClose = false;
	private:
	Window* window;

	MainRenderPass mainRenderPass;
	UIRenderPass uiRenderPass;
	

	

	Frame& getCurrentFrame(){return window->GetFrames()[frameIndex % BUFFERED_FRAME_MAX_NUM];}

	NRIInterface NRI = {};
	nri::Device* m_Device = nullptr;
	nri::Streamer* m_Streamer = nullptr;
	nri::SwapChain* m_SwapChain = nullptr;
	nri::CommandQueue* m_CommandQueue = nullptr;
	nri::Fence* m_FrameFence = nullptr;

	bool m_DebugAPI = false;
	bool m_DebugNRI = false;

	uint32_t frameIndex = 0;

	nri::SPIRVBindingOffsets SPIRV_BINDING_OFFSETS = {
		100, 200, 300, 400}; // just ShaderMake defaults for simplicity
	nri::AllocationCallbacks m_AllocationCallbacks = {};

#pragma region Physics

public:

	PhysicsEngine* GetPhysicsEngine() { return physicsEngine; }

private:

	PhysicsEngine* physicsEngine;

#pragma endregion Physics

};