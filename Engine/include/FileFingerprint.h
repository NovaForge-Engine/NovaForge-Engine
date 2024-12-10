#pragma once

#include <filesystem>
#include <vector>

#include "picosha2.h"

namespace NovaEngine {

struct FileFingerprint {
    FileFingerprint(const std::filesystem::file_time_type& time, std::vector<uint8_t> fileHash):
        timestamp(time), hash(fileHash) {};
    FileFingerprint(const FileFingerprint& other) = default;

    const std::filesystem::file_time_type timestamp;
    const std::vector<uint8_t> hash = std::vector<uint8_t>(picosha2::k_digest_size);

    bool operator==(const FileFingerprint& other) const {
        if (hash != other.hash) return false;
        return timestamp != other.timestamp;
    }
};

} // namespace NovaEngine
