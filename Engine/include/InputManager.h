#pragma once

#include <string>
#include <map>

#include <GLFW/glfw3.h>

#include "include/InputContext.h"
#include "include/Singleton.h"

namespace NovaEngine {

const std::string DEFAULT_CONTEXT_NAME = "Default";

class InputManager: public Singleton<InputManager> {
    public:
    InputManager();
    [[nodiscard]] bool hasContext(const std::string &contextName) const { return _contextMaps.contains(contextName); };
    ContainerAdditionResult addContext(const std::string &contextName);
    InputContext& getContext(const std::string &contextName);
    ContainerQueryResult switchContext(const std::string &contextName);
    ContainerRemovalResult removeContext(const std::string &contextName);
    void addEvent(InputEvent event);

    static void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void mousePositionCallback(GLFWwindow* window, double xpos, double ypos);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    private:
    static KeyboardModifiers checkKeyboardModifiers(GLFWwindow *window);

    std::string _activeName = DEFAULT_CONTEXT_NAME;
    std::map<std::string, InputContext> _contextMaps;
    std::map<InputSource, InputEvent> _lastInputs;
};

}
