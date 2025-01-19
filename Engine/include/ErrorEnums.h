#pragma once

namespace NovaEngine {

enum class ContainerAdditionResult {
    SUCCESS = 0,
    ALREADY_EXISTS = 1
};

enum class ContainerQueryResult {
    SUCCESS = 0,
    NOT_FOUND = 1
};

enum class ContainerRemovalResult {
    SUCCESS = 0,
    NOT_FOUND = 1,
    CANNOT_REMOVE = 2
};

} // namespace NovaEngine