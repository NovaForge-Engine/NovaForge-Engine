#pragma once

#include <fmt/format.h>

#include <string>
#include <regex>

namespace NovaEngine {

struct Version {
    Version(const int& imajor, const int& iminor, const int& ipatch):
        major(imajor), minor(iminor), patch(ipatch) {};
    Version(const int& imajor, const int& iminor):
        major(imajor), minor(iminor), patch(0) {};
    Version(const int& imajor):
        major(imajor), minor(0), patch(0) {};
    Version(const std::string& iprefix, const int& imajor, const int& iminor, const int& ipatch, const std::string& isuffix):
        prefix(iprefix), suffix(isuffix), major(imajor), minor(iminor), patch(ipatch) {};
    static Version from_string(const std::string& toparse) {
        // Pattern = "[$prefix]$major[.$minor][.$patch][$suffix]", where major, minor and patch are integers
        /* All of the following strings should successfully parse:
        "1.0.0"
        "v1.2.3"
        "version 2.4.38-RELEASE"
        "1.22 Java Edition"
        "iteration 7 (experimental)"
        "50"
        */
        std::regex version_parse(R"(^(\D*)(\d+)\.?(\d*)\.?(\d*)(\D*)$)");
        std::smatch results;
        bool match = std::regex_match(toparse, results, version_parse);
        if (!match) {
            throw std::runtime_error(fmt::format("Can't parse version string {:?}", toparse));
        }
        std::string prefix = results[1];
        std::string suffix = results[5];
        int major = std::stoi(results[2]);
        int minor = results[3] == "" ? 0 : std::stoi(results[3]);
        int patch = results[4] == "" ? 0 : std::stoi(results[4]);
        return { prefix, major, minor, patch, suffix };
    }
    const std::string prefix, suffix;
    const int major, minor = 0, patch = 0;

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

    [[nodiscard]] std::string fullString() const {
        return fmt::format("{}{}.{}.{}{}", prefix, major, minor, patch, suffix);
    }
};

struct AssetProcessorFingerprint {
    AssetProcessorFingerprint(const std::string& iname, const Version& iversion):
        name(iname), version(iversion) {};
    AssetProcessorFingerprint(const std::string& iname, const std::string& iversion):
        name(iname), version(Version::from_string(iversion)) {};
    const std::string name;
    const Version version;

    bool operator<(const AssetProcessorFingerprint& other) const {
        if (name != other.name) return name < other.name;
        return version < other.version;
    }

    bool operator==(const AssetProcessorFingerprint& other) const {
        if (name != other.name) return false;
        return version == other.version;
    }
};

} // namespace NovaEngine