#pragma once

// #include <FileWatch.hpp>
#include <watcher.hpp>

#include <regex>
#include <filesystem>
#include <queue>

#include "include/AssetMetadata.h"

namespace NovaEngine {

class FolderScanner {
    public:
    explicit FolderScanner(const std::filesystem::path& folder);

    private:
    void fsCallback(wtr::event event);
    // void rememberOldName(const std::filesystem::path& path);

    void onRename(const std::filesystem::path& path, const std::filesystem::path& oldName);
    void onRemove(const std::filesystem::path& path);
    void onModify(const std::filesystem::path& path);
    void onAdd(const std::filesystem::path& path);

    std::filesystem::path _folder;
    // Regex below should capture all files but ones ending with a metadata extension (.meta as of writing)
    // For negative lookahead explanation: https://stackoverflow.com/questions/26685617
    std::regex _metaRegex = std::regex(R"(^(?!.*\.)" + META_EXTENSION + R"($).*$)", std::regex::icase | std::regex::ECMAScript);
    // std::queue<std::filesystem::path> _renamedOldNames;
};

} // namespace NovaEngine
