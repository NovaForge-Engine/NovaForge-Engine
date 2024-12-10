#pragma once

#include <filesystem>
#include <vector>
#include <optional>

#include "FileFingerprint.h"
#include "AssetEnums.h"

namespace NovaEngine {

enum class AssetError {
    SUCCESS = 0,
    UNKNOWN_ASSET = 1,
    INVALID_ASSET = 2,
    MISSING_ASSET = 3,
    EMPTY_ASSET = 4
};

const std::filesystem::path NOVA_INVALID_PATH = "./con";

class IAssetTester {
    public:
    virtual bool setupFile(const std::filesystem::path& file) { _lastFile = file; return true; };
    virtual bool isAssetOfType() { return false; };
    virtual AssetError isAssetValid();

    [[nodiscard]] virtual std::vector<std::filesystem::path> getDependenciesPaths() const { return {}; };

    [[nodiscard]] virtual std::optional<std::vector<AssetType>> getStoredTypes() const {
        return (_lastFile == NOVA_INVALID_PATH) ? std::nullopt : std::optional(_possibleTypes);
    }

    [[nodiscard]] std::optional<FileFingerprint> getFingerprint() const {
        if (_lastFile == NOVA_INVALID_PATH) return std::nullopt;
        std::ifstream f(_lastFile, std::ios::binary);
        std::vector<unsigned char> hash(picosha2::k_digest_size);
        picosha2::hash256(f, hash.begin(), hash.end());
        return FileFingerprint(std::filesystem::last_write_time(_lastFile), hash);
    };

    [[nodiscard]] AssetFormat getFormat() const { return _format; };

    private:
    std::filesystem::path _lastFile = NOVA_INVALID_PATH;
    AssetFormat _format;
    std::vector<AssetType> _possibleTypes;
};

} // namespace NovaEngine
