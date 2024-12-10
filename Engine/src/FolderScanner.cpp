#include "include/AssetManager.h"
#include "include/AssetCache.h"
#include "include/AssetMetadata.h"

#include "include/FolderScanner.h"

namespace NovaEngine {

FolderScanner::FolderScanner(const std::filesystem::path& folder) :
    _folder(folder),
    // Regex below should capture all files but ones ending with a metadata extension (.meta as of writing)
    // For negative lookahead explanation: https://stackoverflow.com/questions/26685617
    _majorCallback(_folder, std::regex(R"(^(?!.*\.)" + META_EXTENSION + R"($).*$)", std::regex::icase | std::regex::ECMAScript), [this](const std::filesystem::path& path, const filewatch::Event type){ majorCallback(path, type); })
{}

void FolderScanner::majorCallback(const std::filesystem::path& path, const filewatch::Event type) {
    switch (type) {
        case filewatch::Event::added:
            onAdd(path);
            break;
        case filewatch::Event::removed:
            onRemove(path);
            break;
        case filewatch::Event::modified:
            onModify(path);
            break;
        case filewatch::Event::renamed_old:
            rememberOldName(path);
            break;
        case filewatch::Event::renamed_new:
            onRemove(path);
            break;
    }
}

void FolderScanner::rememberOldName(const std::filesystem::path& path) {
    _renamedOldNames.push(path);
}

void FolderScanner::onRename(const std::filesystem::path& path) {
    const std::filesystem::path oldName = _renamedOldNames.front();
    _renamedOldNames.pop();
    // TODO: Preserve previous build artifacts instead of rebuilding
    AssetManager::instance().removeFile(oldName);
    AssetManager::instance().addFile(path);
}

void FolderScanner::onModify(const std::filesystem::path& path) {
    if (AssetDatabase::instance().isUnknownFile(path)) {
        // Try and add file again - maybe it became valid this time
        AssetManager::instance().addFile(path);
        return;
    }
    if (AssetManager::instance().needsRebuild(path)) {
        AssetManager::instance().rebuildFile(path);
    }
}

void FolderScanner::onAdd(const std::filesystem::path& path) {
    AssetManager::instance().addFile(path);
}

void FolderScanner::onRemove(const std::filesystem::path& path) {
    AssetManager::instance().removeFile(path);
}

} // namespace NovaEngine
