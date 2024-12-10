#include <spdlog/spdlog.h>

#include "include/AssetBuildJob.h"

#include "include/AssetProcessor.h"

namespace NovaEngine {

AssetBuildJob::AssetBuildJob(AssetProcessor* processor, std::vector<SourceAsset> sources, std::vector<std::vector<SourceAsset>> deps, toml::value settings): 
    _processor(processor), _sources(sources), _deps(deps), _settings(settings) {}

tl::expected<ProductAsset, AssetBuildError> AssetBuildJob::execute() {
    return _processor->processBuildJob(this);
}

} // namespace NovaEngine
