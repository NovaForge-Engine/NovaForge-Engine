#pragma once

#include <string>

struct Version {
    std::string prefix, suffix;
    int major, minor, patch;

    bool operator<(const Version& other) const {
        if (major != other.major) return major < other.major;
        if (minor != other.minor) return minor < other.minor;
        if (patch != other.patch) return patch < other.patch;
        if (prefix != other.prefix) return prefix < other.prefix;
        return suffix < other.suffix;
    };

    bool operator==(const Version& other) const {
        if (major != other.major) return false;
        if (minor != other.minor) return false;
        if (patch != other.patch) return false;
        if (prefix != other.prefix) return false;
        return suffix == other.suffix;
    };
};

struct AssetProcessorFingerprint {
    std::string name;
    Version version;
};
