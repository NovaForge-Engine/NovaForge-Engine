#pragma once

#include <variant>

#include "include/InputSourceEnums.h"
#include "include/EventAxes.h"

namespace NovaEngine {

using InputSourceVariant = std::variant<KeyboardSource, MouseSource, GamepadSource>;

enum class InputDeviceType {
    KEYBOARD = 0,
    MOUSE = 1,
    GAMEPAD = 2
};

class InputSource {
    public:
    explicit InputSource(InputSourceVariant source);
    InputSource(const InputSource& other);
    InputSource(const InputSource&& other) noexcept;
    InputSource(InputSource& other);
    InputSource(InputSource&& other) noexcept;
    InputSource& operator=(const InputSource& other);
    bool operator==(const InputSource& other) const;
    bool operator<(const InputSource& other) const;
    [[nodiscard]] EventAxes getType() const { return _type; };
    [[nodiscard]] InputDeviceType getDevice() const { return _device; };
    [[nodiscard]] InputSourceVariant getSource() const { return _source; };

    private:
    InputSourceVariant _source;
    InputDeviceType _device;
    EventAxes _type;
};

} // namespace NovaEngine
