#pragma once

#include <filesystem>

#include <toml.hpp>

#include "FileFingerprint.h"
#include "AssetProcessorFingerprint.h"
#include "AssetEnums.h"

namespace NovaEngine {

struct ProductAsset {
    ProductAsset(const std::filesystem::path& path, const FileFingerprint& print, const AssetType& fileType, const AssetProcessorFingerprint& aprint, const std::string& asettings):
        handle(path), fingerprint(print), type(fileType), processedBy(aprint), settingsUsed(asettings) {};
    ProductAsset(const std::filesystem::path& path, const FileFingerprint& print, const AssetType& fileType, const AssetProcessorFingerprint& aprint, const toml::value& asettings):
        handle(path), fingerprint(print), type(fileType), processedBy(aprint), settingsUsed(asettings) {};

    const std::filesystem::path handle;
    const FileFingerprint fingerprint;
    const AssetType type;
    const AssetProcessorFingerprint processedBy;
    const toml::value settingsUsed;
};

} // namespace NovaEngine