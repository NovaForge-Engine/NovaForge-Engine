#include <glm/glm.hpp>

#include "include/InputManager.h"

#include "include/GamepadManager.h"

namespace NovaEngine {

GamepadManager::GamepadManager() {
    glfwSetJoystickCallback(&GamepadManager::callbackJoystickConnectionChanged);

    // see if there are any connected gamepads already
    searchForActiveGamepads();
}

void GamepadManager::callbackJoystickConnectionChanged(int jid, int event) {
    if (event == GLFW_CONNECTED) {
        if (glfwJoystickIsGamepad(jid)) {
            //spdlog::debug("Gamepad {} with id {} has been connected", GamepadManager::instance().getGamepadName(jid), jid);
            GamepadManager::instance()._activeGamepads[jid] = GamepadState();
        }
    } else if (event == GLFW_DISCONNECTED) {
        //spdlog::debug("Gamepad with id {} has been disconnected", jid);
        GamepadManager::instance()._activeGamepads.erase(jid);
    }
}

void GamepadManager::searchForActiveGamepads() {
    for (int gid = 0; gid < GLFW_JOYSTICK_LAST; gid++) {
        if (glfwJoystickIsGamepad(gid)) {
            //spdlog::debug("Found connected gamepad {} with id {} to use", getGamepadName(gid), gid);
            _activeGamepads[gid] = GamepadState();
        }
    }
}

void GamepadManager::pollGamepadEvents() {
    if (!hasActiveGamepads())
        return;
    
    GLFWgamepadstate newState;
    for (auto &activeState: _activeGamepads) {
        glfwGetGamepadState(activeState.first, &newState);
        auto gamepadState = activeState.second;
        for (int i = 0; i < GLFW_GAMEPAD_BUTTON_LAST; i++) {
            if (newState.buttons[i] == GLFW_PRESS) {
                if (gamepadState.buttonsHeld.contains(i)) {
                    gamepadState.buttonsHeld.insert(i);

                    InputSource source(static_cast<GamepadSource>(i));
                    InputEvent newEvent(source, true, true, activeState.first);
                    InputManager::instance().addEvent(newEvent);
                }
            } else if (newState.buttons[i] == GLFW_RELEASE) {
                if (gamepadState.buttonsHeld.count(i)) {
                    gamepadState.buttonsHeld.erase(i);

                    InputSource source(static_cast<GamepadSource>(i));
                    InputEvent newEvent(source, false, true, activeState.first);
                    InputManager::instance().addEvent(newEvent);
                }
            }
        }

        // Create and pass stick events
        glm::vec2 axisNew = glm::vec2(newState.axes[GLFW_GAMEPAD_AXIS_LEFT_X], newState.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]);
        auto iAxisLast = gamepadState.stickLastValues.find(GLFW_GAMEPAD_AXIS_LEFT);
        glm::vec2 axisLast = iAxisLast == gamepadState.stickLastValues.end() ? glm::vec2(0.f) : iAxisLast->second;

        glm::vec2 delta = axisNew - axisLast;
        if (delta != glm::vec2(0.f)) {
            InputSource source(static_cast<GamepadSource>(GamepadSource::STICK_LEFT));
            InputEvent newEvent(source, axisNew, delta, activeState.first);
            InputManager::instance().addEvent(newEvent);
        }
        gamepadState.stickLastValues[GLFW_GAMEPAD_AXIS_LEFT] = axisNew;
        
        axisNew = glm::vec2(newState.axes[GLFW_GAMEPAD_AXIS_RIGHT_X], newState.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]);
        iAxisLast = gamepadState.stickLastValues.find(GLFW_GAMEPAD_AXIS_LEFT);
        axisLast = iAxisLast == gamepadState.stickLastValues.end() ? glm::vec2(0.f) : iAxisLast->second;
        
        delta = axisNew - axisLast;
        if (delta != glm::vec2(0.f)) {
            InputSource source(static_cast<GamepadSource>(GamepadSource::STICK_RIGHT));
            InputEvent newEvent(source, axisNew, delta, activeState.first);
            InputManager::instance().addEvent(newEvent);
        }
        gamepadState.stickLastValues[GLFW_GAMEPAD_AXIS_RIGHT] = axisNew;

        // Create and pass trigger events
        double triggerNew = newState.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER];
        double triggerLast = gamepadState.triggerLastValues.find(GLFW_GAMEPAD_AXIS_LEFT_TRIGGER)->second;

        double tdelta = triggerNew - triggerLast;
        if (tdelta != 0.f) {
            InputSource source(static_cast<GamepadSource>(GamepadSource::AXIS_LEFT_TRIGGER));
            InputEvent newEvent(source, triggerNew, tdelta, activeState.first);
            InputManager::instance().addEvent(newEvent);
        }
        gamepadState.triggerLastValues[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] = triggerNew;

        triggerNew = newState.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER];
        triggerLast = gamepadState.triggerLastValues.find(GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER)->second;

        tdelta = triggerNew - triggerLast;
        if (tdelta != 0.f) {
            InputSource source(static_cast<GamepadSource>(GamepadSource::AXIS_RIGHT_TRIGGER));
            InputEvent newEvent(source, triggerNew, tdelta, activeState.first);
            InputManager::instance().addEvent(newEvent);
        }
        gamepadState.triggerLastValues[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] = triggerNew;
    }
}

bool GamepadManager::hasActiveGamepad(int id) {
    return _activeGamepads.contains(id);
}

bool GamepadManager::hasActiveGamepads() {
    return !_activeGamepads.empty();
}

std::string GamepadManager::getGamepadName(int id) {
    if (hasActiveGamepad(id))
        return { glfwGetGamepadName(id) };
    else
        return {};
}

} // End of namespace Platform
