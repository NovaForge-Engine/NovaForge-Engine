#pragma once

#include <set>
#include <map>
#include <string>
#include <optional>
#include <functional>

#include <glm/vec2.hpp>
#include <GLFW/glfw3.h>

#include "include/Singleton.h"
#include "Export.h"

namespace NovaEngine {

struct GamepadState {
    std::set<int> buttonsHeld;
	std::map<int, double> triggerLastValues;
	std::map<int, glm::vec2> stickLastValues;
};

/*!
 * \brief Class for supplementing gamepad input
 *
 * This class is a singleton for providing additional callbacks to inputs. It allows to overcome 
 * lack of gamepad input callbacks in GLFW, which is required for easier implementation of in-game controls. 
 */

void ENGINE_DLL GLFWJoystickConnectionCallback(int jid, int event);


class ENGINE_DLL GamepadManager : public Singleton<GamepadManager>
{
public:
	GamepadManager();

	void pollGamepadEvents();
	bool hasActiveGamepad(int id);
	bool hasActiveGamepads();
	std::string getGamepadName(int id);
	void searchForActiveGamepads();

	void JoystickConnectionCallback(int jid, int event);

	void ConnectJoystick(int id);
	void DisconnectJoystick(int id);


private:

	static void callbackJoystickConnectionChanged(int jid, int event);

	std::map<int, GamepadState> _activeGamepads;

    static constexpr int GLFW_GAMEPAD_AXIS_LEFT = GLFW_GAMEPAD_AXIS_LEFT_X | GLFW_GAMEPAD_AXIS_LEFT_Y;
    static constexpr int GLFW_GAMEPAD_AXIS_RIGHT = GLFW_GAMEPAD_AXIS_RIGHT_X | GLFW_GAMEPAD_AXIS_RIGHT_Y;
};

} // namespace NovaEngine
