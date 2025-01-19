#include "include/AssetManager.h"

#include "include/FolderScanner.h"

namespace NovaEngine {

FolderScanner::FolderScanner(const std::filesystem::path& folder) :
    _folder(folder)
{}

// void FolderScanner::majorCallback(const std::filesystem::path& path, const filewatch::Event type) {
//     switch (type) {
//         case filewatch::Event::added:
//             onAdd(path);
//             break;
//         case filewatch::Event::removed:
//             onRemove(path);
//             break;
//         case filewatch::Event::modified:
//             onModify(path);
//             break;
//         case filewatch::Event::renamed_old:
//             rememberOldName(path);
//             break;
//         case filewatch::Event::renamed_new:
//             onRemove(path);
//             break;
//     }
// }

// void FolderScanner::rememberOldName(const std::filesystem::path& path) {
//     _renamedOldNames.push(path);
// }

void FolderScanner::fsCallback(wtr::event event) {
    switch (event.effect_type) {
        case wtr::event::effect_type::create:
            onAdd(event.path_name);
            break;
        case wtr::event::effect_type::destroy:
            onRemove(event.path_name);
            break;
        case wtr::event::effect_type::modify:
            onModify(event.path_name);
            break;
        case wtr::event::effect_type::rename:
            onRename(event.associated->path_name, event.path_name);
            break;
        case wtr::event::effect_type::owner:
        case wtr::event::effect_type::other:
        default:
            break;
    }
}

void FolderScanner::onRename(const std::filesystem::path& path, const std::filesystem::path& oldName) {
    // TODO: Preserve previous build artifacts instead of rebuilding (?)
    AssetManager::instance().renameFile(oldName, path);
}

void FolderScanner::onModify(const std::filesystem::path& path) {
    AssetManager::instance().updateFile(path);
}

void FolderScanner::onAdd(const std::filesystem::path& path) {
    AssetManager::instance().addFile(path);
}

void FolderScanner::onRemove(const std::filesystem::path& path) {
    AssetManager::instance().removeFile(path);
}

} // namespace NovaEngine
