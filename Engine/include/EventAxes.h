#pragma once

#include <variant>

#include <glm/glm.hpp>

namespace NovaEngine {

enum class EventAxes {
    BUTTON,
    SINGLE,
    DOUBLE
};

using InputValueVariant = std::variant<bool, double, glm::vec2>;

bool eventAxesValueMatches(InputValueVariant value, EventAxes axes);

} // namespace NovaEngine
