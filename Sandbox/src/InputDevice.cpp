//// This is a personal academic project. Dear PVS-Studio, please check it.
//// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
//#include "InputDevice.h"
//#include "Application.h"
//#include "../external/ImGui/imgui.h"
//using namespace DirectX::SimpleMath;
//
//
//
//InputDevice::~InputDevice()
//{
//	keys->clear();
//}
//
//void InputDevice::OnKeyDown(KeyboardInputEventArgs args)
//{
//	bool Break = args.Flags & 0x01;
//
//	auto key = static_cast<Keys>(args.VKey);
//
//	if (args.MakeCode == 42) key = Keys::LeftShift;
//	if (args.MakeCode == 54) key = Keys::RightShift;
//
//	if (Break) {
//		if (keys->count(key))	RemovePressedKey(key);
//	}
//	else {
//		if (!keys->count(key))	AddPressedKey(key);
//	}
//}
//
//
//
//void InputDevice::OnMouseWheel(float mouseWheel)
//{
//}
//
//
//
//void InputDevice::OnMouseKey(int keyCode, bool isDown)
//{
//
//}
//
//void InputDevice::OnMouseMove(RawMouseEventArgs args)
//{
//
//
//	if (args.ButtonFlags & static_cast<int>(MouseButtonFlags::LeftButtonDown))
//		AddPressedKey(Keys::LeftButton);
//	if (args.ButtonFlags & static_cast<int>(MouseButtonFlags::LeftButtonUp))
//		RemovePressedKey(Keys::LeftButton);
//	if (args.ButtonFlags & static_cast<int>(MouseButtonFlags::RightButtonDown))
//		AddPressedKey(Keys::RightButton);
//	if (args.ButtonFlags & static_cast<int>(MouseButtonFlags::RightButtonUp))
//		RemovePressedKey(Keys::RightButton);
//	if (args.ButtonFlags & static_cast<int>(MouseButtonFlags::MiddleButtonDown))
//		AddPressedKey(Keys::MiddleButton);
//	if (args.ButtonFlags & static_cast<int>(MouseButtonFlags::MiddleButtonUp))
//		RemovePressedKey(Keys::MiddleButton);
//
//	MouseOffset.x = args.X;
//	MouseOffset.y = args.Y;
//
//
//	DOnMouseMove.Broadcast(MouseOffset);
//	MouseWheelDelta = args.WheelDelta;
//	//const MouseMoveEventArgs moveArgs = {MousePosition, MouseOffset, MouseWheelDelta};
//
//}
//
//void InputDevice::OnChangeScreenSize(int width, int height) {
//	ScreenParam.Width = width;
//	ScreenParam.Height = height;
//};
//
//
//void InputDevice::Initialize()
//{
//	keys = new std::unordered_set<Keys>();
//}
//
//
//void InputDevice::AddPressedKey(Keys key)
//{
//	//if (!game->isActive) {
//	//	return;
//	//}
//	keys->insert(key);
//}
//
//void InputDevice::RemovePressedKey(Keys key)
//{
//	keys->erase(key);
//}
//
//bool InputDevice::IsKeyDown(Keys key)
//{
//	return keys->count(key);
//}
//
