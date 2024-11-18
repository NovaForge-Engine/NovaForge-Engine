#pragma once

#include "include/InputSource.h"

namespace NovaEngine {

class InputEvent {
    public:
    InputEvent(InputSource source, InputValueVariant absolute, InputValueVariant relative):
        _source(std::move(source)), _absolute(absolute), _relative(relative) {};
    [[nodiscard]] InputValueVariant getAbsolute() const { return _absolute; };
    [[nodiscard]] InputValueVariant getRelative() const { return _relative; };
    [[nodiscard]] InputSource getSource() const { return _source; };
    [[nodiscard]] EventAxes getAxes() const { return _source.getType(); };
    
    private:
    InputSource _source;
    InputValueVariant _absolute, _relative;
};

} // namespace NovaEngine