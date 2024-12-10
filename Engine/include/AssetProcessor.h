#pragma once

#include <optional>

#include <toml.hpp>
#include <tl/expected.hpp>
#include <utility>

#include "AssetBuildJob.h"
#include "AssetProcessorFingerprint.h"
#include "SourceAsset.h"
#include "ProductAsset.h"

namespace NovaEngine {

struct AssetBuildError {
    int code;
    std::string message;
};

class AssetProcessor {
    public:
    const AssetProcessorFingerprint fingerprint;

    virtual bool canProcessAsset(SourceAsset source);
    virtual tl::expected<ProductAsset, AssetBuildError> processBuildJob(const AssetBuildJob *job);
    virtual std::optional<const AssetBuildJob*> processAsset(std::vector<SourceAsset> asset, std::vector<std::vector<SourceAsset>> deps, std::optional<toml::value> settings);
    std::optional<const AssetBuildJob*> processAsset(SourceAsset asset, std::vector<SourceAsset> deps, std::optional<toml::value> settings) { return processAsset(std::vector<SourceAsset>{std::move(asset)}, std::vector<std::vector<SourceAsset>>{std::move(deps)}, std::move(settings)); };
    virtual void cancelAssetProcessing(const SourceAsset& asset) {
        auto leftover = std::remove_if(_jobs.begin(), _jobs.end(), [asset](auto &job) {return std::find(job._sources.begin(), job._sources.end(), asset) != job._sources.end();});
        _jobs.erase(leftover, _jobs.end());
    };

    private:
    std::vector<AssetBuildJob> _jobs;
};

} // namespace NovaEngine