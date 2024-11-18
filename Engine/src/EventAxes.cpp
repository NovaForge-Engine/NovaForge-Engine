#include "include/EventAxes.h"

namespace NovaEngine {

bool eventAxesValueMatches(InputValueVariant value, EventAxes axes) {
    const bool isButton = (value.index() == InputValueVariant(false).index()) && (axes == EventAxes::BUTTON);
    const bool isSingle = (value.index() == InputValueVariant(0.f).index()) && (axes == EventAxes::SINGLE);
    const bool isDouble = (value.index() == InputValueVariant(glm::vec2()).index()) && (axes == EventAxes::DOUBLE);
    return (isButton) || (isSingle) || (isDouble);
};

}
