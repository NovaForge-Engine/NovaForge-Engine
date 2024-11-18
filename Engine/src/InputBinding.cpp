#include <stdexcept>

#include "include/InputBinding.h"

namespace NovaEngine {

void InputBinding::informSubscribers(InputEvent event) {
    const EventAxes eventAxes = event.getAxes();
    InputValueVariant absValue = event.getAbsolute();
    InputValueVariant relValue = event.getRelative();
    if (eventAxes != _axes) {
        if (!_conversionFunction.has_value())
            throw std::runtime_error("Event axes are mismatched and no conversion routine provided");
        absValue = (*_conversionFunction)(absValue, _axes);
        relValue = (*_conversionFunction)(relValue, _axes);
        if (!eventAxesValueMatches(absValue, _axes))
            throw std::runtime_error("Bad conversion routine: values still don't match expected shape");
        event = InputEvent(event.getSource(), absValue, relValue);
    }
    for (auto &delegate: _subscribers) {
        delegate(event);
    }
}

ContainerAdditionResult InputBinding::addSubscriber(InputDelegate newSubscriber) {
    const bool hasDelegate = std::find(_subscribers.begin(), _subscribers.end(), newSubscriber) != _subscribers.end();
    if (hasDelegate) return ContainerAdditionResult::ALREADY_EXISTS;
    _subscribers.push_back(newSubscriber);
    return ContainerAdditionResult::SUCCESS;
}

ContainerQueryResult InputBinding::removeSubscriber(InputDelegate toRemove) {
    const auto delegatePosition = std::find(_subscribers.begin(), _subscribers.end(), toRemove);
    const bool hasDelegate = delegatePosition != _subscribers.end();
    if (!hasDelegate) return ContainerQueryResult::NOT_FOUND;
    _subscribers.erase(delegatePosition);
    return ContainerQueryResult::SUCCESS;
}

InputBinding& InputBinding::operator=(const InputBinding& other) {
    if (this != &other) {
        _axes = other._axes;
        if (other._conversionFunction) _conversionFunction = other._conversionFunction;
        _name = other.getName();
        _type = other._type;
        _subscribers = other._subscribers;
    }
    return *this;
};

InputBinding::InputBinding(InputBinding &&other) noexcept{
    _axes = other._axes;
    if (other._conversionFunction) _conversionFunction = other._conversionFunction;
    _name = other._name;
    _type = other._type;
    _subscribers = other._subscribers;
};

InputBinding::InputBinding(InputBinding &other) {
    _axes = other._axes;
    if (other._conversionFunction) _conversionFunction = other._conversionFunction;
    _name = other._name;
    _type = other._type;
    _subscribers = other._subscribers;
};

} // namespace NovaEngine