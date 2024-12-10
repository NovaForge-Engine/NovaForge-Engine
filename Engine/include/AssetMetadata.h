#pragma once

#include <toml.hpp>

#include <filesystem>

namespace NovaEngine {

constexpr std::string META_EXTENSION = "META";
constexpr std::string META_SUBSTR = "." + META_EXTENSION;

struct AssetMetadata {
    AssetMetadata(const std::filesystem::path& file);
    [[nodiscard]] const toml::value& data() const { return _data; };
    void replaceWith(const toml::value& newMetadata);
    void saveToDisk();

    private:
    std::filesystem::path _filePath, _metaPath;
    toml::value _data;
};

} // namespace NovaEngine