#pragma once

#include <string>
#include <optional>

#include <entt/entt.hpp>
#include <utility>

#include "include/ErrorEnums.h"
#include "include/InputEvent.h"

namespace NovaEngine {

enum class EventType {
    STARTED,
    CHANGED,
    ENDED
};

using InputDelegate = entt::delegate<void (InputEvent)>;
using InputValueConversionDelegate = entt::delegate<InputValueVariant (InputValueVariant, EventAxes)>;

class InputBinding {
    public:
    InputBinding(std::string name, EventAxes axes, EventType type):
        _name(std::move(name)), _axes(axes), _type(type) {};
    InputBinding(const InputBinding &other);
    InputBinding(const InputBinding &&other) noexcept;
    InputBinding(InputBinding &other);
    InputBinding(InputBinding &&other) noexcept;
    InputBinding& operator=(const InputBinding& other);
    
    void informSubscribers(InputEvent event);
    ContainerAdditionResult addSubscriber(InputDelegate newSubscriber);
    ContainerQueryResult removeSubscriber(InputDelegate toRemove);
    void changeConversionFunction(InputValueConversionDelegate newFunction) { _conversionFunction = newFunction; };
    [[nodiscard]] std::string getName() const { return _name; };
    [[nodiscard]] EventType getType() const { return _type; };

    bool operator < (const InputBinding &other) const { return _name < other._name; };

    private:
    EventAxes _axes;
    EventType _type;
    std::string _name;
    std::optional<InputValueConversionDelegate> _conversionFunction;
    std::vector<InputDelegate> _subscribers;
};

} // namespace NovaEngine
