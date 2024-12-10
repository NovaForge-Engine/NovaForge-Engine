#pragma once

#include <FileWatch.hpp>

#include <filesystem>
#include <queue>

namespace NovaEngine {

using FileWatchFilesystem = filewatch::FileWatch<std::filesystem::path>;

class FolderScanner {
    public:
    explicit FolderScanner(const std::filesystem::path& folder);

    private:
    void majorCallback(const std::filesystem::path& path, filewatch::Event type);
    void rememberOldName(const std::filesystem::path& path);

    void onRename(const std::filesystem::path& path);
    void onRemove(const std::filesystem::path& path);
    void onModify(const std::filesystem::path& path);
    void onAdd(const std::filesystem::path& path);

    std::filesystem::path _folder;
    FileWatchFilesystem _majorCallback;
    std::queue<std::filesystem::path> _renamedOldNames;
};

} // namespace NovaEngine
