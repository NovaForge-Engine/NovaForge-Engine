#include <glm/glm.hpp>

#include "include/InputContext.h"

namespace NovaEngine {

/* maaxxaam @ 17.11.24
 * The return type on this should be along the lines of
 * std::vector<std::pair<InputBinding, InputSource>>::iterator
 * However, clang does not accept it yet auto just works :shrug:
 */
auto InputContext::searchBinding(const std::string &name) {
    return std::find_if(_bindings.begin(), _bindings.end(),
        [name] (const std::pair<InputBinding, InputSource> &item) {
            return item.first.getName() == name;
        });
}

bool InputContext::hasBinding(const std::string &actionName) {
    return searchBinding(actionName) != _bindings.end();
}

InputBinding& InputContext::getBinding(const std::string &actionName) {
    auto binding = searchBinding(actionName);
    return binding->first;
}

ContainerAdditionResult InputContext::addBinding(InputBinding action, const InputSourceVariant source) {
    if (hasBinding(action.getName())) return ContainerAdditionResult::ALREADY_EXISTS;
    const InputSource sourceToAdd(source);
    _bindings.emplace_back(action, sourceToAdd);
    return ContainerAdditionResult::SUCCESS;
}

ContainerQueryResult InputContext::attachToBinding(InputDelegate newFunction, const std::string &actionName) {
    auto binding = searchBinding(actionName);
    const bool hasBinding = binding != _bindings.end();
    if (!hasBinding) return ContainerQueryResult::NOT_FOUND;
    binding->first.addSubscriber(newFunction);
    return ContainerQueryResult::SUCCESS;
}

ContainerQueryResult InputContext::removeBinding(const std::string &actionName) {
    auto binding = searchBinding(actionName);
    const bool hasBinding = binding != _bindings.end();
    if (!hasBinding) return ContainerQueryResult::NOT_FOUND;
    _bindings.erase(binding);
    return ContainerQueryResult::SUCCESS;
}

ContainerQueryResult InputContext::processEvent(InputEvent event) {
    const InputSource source = event.getSource();
    auto ptr = _bindings.begin();
    while (true) {
        auto searchResult = std::find_if(ptr, _bindings.end(),
            [source] (const std::pair<InputBinding, InputSource> &item) {
                return source == item.second;
            });
        if (searchResult == _bindings.end()) {
            break;
        }
        EventType type = searchResult->first.getType();
        bool inform = (type == EventType::STARTED && isStartEvent(event)) ||
         (type == EventType::ENDED && isEndEvent(event)) ||
         (type == EventType::CHANGED && !isEndEvent(event) && !isStartEvent(event));
        if (inform) {
            searchResult->first.informSubscribers(event);
        }
        ptr = ++searchResult;
    }
    return ContainerQueryResult::SUCCESS;
}

bool InputContext::isStartEvent(const InputEvent &event) {
    InputValueVariant absValue = event.getAbsolute();
    InputValueVariant relValue = event.getRelative();
    return absValue == relValue;
}

std::map<InputBinding, InputSource> InputContext::getBindings() const {
    std::map<InputBinding, InputSource> result;
    for (auto &item: _bindings) {
        result.emplace(item.first, item.second);
    }
    return result;
}

bool InputContext::isEndEvent(const InputEvent &event) {
    InputValueVariant absValue = event.getAbsolute();
    if (absValue.index() == InputValueVariant(false).index()) return std::get<bool>(absValue) == false;
    if (absValue.index() == InputValueVariant(0.f).index()) return std::get<double>(absValue) == 0.f;
    if (absValue.index() == InputValueVariant(glm::vec2()).index()) return std::get<glm::vec2>(absValue) == glm::vec2(0.f);
    // default case
    return false;
}

} // namespace NovaEngine