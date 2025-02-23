#include <stdexcept>

#include "include/InputSource.h"

namespace NovaEngine {

InputSource::InputSource(const InputSourceVariant source): _source(source) {
    const bool isMouse = source.index() == InputSourceVariant(MouseSource::BUTTON_1).index();
    const bool isKeyboard = source.index() == InputSourceVariant(KeyboardSource::KEY_0).index();
    const bool isGamepad = source.index() == InputSourceVariant(GamepadSource::BUTTON_A).index();

    if (isMouse) {
        _device = InputDeviceType::MOUSE;
        const auto mouseSource = std::get<MouseSource>(source);
        if (mouseSource== MouseSource::MOVEMENT) {
            _type = EventAxes::DOUBLE;
        } else if ((mouseSource == MouseSource::SCROLL_HORIZONTAL) || (mouseSource == MouseSource::SCROLL_VERTICAL)) {
            _type = EventAxes::SINGLE;
        } else {
            _type = EventAxes::BUTTON;
        }
    } else if (isKeyboard) {
        _device = InputDeviceType::KEYBOARD;
        _type = EventAxes::BUTTON;
    } else if (isGamepad) {
        _device = InputDeviceType::GAMEPAD;
        const auto gamepadSource = std::get<GamepadSource>(source);
        const bool isGamepadButton = static_cast<uint8_t>(gamepadSource) < static_cast<uint8_t>(GamepadSource::AXIS_LEFT_X);
        const bool isGamepadSingleAxis = static_cast<uint8_t>(gamepadSource) < static_cast<uint8_t>(GamepadSource::STICK_LEFT) &&
            static_cast<uint8_t>(gamepadSource) >= static_cast<uint8_t>(GamepadSource::AXIS_LEFT_X);
        const bool isGamepadStick = static_cast<uint8_t>(gamepadSource) >= static_cast<uint8_t>(GamepadSource::STICK_LEFT);
        if (isGamepadButton) {
            _type = EventAxes::BUTTON;
        } else if (isGamepadSingleAxis) {
            _type = EventAxes::SINGLE;
        } else if (isGamepadStick) { 
            _type = EventAxes::DOUBLE;
        }
    } else {
        throw std::runtime_error("No idea what this input source is supposed to be");
    }
};

InputSource& InputSource::operator=(const InputSource& other) {
    if (this != &other) {
        _source = other._source;
        _device = other._device;
        _type = other._type;
    }
    return *this;
};

bool InputSource::operator==(const InputSource& other) const {
    return _source == other._source;
}

bool InputSource::operator<(const InputSource& other) const {
    if (other.getDevice() != getDevice()) {
        return getDevice() < other.getDevice();
    } else {
        return _source < other._source;
    }
}

InputSource::InputSource(const InputSource &&other) noexcept {
    _source = other._source;
    _device = other._device;
    _type = other._type;
}

InputSource::InputSource(const InputSource &other): _source(other._source) {
    _device = other._device;
    _type = other._type;
}

InputSource::InputSource(InputSource &&other) noexcept {
    _source = other._source;
    _device = other._device;
    _type = other._type;
}

InputSource::InputSource(InputSource &other): _source(other._source) {
    _device = other._device;
    _type = other._type;
}

} // namespace NovaEngine