#pragma once

#include "include/InputSource.h"

namespace NovaEngine {

class InputEvent {
    public:
    InputEvent(InputSource source, InputValueVariant absolute, InputValueVariant relative):
        _source(std::move(source)), _absolute(absolute), _relative(relative) {};
    InputEvent(InputSource source, InputValueVariant absolute, InputValueVariant relative, uint8_t extraData):
        _source(std::move(source)), _absolute(absolute), _relative(relative), _extraData(extraData) {};
    [[nodiscard]] InputValueVariant getAbsolute() const { return _absolute; };
    [[nodiscard]] InputValueVariant getRelative() const { return _relative; };
    [[nodiscard]] uint8_t getExtraData() const { return _extraData; };
    [[nodiscard]] InputSource getSource() const { return _source; };
    [[nodiscard]] EventAxes getAxes() const { return _source.getType(); };
    
    private:
    InputSource _source;
    InputValueVariant _absolute, _relative;
    uint8_t _extraData = 0;
};

} // namespace NovaEngine