#pragma once

#ifndef NOVA_ASSET_MANAGER_H
#define NOVA_ASSET_MANAGER_H

#include <filesystem>

#include "include/Export.h"
#include "include/AssetBuildJob.h"
#include "include/IAssetTester.h"

#include "include/Singleton.h"

namespace NovaEngine {

constexpr std::string ASSET_FOLDER = "Assets";
constexpr std::string PRODUCT_FOLDER = "PreppedAssets";

class ENGINE_DLL AssetManager: public Singleton<AssetManager> {
    public:
    bool addFile(const std::filesystem::path& file);
    void renameFile(const std::filesystem::path& oldPath, const std::filesystem::path& newPath);
    void updateFile(const std::filesystem::path& file);
    bool needsRebuild(const ProductAsset& file);
    void removeFile(const std::filesystem::path& file);

    void scheduleRegularJob(const AssetBuildJob* job);
    void scheduleCriticalJob(const AssetBuildJob* job);
    std::vector<tl::expected<ProductAsset, AssetBuildError>> executeCriticalJobs();
    std::vector<tl::expected<ProductAsset, AssetBuildError>> executeQueuedJobs();

    private:
    std::vector<IAssetTester*> _testers;
    std::vector<AssetProcessor*> _processors;

    std::vector<const AssetBuildJob*> _critQueue;
    std::vector<const AssetBuildJob*> _regularQueue;

    void rescheduleAsset(const SourceAsset& asset);
    // std::vector<const AssetBuildJob*> _active;
    // const int activeJobsLimit = 8;
};

} // namespace NovaEngine

#endif // NOVA_ASSET_MANAGER_H