#include <fmt/format.h>

#include <fstream>

#include "include/AssetMetadata.h"

namespace NovaEngine {

AssetMetadata::AssetMetadata(const std::filesystem::path& file) {
    if (!std::filesystem::exists(file)) {
        throw std::runtime_error(fmt::format("File {:?} does not exists...", file.string()));
    }
    _filePath = file;
    _metaPath = std::filesystem::path(file.string() + META_SUBSTR);
    if (!std::filesystem::exists(_metaPath)) {
        _data = toml::value();
    } else {
        _data = toml::parse(_metaPath);
    }
}

void AssetMetadata::replaceWith(const toml::value& newMetadata) {
    _data = newMetadata;
}

void AssetMetadata::saveToDisk() {
    std::ofstream outFile(_metaPath);
    outFile << toml::format(_data);
    outFile.close();
}

} // namespace NovaEngine