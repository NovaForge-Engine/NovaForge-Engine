#include <utility>
#include <cstddef>
#include "spdlog/spdlog.h"
#include "include/InputManager.h"
#include <iostream>

namespace NovaEngine {;


InputManager::InputManager() {
    _contextMaps.insert(std::make_pair(DEFAULT_CONTEXT_NAME, InputContext(DEFAULT_CONTEXT_NAME)));


}



ContainerAdditionResult InputManager::addContext(const std::string &contextName) {
    if (hasContext(contextName)) {
        return ContainerAdditionResult::ALREADY_EXISTS;
    }
    std::pair<std::string, InputContext> contextPair(contextName, InputContext(contextName));
    _contextMaps.insert(std::move(contextPair));
    return ContainerAdditionResult::SUCCESS;
}

ContainerRemovalResult InputManager::removeContext(const std::string &contextName) {
    if (!hasContext(contextName)) {
        return ContainerRemovalResult::NOT_FOUND;
    }
    if (contextName == DEFAULT_CONTEXT_NAME || contextName == _activeName) {
        return ContainerRemovalResult::CANNOT_REMOVE;
    }
    _contextMaps.erase(contextName);
    return ContainerRemovalResult::SUCCESS;
}

ContainerQueryResult InputManager::switchContext(const std::string &contextName) {
    if (!hasContext(contextName)) {
        return ContainerQueryResult::NOT_FOUND;
    }
    if (contextName == _activeName) {
        //spdlog::warn("Tried to switch to an already active context \"{}\"", contextName);
    }
    _activeName = contextName;
    return ContainerQueryResult::SUCCESS;
}

void InputManager::addEvent(InputEvent event) {
    _contextMaps.at(_activeName).processEvent(std::move(event));
}

InputContext& InputManager::getContext(const std::string &contextName) {
    if (!_contextMaps.contains(contextName)) {
        throw std::runtime_error("Access to unknown context. You can use hasContext() to check if one exists.");
    } else {
        InputContext& context = _contextMaps.at(contextName);
		return context;
    }
};

void InputManager::keyboardCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    const bool what = action == GLFW_RELEASE ? false : true;
    const auto keySource = static_cast<KeyboardSource>(key);
    InputSource source(keySource);
    InputEvent event(source, what, true, mods);
    InputManager::instance().addEvent(event);
    InputManager::instance()._lastInputs.insert_or_assign(source, event);
}

void InputManager::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    const bool what = action == GLFW_RELEASE ? false : true;
    const auto mouseSource = static_cast<MouseSource>(button);
    InputSource source(mouseSource);
    InputEvent event(source, what, true, mods);
    InputManager::instance().addEvent(event);
    InputManager::instance()._lastInputs.insert_or_assign(source, event);
}

void InputManager::mousePositionCallback(GLFWwindow *window, double xpos, double ypos) {
    const KeyboardModifiers mods = checkKeyboardModifiers(window);
    InputSource source(MouseSource::MOVEMENT);
    glm::vec2 absValue(xpos, ypos);
    glm::vec2 oldValue(0.f);
    if (InputManager::instance()._lastInputs.contains(source)) {
        oldValue = std::get<glm::vec2>(InputManager::instance()._lastInputs.find(source)->second.getAbsolute());
    }
    glm::vec2 relValue = absValue - oldValue;
    InputEvent event(source, absValue, relValue, mods);
    InputManager::instance().addEvent(event);
    InputManager::instance()._lastInputs.insert_or_assign(source, event);
}

void InputManager::mouseScrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
    const KeyboardModifiers mods = checkKeyboardModifiers(window);
    InputSource source(MouseSource::SCROLL_HORIZONTAL);
    double absValue(xoffset);
    double relValue = absValue;
    if (absValue != 0.f) {
        InputEvent event(source, absValue, relValue, mods);
        InputManager::instance().addEvent(event);
        InputManager::instance()._lastInputs.insert_or_assign(source, event);
    }
    source = InputSource(MouseSource::SCROLL_VERTICAL);
    absValue = yoffset;
    relValue = absValue;
    if (absValue != 0.f) {
        InputEvent event(source, absValue, relValue, mods);
        InputManager::instance().addEvent(event);
        InputManager::instance()._lastInputs.insert_or_assign(source, event);
    }
}

KeyboardModifiers InputManager::checkKeyboardModifiers(GLFWwindow *window) {
    KeyboardModifiers result;
    const bool caps_state = (glfwGetKey(window, GLFW_KEY_CAPS_LOCK) == GLFW_PRESS);
    const bool num_state = (glfwGetKey(window, GLFW_KEY_NUM_LOCK) == GLFW_PRESS);
    const bool ctrl_state = (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS);
    const bool shift_state = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);
    const bool alt_state = (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS);
    const bool super_state = (glfwGetKey(window, GLFW_KEY_LEFT_SUPER) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_SUPER) == GLFW_PRESS);
    return (
        static_cast<uint8_t>(KeyboardModifier::WIN_SUPER_COMMAND) * super_state ||
        static_cast<uint8_t>(KeyboardModifier::ALT) * alt_state ||
        static_cast<uint8_t>(KeyboardModifier::SHIFT) * shift_state ||
        static_cast<uint8_t>(KeyboardModifier::CTRL) * ctrl_state ||
        static_cast<uint8_t>(KeyboardModifier::CAPS_LOCK) * caps_state ||
        static_cast<uint8_t>(KeyboardModifier::NUM_LOCK) * num_state 
    );
}

} // namespace NovaEngine