#include "Window.h"
using namespace nova;
void GLFW_ErrorCallback(int32_t error, const char* message)
{
	printf("GLFW error[%d]: %s\n", error, message);
#if _WIN32
	DebugBreak();
#else
	raise(SIGTRAP);
#endif
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action,
                 int mods)
{
	NovaEngine::InputManager::instance().keyboardCallback(window, key, scancode,
	                                                      action, mods);
}

NovaWindow::NovaWindow()
{
}

NovaWindow::~NovaWindow()
{
}

bool NovaWindow::Initialize(int width, int height, std::string api)
{
	glfwSetErrorCallback(GLFW_ErrorCallback);

#if __linux__
	glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
#endif

	glfwInit();

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();

	float contentScale = 1.0f;
	if (m_DpiMode != 0)
	{
		float unused;
		glfwGetMonitorContentScale(monitor, &contentScale, &unused);
		printf("DPI scale %.1f%% (%s)\n", contentScale * 100.0f,
		       m_DpiMode == 2 ? "quality" : "performance");
	}
	m_RenderWindowWidth = (uint32_t)floor(m_RenderOutputWidth * contentScale);
	m_RenderWindowHeight = (uint32_t)floor(m_RenderOutputHeight * contentScale);

	const GLFWvidmode* vidmode = glfwGetVideoMode(monitor);
	const uint32_t screenW = (uint32_t)vidmode->width;
	const uint32_t screenH = (uint32_t)vidmode->height;

	m_RenderWindowWidth = std::min(m_RenderWindowWidth, screenW);
	m_RenderWindowHeight = std::min(m_RenderWindowHeight, screenH);

	// Rendering output size
	m_RenderOutputWidth = std::min(m_RenderOutputWidth, m_RenderWindowWidth);
	m_RenderOutputHeight = std::min(m_RenderOutputHeight, m_RenderWindowHeight);

	if (m_DpiMode == 2)
	{
		m_RenderOutputWidth = m_RenderWindowWidth;
		m_RenderOutputHeight = m_RenderWindowHeight;
	}

	bool decorated =
		m_RenderWindowWidth != screenW && m_RenderWindowHeight != screenH;

	printf("Creating %swindow (%u, %u)\n", decorated ? "" : "borderless ",
	       m_RenderWindowWidth, m_RenderWindowHeight);

	int32_t x = (screenW - m_RenderWindowWidth) >> 1;
	int32_t y = (screenH - m_RenderWindowHeight) >> 1;

	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_DECORATED, decorated ? 1 : 0);
	glfwWindowHint(GLFW_RESIZABLE, 0);
	glfwWindowHint(GLFW_POSITION_X, x);
	glfwWindowHint(GLFW_POSITION_Y, y);

	char windowName[256];
	snprintf(windowName, sizeof(windowName), "%s [%s]", "NovaForge editor",
	         api.c_str());

	m_Window = glfwCreateWindow(m_RenderWindowWidth, m_RenderWindowHeight,
	                            windowName, NULL, NULL);
	if (!m_Window)
	{
		glfwTerminate();
		return false;
	}

#if _WIN32
	m_NRIWindow.windows.hwnd = glfwGetWin32Window(m_Window);
#elif __linux__
	m_NRIWindow.x11.dpy = glfwGetX11Display();
	m_NRIWindow.x11.window = glfwGetX11Window(m_Window);
#endif

	// elizoorg 01.11.2024
	// TODO: Add glfw window callbacks

	glfwSetKeyCallback(m_Window, keyCallback);
	glfwSetMouseButtonCallback(m_Window,
	                           &NovaEngine::InputManager::mouseButtonCallback);
	glfwSetScrollCallback(m_Window,
	                      &NovaEngine::InputManager::mouseScrollCallback);
	glfwSetCursorPosCallback(m_Window,
	                         &NovaEngine::InputManager::mousePositionCallback);

	return true;
}

void NovaWindow::Update()
{
}

void NovaWindow::Render()
{
}

void NovaWindow::Shutdown()
{
}
