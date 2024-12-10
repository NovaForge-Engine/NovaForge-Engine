#pragma once

#include <filesystem>
#include <vector>

#include "AssetEnums.h"
#include "FileFingerprint.h"

namespace NovaEngine {

struct SourceAsset {
    SourceAsset(const std::filesystem::path& path, const AssetFormat& type, const FileFingerprint& print, std::vector<AssetType> datatypes):
        handle(path), format(type), includedTypes(datatypes), fingerprint(print) {};
    [[nodiscard]] bool hasAssetType(AssetType type) const { return std::find(includedTypes.begin(), includedTypes.end(), type) != includedTypes.end(); };

    const std::filesystem::path handle;
    const AssetFormat format;
    const std::vector<AssetType> includedTypes;
    const FileFingerprint fingerprint;

    bool operator==(const SourceAsset& other) {
        if (handle != other.handle) return false;
        if (fingerprint != other.fingerprint) return false;
        return true;
    }

    bool operator!=(const SourceAsset& other) {
        return !(*this == other);
    }
};

} // namespace NovaEngine
