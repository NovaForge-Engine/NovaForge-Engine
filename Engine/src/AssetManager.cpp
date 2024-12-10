#include "include/AssetCache.h"
#include "include/AssetProcessor.h"
#include "include/AssetMetadata.h"

#include "include/AssetManager.h"

namespace NovaEngine {

bool AssetManager::addFile(const std::filesystem::path& file) {
    // Sanity check: see if we know this file
    if (AssetDatabase::instance().isInDatabase(file)) {
        updateFile(file);
    }
    // Check if we know how to process this format
    AssetFormat format = AssetFormat::UNKNOWN;
    AssetError err = AssetError::UNKNOWN_ASSET;
    IAssetTester *tester;
    for (auto & _tester : _testers) {
        tester = _tester;
        tester->setupFile(file);
        if (!tester->isAssetOfType()) continue;
        format = tester->getFormat();
        err = tester->isAssetValid();
        break;
    }
    // If we don't, then add an unknown entry
    if (format == AssetFormat::UNKNOWN || err != AssetError::SUCCESS) {
        AssetDatabase::instance().addUnknown(file, static_cast<int>(err), std::nullopt);
        return false;
    }
    auto types = tester->getStoredTypes().value();
    FileFingerprint fingerprint = tester->getFingerprint().value();
    // Create a SourceAsset
    auto newAsset = SourceAsset(file, format, fingerprint, types);
    AssetDatabase::instance().addNewSource(newAsset);
    // Try to get dependencies
    auto depPaths = tester->getDependenciesPaths();
    bool hasMissingAssets = false;
    std::vector<SourceAsset> deps;
    for (auto &depPath: depPaths) {
        if (!AssetDatabase::instance().isInDatabase(depPath)) {
            hasMissingAssets = true;
            AssetDatabase::instance().addNewUnknown(
                depPath, 
                FileFingerprint(std::filesystem::file_time_type(), {}), 
                static_cast<int>(AssetError::MISSING_ASSET), std::nullopt);
        } else {
            deps.push_back(AssetDatabase::instance().getSource(depPath));
        }
        AssetDatabase::instance().addDependency(file, depPath);
    }
    // If all deps are met, find a processor for the new asset
    if (!hasMissingAssets) return false;
    AssetProcessor* selectedProcessor;
    for (auto processor: _processors) {
        if (processor->canProcessAsset(newAsset)) {
            selectedProcessor = processor;
            break;
        }
    }
    // Pass a new asset into the processor with default params
    auto putResult = selectedProcessor->processAsset(newAsset, deps, std::nullopt);
    // If there is a new build job, schedule it for processing
    if (putResult.has_value()) scheduleRegularJob(putResult.value());
    return true;
}

void AssetManager::removeFile(const std::filesystem::path& file) {
    auto products = AssetDatabase::instance().getProductsFromSource(file);
    for (auto &product: products) {
        AssetDatabase::instance().removeFile(product.handle);
        std::filesystem::remove(file);
        std::filesystem::remove(std::filesystem::path(file.string() + META_SUBSTR));
    }
    AssetDatabase::instance().removeFile(file);
}

void AssetManager::rescheduleAsset(const SourceAsset& asset) {
    //
}

void AssetManager::updateFile(const std::filesystem::path& file) {
    // Sanity check: see if DB has file info
    if (!AssetDatabase::instance().isInDatabase(file)) {
        addFile(file);
        return;
    }
    auto asset = AssetDatabase::instance().getSource(file);
    // Find suitable asset tester
    IAssetTester *tester = nullptr;
    for (auto & _tester : _testers) {
        if (_tester->getFormat() == asset.format) {
            tester = _tester;
            break;
        }
    }
    if (tester == nullptr) throw std::runtime_error(fmt::format("Can't find asset tester for known file {:?}", file.string()));
    tester->setupFile(file);
    FileFingerprint curPrint = tester->getFingerprint().value();
    AssetDatabase::instance().updateFingerprint(file, curPrint);
    // See if the file is still well-formed
    if (!tester->isAssetOfType()) {
        // For now, it's just easier to simulate an "add". Probably must be done better.
        AssetDatabase::instance().removeFile(file);
        addFile(file);
        return;
    }
    // Get out if gile has no changes
    if (curPrint == asset.fingerprint) return;
    // Cancel any pending build jobs for an outdated SourceAsset
    for (auto &processor: _processors) processor->cancelAssetProcessing(asset);
    // Schedule rebuild for all assets that depend on changed file
    auto dependants = AssetDatabase::instance().getProductsFromSource(file);
    for (auto& product: dependants) {
        auto sources = AssetDatabase::instance().getSourcesFromProduct(product.handle);
        std::vector<std::vector<SourceAsset>> deps;
        deps.reserve(sources.size());
        for (auto &source: sources) {
            deps.push_back(AssetDatabase::instance().getAssetDependencies(source.handle));
        }
        AssetProcessor* processor = *std::find_if(_processors.begin(), _processors.end(), [product](auto &processor) {return processor->fingerprint == product.processedBy;});
        auto job = processor->processAsset(sources, deps, std::nullopt);
        if (job.has_value()) scheduleRegularJob(job.value());
    }
}

void AssetManager::renameFile(const std::filesystem::path& oldPath, const std::filesystem::path& newPath) {
    AssetDatabase::instance().updateName(oldPath, newPath);
}

std::vector<tl::expected<ProductAsset, AssetBuildError>> AssetManager::executeCriticalJobs() {
    std::vector<tl::expected<ProductAsset, AssetBuildError>> result;
    while (!_critQueue.empty()) {
        auto job = _critQueue.back();
        auto deps = job->_sources;
        auto buildResult = job->_processor->processBuildJob(job);
        if (buildResult.has_value()) {
            auto product = buildResult.value();
            if (AssetDatabase::instance().isKnownProductFile(product.handle)) {
                AssetDatabase::instance().updateFingerprint(product.handle, product.fingerprint);
                AssetDatabase::instance().updateProduct(product.handle, product.processedBy, toml::format(product.settingsUsed));
            } else {
                AssetDatabase::instance().addNewProduct(product);
            }
        }
        _critQueue.pop_back();
    }
    return result;
}

std::vector<tl::expected<ProductAsset, AssetBuildError>> AssetManager::executeQueuedJobs() {
    std::vector<tl::expected<ProductAsset, AssetBuildError>> result;
    while (!_regularQueue.empty()) {
        auto job = _regularQueue.back();
        auto deps = job->_sources;
        auto buildResult = job->_processor->processBuildJob(job);
        if (buildResult.has_value()) {
            auto product = buildResult.value();
            if (AssetDatabase::instance().isKnownProductFile(product.handle)) {
                AssetDatabase::instance().updateFingerprint(product.handle, product.fingerprint);
                AssetDatabase::instance().updateProduct(product.handle, product.processedBy, toml::format(product.settingsUsed));
            } else {
                AssetDatabase::instance().addNewProduct(product);
            }
        }
        _regularQueue.pop_back();
    }
    return result;
}

} // namespace NovaEngine