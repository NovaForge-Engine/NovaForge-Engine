#pragma once

#include "include/InputBinding.h"

namespace NovaEngine {

class InputContext {
    public:
    explicit InputContext(std::string name): _name(std::move(name)) {};

    [[nodiscard]] std::string getName() const { return _name; };
    ContainerAdditionResult addBinding(InputBinding action, InputSourceVariant source);
    ContainerQueryResult attachToBinding(InputDelegate newFunction, const std::string &actionName);
    ContainerQueryResult removeBinding(const std::string &actionName);
    [[nodiscard]] std::map<InputBinding, InputSource> getBindings() const;
    bool hasBinding(const std::string &actionName);
    InputBinding& getBinding(const std::string &actionName);
    ContainerQueryResult processEvent(InputEvent event);

    private:
    auto searchBinding(const std::string &name);
    static bool isStartEvent(const InputEvent &event);
    static bool isEndEvent(const InputEvent &event);

    std::string _name;
    std::vector<std::pair<InputBinding, InputSource>> _bindings;
};

} // namespace NovaEngine