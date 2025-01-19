#pragma once

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

#include <stdio.h>

#include <include/InputManager.h>
#include <include/GamepadManager.h>

#include "Settings.h"

void GLFW_ErrorCallback(int32_t error, const char* message);

#include <stdint.h>

#include <NRI.h>
#include <Extensions/NRISwapChain.h>

void keyCallback(GLFWwindow* window, int key, int scancode, int action,
				 int mods);


struct BackBuffer
{
	nri::Descriptor* colorAttachment;
	nri::Texture* texture;
};

struct Frame
{
	nri::CommandAllocator* commandAllocator;
	nri::CommandBuffer* commandBuffer;

	uint64_t constantBufferViewOffset;
};

class Window
{
	public:
	Window();
	~Window();

	bool Initialize(int width, int height);
	void Update();
	void Render();
	void Shutdown();


	uint32_t m_RenderWindowWidth;
	uint32_t m_RenderOutputWidth = 1920;
	uint32_t m_RenderWindowHeight;
	uint32_t m_RenderOutputHeight  = 1080;
	
	std::array<Frame, BUFFERED_FRAME_MAX_NUM>& GetFrames() { return m_Frames; }
	std::array<BackBuffer,SWAP_CHAIN_TEXTURE_NUM>& GetSwapChainBuffers() { return m_SwapChainBuffers; }

	nri::Window& GetNRIWindow() {return m_NRIWindow;}
	GLFWwindow* GetGLFWWindow() { return m_Window; }


	std::array<Frame, BUFFERED_FRAME_MAX_NUM> m_Frames = {};
	std::array<BackBuffer, SWAP_CHAIN_TEXTURE_NUM> m_SwapChainBuffers;
	
		uint8_t m_VsyncInterval = 0;
	uint32_t m_DpiMode = 0;
	
	private:

	nri::Window m_NRIWindow = {};
	GLFWwindow* m_Window = nullptr;




	
};