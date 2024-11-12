//#pragma once
//
//#include "Exports.h"
//#include "InputDevice.h"
//#include "../external/Delegates.h"
//#include <iostream>
//
//
//namespace Engine
//{
//	class Application;
//}
//
//
//class ENGINE_API WinApi_Display
//{
//public:
//	WinApi_Display();
//	~WinApi_Display();
//	WinApi_Display(Engine::Application* app) : _app(app)
//	{
//
//	}
//
//	bool CreateDisplay();
//	void ShowClientWindow();
//	void OnChangeScreenSize();
//	bool PollMessages();
//	void SetRawInputDevice();
//	int getWidth() { return screenWidth; }
//	int getHeight() { return screenHeight; }
//	HWND getHWND() { return hWnd; }
//
//	int screenWidth = 1920;
//	int screenHeight = 1080;
//
//	static constexpr const char* WINDOW_CLASS_NAME = "WndClass";
//
//	DECLARE_MULTICAST_DELEGATE(OnFocusChangedDelegate, bool);
//	OnFocusChangedDelegate OnFocusChanged;
//
//	DECLARE_MULTICAST_DELEGATE(OnKeyDownDelegate, InputDevice::KeyboardInputEventArgs);
//	OnKeyDownDelegate OnKeyDown;
//private:
//	static LRESULT CALLBACK WndProcStatic(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);
//
//
//	LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
//private:
//	HWND hWnd;
//	LPCWSTR applicationName;
//	WNDCLASSEX wc;
//	HINSTANCE hInstance;
//	RECT windowRect;
//
//	friend class Engine::Application;
//	Engine::Application* _app;
//
//
//
//};
//
