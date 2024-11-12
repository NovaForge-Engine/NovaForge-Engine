//
//// This is a personal academic project. Dear PVS-Studio, please check it.
//// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
//
//#include "Exports.h"
//#include "Display.h"
//
//#include "Application.h"
//#include "../external/ImGui/imgui.h"
//
//
//
//
//
//
//WinApi_Display::WinApi_Display()
//{
//}
//
//WinApi_Display::~WinApi_Display()
//{
//}
//
//
//void WinApi_Display::OnChangeScreenSize() {
//	//screenWidth = args.Width;
//	//creenHeight = args.Height;
//}
//
//bool WinApi_Display::PollMessages()
//{
//
//	MSG msg = {};
//	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
//		TranslateMessage(&msg);
//		DispatchMessage(&msg);
//	}
//
//	// If windows signals to end the application then exit out.
//	if (msg.message == WM_QUIT) {
//		return false;
//	}
//
//
//	//POINT p;
//	//GetCursorPos(&p);
//	//ScreenToClient(hWnd, &p);
//	//OnMouseMove.Broadcast(p.x, p.y);
//	//std::cout << "EndPollMessages\n";
//	return true;
//}
//
//void WinApi_Display::SetRawInputDevice()
//{
//	RAWINPUTDEVICE Rid[2];
//
//	Rid[0].usUsagePage = 0x01;
//	Rid[0].usUsage = 0x02;
//	Rid[0].dwFlags = 0;   // adds HID mouse and also ignores legacy mouse messages
//	Rid[0].hwndTarget = hWnd;
//
//	Rid[1].usUsagePage = 0x01;
//	Rid[1].usUsage = 0x06;
//	Rid[1].dwFlags = 0;   // adds HID keyboard and also ignores legacy keyboard messages
//	Rid[1].hwndTarget = hWnd;
//
//	if (RegisterRawInputDevices(Rid, 2, sizeof(Rid[0])) == FALSE)
//	{
//		auto errorCode = GetLastError();
//		std::cout << "ERROR: " << errorCode << std::endl;
//	}
//}
//
//LRESULT WinApi_Display::WndProcStatic(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//
//
//	WinApi_Display* pThis;
//	if (message == WM_NCCREATE) {
//
//		LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
//		pThis = static_cast<WinApi_Display*>(lpcs->lpCreateParams);
//
//		SetWindowLongPtr(hWnd, GWLP_USERDATA,
//			reinterpret_cast<LONG_PTR>(pThis));
//	}
//	else {
//		pThis = reinterpret_cast<WinApi_Display*>(
//			GetWindowLongPtr(hWnd, GWLP_USERDATA));
//	}
//	if (pThis) {
//		return pThis->WndProc(hWnd, message, wParam, lParam);
//	}
//
//	return DefWindowProc(hWnd, message, wParam, lParam);
//
//}
//
//extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
//
//
//LRESULT WinApi_Display::WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
//{
//	if (ImGui_ImplWin32_WndProcHandler(hWnd, umessage, wparam, lparam))
//		return true;
//
//
//	switch (umessage)
//	{
//	case WM_DESTROY:
//		PostQuitMessage(0);
//		break;
//
//	case WM_INPUT:
//	{
//		UINT dwSize = 0;
//		GetRawInputData(reinterpret_cast<HRAWINPUT>(lparam), RID_INPUT, nullptr, &dwSize, sizeof(RAWINPUTHEADER));
//		LPBYTE lpb = new BYTE[dwSize];
//		if (lpb == nullptr) {
//			return 0;
//		}
//
//		if (GetRawInputData((HRAWINPUT)lparam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
//			OutputDebugString(TEXT("GetRawInputData does not return correct size !\n"));
//
//		RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(lpb);
//
//		if (raw->header.dwType == RIM_TYPEKEYBOARD)
//		{
//			//printf(" Kbd: make=%04i Flags:%04i Reserved:%04i ExtraInformation:%08i, msg=%04i VK=%i \n",
//			//	raw->data.keyboard.MakeCode,
//			//	raw->data.keyboard.Flags,
//			//	raw->data.keyboard.Reserved,
//			//	raw->data.keyboard.ExtraInformation,
//			//	raw->data.keyboard.Message,
//			//	raw->data.keyboard.VKey);
//
//			_app->getInput()->OnKeyDown(InputDevice::KeyboardInputEventArgs{
//				raw->data.keyboard.MakeCode,
//				raw->data.keyboard.Flags,
//				raw->data.keyboard.VKey,
//				raw->data.keyboard.Message
//				});
//
//			/*OnKeyDown.Broadcast({
//				raw->data.keyboard.MakeCode,
//				raw->data.keyboard.Flags,
//				raw->data.keyboard.VKey,
//				raw->data.keyboard.Message
//				});*/
//		}
//		else if (raw->header.dwType == RIM_TYPEMOUSE)
//		{
//			//printf(" Mouse: X=%04d Y:%04d \n", raw->data.mouse.lLastX, raw->data.mouse.lLastY);
//			//TODO: This delegates won't work rn
//
//			_app->getInput()->OnMouseMove(InputDevice::RawMouseEventArgs{
//			raw->data.mouse.usFlags,
//				raw->data.mouse.usButtonFlags,
//				static_cast<int>(raw->data.mouse.ulExtraInformation),
//				static_cast<int>(raw->data.mouse.ulRawButtons),
//				static_cast<short>(raw->data.mouse.usButtonData),
//				raw->data.mouse.lLastX,
//				raw->data.mouse.lLastY
//				});
//
//
//
//			/*OnMouseMove.Broadcast({
//				raw->data.mouse.usFlags,
//				raw->data.mouse.usButtonFlags,
//				static_cast<int>(raw->data.mouse.ulExtraInformation),
//				static_cast<int>(raw->data.mouse.ulRawButtons),
//				static_cast<short>(raw->data.mouse.usButtonData),
//				raw->data.mouse.lLastX,
//				raw->data.mouse.lLastY
//				});*/
//		}
//
//		delete[] lpb;
//		return DefWindowProc(hwnd, umessage, wparam, lparam);
//	}
//	default:
//	{
//		return DefWindowProc(hwnd, umessage, wparam, lparam);
//	}
//	}
//}
//
//bool WinApi_Display::CreateDisplay()
//{
//
//	hInstance = GetModuleHandle(nullptr);
//	applicationName = L"WndClass";
//
//	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
//	wc.lpfnWndProc = WndProcStatic;
//	wc.cbClsExtra = 0;
//	wc.cbWndExtra = 0;
//	wc.hInstance = hInstance;
//	wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
//	wc.hIconSm = wc.hIcon;
//	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
//	wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
//	wc.lpszMenuName = nullptr;
//	wc.lpszClassName = applicationName;
//	wc.cbSize = sizeof(WNDCLASSEX);
//
//	RegisterClassEx(&wc);
//
//
//	windowRect = { 0, 0, static_cast<LONG>(screenWidth), static_cast<LONG>(screenHeight) };
//
//	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
//
//	auto dwStyle = WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_THICKFRAME;
//
//	auto posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
//	auto posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
//
//	hWnd = CreateWindowExA(0, WINDOW_CLASS_NAME, "",
//		dwStyle,
//		posX, posY,
//		windowRect.right - windowRect.left,
//		windowRect.bottom - windowRect.top,
//		nullptr, nullptr, GetModuleHandleA(nullptr), this);
//	if (!hWnd) {
//		std::cout << "Error while creating window!\n";
//		__debugbreak();
//	}
//
//
//	SetRawInputDevice();
//	ShowClientWindow();
//
//
//
//
//	return false;
//}
//
//void WinApi_Display::ShowClientWindow()
//{
//	ShowWindow(hWnd, SW_SHOW);
//	SetForegroundWindow(hWnd);
//	SetFocus(hWnd);
//	ShowCursor(true);
//	UpdateWindow(hWnd);
//
//}
//
