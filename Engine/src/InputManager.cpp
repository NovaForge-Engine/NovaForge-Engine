#include <spdlog/spdlog.h>

#include <utility>

#include "include/InputManager.h"

namespace NovaEngine {

InputManager::InputManager() {
    _contextMaps[DEFAULT_CONTEXT_NAME] = InputContext(DEFAULT_CONTEXT_NAME);
}

ContainerAdditionResult InputManager::addContext(const std::string &contextName) {
    if (hasContext(contextName)) {
        return ContainerAdditionResult::ALREADY_EXISTS;
    }
    _contextMaps[contextName] = InputContext(contextName);
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
        spdlog::warn("Tried to switch to an already active context \"{}\"", contextName);
    }
    _activeName = contextName;
    return ContainerQueryResult::SUCCESS;
}

void InputManager::addEvent(InputEvent event) {
    _contextMaps[_activeName].processEvent(std::move(event));
}

} // namespace NovaEngine