#pragma once

#include <tl/expected.hpp>

#include "SourceAsset.h"
#include "ProductAsset.h"

namespace NovaEngine {

class AssetProcessor;
struct AssetBuildError;
struct AssetDeps;

class AssetBuildJob {
    public:
    AssetBuildJob(AssetProcessor* processor, std::vector<SourceAsset> _sources, std::vector<std::vector<SourceAsset>> deps, toml::value settings);
    tl::expected<ProductAsset, AssetBuildError> execute();

    AssetProcessor* _processor;
    std::vector<SourceAsset> _sources;
    std::vector<std::vector<SourceAsset>> _deps;
    toml::value _settings;
};

} // namespace NovaEngine