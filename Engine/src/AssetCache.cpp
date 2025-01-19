#include <fmt/format.h>

#include <algorithm>

#include "include/AssetDatabaseMigration.h"

#include "include/AssetCache.h"

namespace NovaEngine {

AssetDatabase::AssetDatabase() {
    { // AssetDatabaseMigration has its own DB connection within this scope
        AssetDatabaseMigration migrate;
        migrate.checkMigrations();
    }
    int openResult = sqlite3_open(fmt::format("./{}", DB_FILE_NAME).c_str(), &_assetDB);
    if (openResult != SQLITE_OK) {
        throw std::runtime_error(fmt::format("Couldn't load asset DB for migration purposes: error #{}", openResult));
    }
    prepLongtermStmt(_atq.c_str(), &_assetTypesQuery);
    prepLongtermStmt(_ati.c_str(), &_assetTypesInsert);
    prepLongtermStmt(_atr.c_str(), &_assetTypesRemove);
    prepLongtermStmt(_fps.c_str(), &_filePathSelect);
    prepLongtermStmt(_ffs.c_str(), &_fileFingerprintSelect);
    prepLongtermStmt(_sps.c_str(), &_sourcePathSelect);
    prepLongtermStmt(_sfs.c_str(), &_sourceFingerprintSelect);
    prepLongtermStmt(_pps.c_str(), &_productPathSelect);
    prepLongtermStmt(_pfs.c_str(), &_productFingerprintSelect);
    prepLongtermStmt(_ups.c_str(), &_unknownPathSelect);
    prepLongtermStmt(_ufs.c_str(), &_unknownFingerprintSelect);
    prepLongtermStmt(_psq.c_str(), &_productSourcesQuery);
    prepLongtermStmt(_dq.c_str(), &_depsQuery);
    prepLongtermStmt(_fi.c_str(), &_fileInsert);
    prepLongtermStmt(_si.c_str(), &_sourceInsert);
    prepLongtermStmt(_pi.c_str(), &_productInsert);
    prepLongtermStmt(_ui.c_str(), &_unknownInsert);
    prepLongtermStmt(_di.c_str(), &_depInsert);
    prepLongtermStmt(_psi.c_str(), &_productSourcesInsert);
    prepLongtermStmt(_fr.c_str(), &_fileRemove);
    prepLongtermStmt(_sr.c_str(), &_sourceRemove);
    prepLongtermStmt(_pr.c_str(), &_productRemove);
    prepLongtermStmt(_ur.c_str(), &_unknownRemove);
    prepLongtermStmt(_dr.c_str(), &_depRemove);
    prepLongtermStmt(_psr.c_str(), &_productSourcesRemove);
    prepLongtermStmt(_uf.c_str(), &_updateFingerprint);
    prepLongtermStmt(_up.c_str(), &_updateProduct);
    prepLongtermStmt(_un.c_str(), &_updateName);
    prepLongtermStmt(_qkd.c_str(), &_queryKnownDeps);
    prepLongtermStmt(_ppq.c_str(), &_productProductQuery);
    
    // Run DB setup
    sqlite3_stmt *stmt;
    const char* tail = DB_SETUP_COMMANDS.c_str();
    const char * zSql = DB_SETUP_COMMANDS.c_str();
    do {
        int prepResult = sqlite3_prepare_v2(_assetDB, zSql, -1, &stmt, &tail);
        if (prepResult != SQLITE_OK) {
            throw std::runtime_error(fmt::format("Unable to prep statement: error #{}\nStatement: {:?}", prepResult, DB_SETUP_COMMANDS));
        }
        int stepResult = SQLITE_ROW;
        do {
            stepResult = sqlite3_step(stmt);
        } while (stepResult == SQLITE_ROW);
        if (stepResult != SQLITE_DONE) {
            throwStmt(stmt, stepResult);
        }
        zSql = tail; 
        sqlite3_finalize(stmt);
    } while (*zSql != '\0');
}

void AssetDatabase::throwStmt(sqlite3_stmt *stmt, const int& error) const {
    // Pointer from sqlite3_expanded_sql has to be freed manually, so we output error then throw
    const char *fullSQL = sqlite3_expanded_sql(stmt);
    const char *errmsg = sqlite3_errmsg(_assetDB);
    fmt::println("Error #{0} while stepping through the statement.\nError description:\n{3}\nRaw:\n{1}\nExpanded:\n{2}", error, sqlite3_sql(stmt), fullSQL, errmsg);
    sqlite3_free((void*)fullSQL);
    throw std::runtime_error(fmt::format("SQLite statement execution error #{}", error));
}

void AssetDatabase::prepStmt(const char* zSql, sqlite3_stmt** stmt) {
    const char* tail;
    int prepResult = sqlite3_prepare_v2(_assetDB, zSql, -1, stmt, &tail);
    if (prepResult != SQLITE_OK) {
        throw std::runtime_error(fmt::format("Can't prepare statement: error #{}\nStatement: {:?}", prepResult, zSql));
    }
}

void AssetDatabase::prepLongtermStmt(const char *zSql, sqlite3_stmt **stmt) {
    const char* tail;
    int prepResult = sqlite3_prepare_v3(_assetDB, zSql, -1, SQLITE_PREPARE_PERSISTENT, stmt, &tail);
    if (prepResult != SQLITE_OK) {
        throw std::runtime_error(fmt::format("Can't prepare statement: error #{}\nStatement: {:?}", prepResult, zSql));
    }
}

void AssetDatabase::bindPathHash(sqlite3_stmt *stmt, const char *name, const std::filesystem::path& path) const {
    std::vector<unsigned char> pathHash(picosha2::k_digest_size); 
    auto pathStr = path.string();
    picosha2::hash256(pathStr.begin(), pathStr.end(), pathHash.begin(), pathHash.end());
    bindBlob(stmt, name, (void*)pathHash.data(), static_cast<int>(pathHash.size()));
}

void AssetDatabase::bindBlob(sqlite3_stmt *stmt, const char *name, const void *ptr, const int& size) const {
    int bind = sqlite3_bind_parameter_index(stmt, name);
    int blobResult = sqlite3_bind_blob(stmt, bind, ptr, size, SQLITE_STATIC);
    if (blobResult != SQLITE_OK) {
        throw std::runtime_error(fmt::format(
            "Error #{} while binding blob\nError message: {:?}\nStatement: {:?}", 
            blobResult, sqlite3_errmsg(_assetDB), sqlite3_sql(stmt)));
    }
}

bool AssetDatabase::isKnownProductFile(const std::filesystem::path& file) {
    bindPathHash(_productPathSelect, "PATH", file);
    int stepResult = sqlite3_step(_productPathSelect);
    sqlite3_reset(_productPathSelect);
    switch (stepResult) {
        case SQLITE_ROW: return true;
        case SQLITE_DONE: return false;
        default: throwStmt(_productPathSelect, stepResult); return false;
    }
}

bool AssetDatabase::isUnknownFile(const std::filesystem::path& file) {
    bindPathHash(_unknownPathSelect, "PATH", file);
    int stepResult = sqlite3_step(_unknownPathSelect);
    sqlite3_reset(_unknownPathSelect);
    switch (stepResult) {
        case SQLITE_ROW: return true;
        case SQLITE_DONE: return false;
        default: throwStmt(_unknownPathSelect, stepResult); return false;
    }
}

bool AssetDatabase::isKnownSourceFile(const std::filesystem::path& file) {
    bindPathHash(_sourcePathSelect, "PATH", file);
    int stepResult = sqlite3_step(_sourcePathSelect);
    sqlite3_reset(_sourcePathSelect);
    switch (stepResult) {
        case SQLITE_ROW: return true;
        case SQLITE_DONE: return false;
        default: throwStmt(_sourcePathSelect, stepResult); return false;
    }
}

int AssetDatabase::findColumn(sqlite3_stmt *stmt, const char *colName) {
    for (int i = 0; i < sqlite3_column_count(stmt); i++) {
        if (strcmp(sqlite3_column_name(stmt, i), colName) == 0) {
            return i;
        }
    }
    return -1;
}

std::vector<AssetType> AssetDatabase::getAssetTypes(const std::filesystem::path& file) const {
    bindPathHash(_assetTypesQuery, "PATH", file);

    std::vector<AssetType> result;
    int stepResult = SQLITE_ROW;
    stepResult = sqlite3_step(_assetTypesQuery);
    if ((stepResult != SQLITE_ROW) && (stepResult != SQLITE_DONE)) {
        throwStmt(_assetTypesQuery, stepResult);
    }
    int column = findColumn(_assetTypesQuery, "type");
    while (stepResult == SQLITE_ROW) {
        long long type = sqlite3_column_int64(_assetTypesQuery, column);
        // maaxxaam @ 07.01.25
        // TODO: add check if enum conversion is invalid
        result.push_back(static_cast<AssetType>(type));
        stepResult = sqlite3_step(_assetTypesQuery);
        if ((stepResult != SQLITE_ROW) && (stepResult != SQLITE_DONE)) {
            throwStmt(_assetTypesQuery, stepResult);
        }
    };
    sqlite3_reset(_assetTypesQuery);
    return result;
}

ProductAsset AssetDatabase::getProductFromFingerprint(const FileFingerprint& fingerprint) const {
    bindFingerprint(_productFingerprintSelect, fingerprint);
    int stepResult = sqlite3_step(_productFingerprintSelect);
    if ((stepResult != SQLITE_ROW) && (stepResult != SQLITE_DONE)) {
        throwStmt(_productFingerprintSelect, stepResult);
    }
    if (stepResult != SQLITE_ROW) {
        // Since we already checked for errors, no row means we don't have the asset
        // Make a dummy then... 
        return { 
            "./con", 
            FileFingerprint(std::filesystem::file_time_type(), {}), 
            AssetType::CONFIG, 
            AssetProcessorFingerprint("con", Version(0)), 
            std::string("con")
        };
    }
    AssetProcessorFingerprint print{
        getColumn<std::string>(_productFingerprintSelect, "processedBy"),
        getColumn<std::string>(_productFingerprintSelect, "processedVersion")
    };
    std::string settings = getColumn<std::string>(_productFingerprintSelect, "settings");
    std::string path = getColumn<std::string>(_sourceFingerprintSelect, "path");
    sqlite3_reset(_productFingerprintSelect);
    return {
        path,
        getFingerprint(path),
        getAssetTypes(path)[0],
        print,
        settings
    };
}

ProductAsset AssetDatabase::getProduct(const std::filesystem::path& file) const {
    bindPathHash(_productPathSelect, "PATH", file);
    int stepResult = sqlite3_step(_productPathSelect);
    if ((stepResult != SQLITE_ROW) && (stepResult != SQLITE_DONE)) {
        throwStmt(_productPathSelect, stepResult);
    }
    if (stepResult != SQLITE_ROW) {
        // Since we already checked for errors, no row means we don't have the asset
        // Make a dummy then... 
        return { 
            "./con", 
            FileFingerprint(std::filesystem::file_time_type(), {}), 
            AssetType::CONFIG, 
            AssetProcessorFingerprint("con", Version(0)), 
            std::string("con")
        };
    }
    AssetProcessorFingerprint print{
        getColumn<std::string>(_productPathSelect, "processedBy"),
        getColumn<std::string>(_productPathSelect, "processedVersion")
    };
    std::string settings = getColumn<std::string>(_productPathSelect, "settings");
    sqlite3_reset(_productPathSelect);
    return {
        file,
        getFingerprint(file),
        getAssetTypes(file)[0],
        print,
        settings
    };
}

bool AssetDatabase::isInDatabase(const std::filesystem::path& file) {
    bindPathHash(_filePathSelect, "PATH", file);
    int stepResult = sqlite3_step(_filePathSelect);
    if ((stepResult != SQLITE_ROW) && (stepResult != SQLITE_DONE)) {
        throwStmt(_filePathSelect, stepResult);
    };
    sqlite3_reset(_filePathSelect);
    return stepResult == SQLITE_ROW;
}

FileFingerprint AssetDatabase::getFingerprint(const std::filesystem::path& file) const {
    bindPathHash(_filePathSelect, "PATH", file);
    int stepResult = sqlite3_step(_filePathSelect);
    if ((stepResult != SQLITE_ROW) && (stepResult != SQLITE_DONE)) {
        throwStmt(_filePathSelect, stepResult);
    };
    if (stepResult != SQLITE_ROW) {
        throw std::runtime_error(fmt::format("No file data found for path {:?}", file.string()));
    };
    auto time = getColumn<std::filesystem::file_time_type>(_filePathSelect, "modified");
    auto hash = getColumn<std::vector<uint8_t>>(_filePathSelect, "fileHash");
    sqlite3_reset(_filePathSelect);
    return {
        time,
        hash
    };
};

SourceAsset AssetDatabase::getSourceFromFingerprint(const FileFingerprint& fingerprint) const {
    bindFingerprint(_sourceFingerprintSelect, fingerprint);
    int stepResult = sqlite3_step(_sourceFingerprintSelect);
    if ((stepResult != SQLITE_ROW) && (stepResult != SQLITE_DONE)) {
        throwStmt(_sourceFingerprintSelect, stepResult);
    }
    if (stepResult != SQLITE_ROW) {
        // Since we already checked for errors, no row means we don't have the asset
        // Make a dummy then... 
        return {"./con", AssetFormat::USD, FileFingerprint(std::filesystem::file_time_type(), {}), {}};
    }
    auto format = static_cast<AssetFormat>(getColumn<long long>(_sourceFingerprintSelect, "format"));
    std::string path = getColumn<std::string>(_sourceFingerprintSelect, "path");
    sqlite3_reset(_sourcePathSelect);
    return {
        path,
        format,
        fingerprint,
        getAssetTypes(path)
    };
}

SourceAsset AssetDatabase::getSource(const std::filesystem::path& file) const {
    bindPathHash(_sourcePathSelect, "PATH", file);
    int stepResult = sqlite3_step(_sourcePathSelect);
    if ((stepResult != SQLITE_ROW) && (stepResult != SQLITE_DONE)) {
        throwStmt(_sourcePathSelect, stepResult);
    }
    if (stepResult != SQLITE_ROW) {
        // Since we already checked for errors, no row means we don't have the asset
        // Make a dummy then... 
        return {"./con", AssetFormat::USD, FileFingerprint(std::filesystem::file_time_type(), {}), {}};
    }
    auto format = static_cast<AssetFormat>(getColumn<long long>(_sourcePathSelect, "format"));
    sqlite3_reset(_sourcePathSelect);
    return {
        file,
        format,
        getFingerprint(file),
        getAssetTypes(file)
    };
}

std::vector<SourceAsset> AssetDatabase::getAssetDependencies(const std::filesystem::path& file) const {
    bindPathHash(_depsQuery, "PATH", file);
    int stepResult = sqlite3_step(_depsQuery);
    if ((stepResult != SQLITE_ROW) && (stepResult != SQLITE_DONE)) {
        throwStmt(_depsQuery, stepResult);
    }
    if (stepResult != SQLITE_ROW) {
        // Since we already checked for errors, no row means we don't have the asset
        // Make a dummy then... 
        return {};
    }
    std::vector<SourceAsset> result;
    while (stepResult == SQLITE_ROW) {
        auto path = getColumn<std::string>(_depsQuery, "path");
        auto format = static_cast<AssetFormat>(getColumn<long long>(_depsQuery, "format"));
        auto modified = getColumn<std::filesystem::file_time_type>(_depsQuery, "modified");
        auto fileHash = getColumn<std::vector<uint8_t>>(_depsQuery, "fileHash");
        result.emplace_back(
            path,
            format,
            FileFingerprint(modified, fileHash),
            getAssetTypes(path)
        );
        stepResult = sqlite3_step(_depsQuery);
    };
    if ((stepResult != SQLITE_ROW) && (stepResult != SQLITE_DONE)) {
        throwStmt(_depsQuery, stepResult);
    }
    sqlite3_reset(_depsQuery);
    return result;
}

std::vector<SourceAsset> AssetDatabase::getSourcesFromProduct(const std::filesystem::path& file) const {
    bindPathHash(_productProductQuery, "PATH", file);
    int stepResult = sqlite3_step(_productProductQuery);
    if ((stepResult != SQLITE_ROW) && (stepResult != SQLITE_DONE)) {
        throwStmt(_productProductQuery, stepResult);
    }
    if (stepResult != SQLITE_ROW) {
        // Since we already checked for errors, no row means we don't have the asset
        // Make a dummy then... 
        return {};
    }
    std::vector<SourceAsset> result;
    while (stepResult == SQLITE_ROW) {
        auto path = getColumn<std::string>(_productProductQuery, "path");
        auto modified = getColumn<std::filesystem::file_time_type>(_productProductQuery, "modified");
        auto fileHash = getColumn<std::vector<uint8_t>>(_productProductQuery, "fileHash");
        auto format = static_cast<AssetFormat>(getColumn<long long>(_productProductQuery, "format"));
        result.emplace_back(
            path,
            format,
            FileFingerprint(modified, fileHash),
            getAssetTypes(path)
        );
        stepResult = sqlite3_step(_productProductQuery);
    };
    if ((stepResult != SQLITE_ROW) && (stepResult != SQLITE_DONE)) {
        throwStmt(_productProductQuery, stepResult);
    }
    sqlite3_reset(_productProductQuery);
    return result;
}

std::vector<ProductAsset> AssetDatabase::getProductsFromSource(const std::filesystem::path& file) const {
    bindPathHash(_productSourcesQuery, "PATH", file);
    int stepResult = sqlite3_step(_productSourcesQuery);
    if ((stepResult != SQLITE_ROW) && (stepResult != SQLITE_DONE)) {
        throwStmt(_productSourcesQuery, stepResult);
    }
    if (stepResult != SQLITE_ROW) {
        // Since we already checked for errors, no row means we don't have the asset
        // Make a dummy then... 
        return {};
    }
    std::vector<ProductAsset> result;
    while (stepResult == SQLITE_ROW) {
        auto path = getColumn<std::string>(_productSourcesQuery, "path");
        auto modified = getColumn<std::filesystem::file_time_type>(_productSourcesQuery, "modified");
        auto fileHash = getColumn<std::vector<uint8_t>>(_productSourcesQuery, "fileHash");
        auto printBy = getColumn<std::string>(_productSourcesQuery, "processedBy");
        auto printVersion = getColumn<std::string>(_productSourcesQuery, "processedVersion");
        auto settings = getColumn<std::string>(_productSourcesQuery, "settings");
        result.emplace_back(
            path,
            FileFingerprint(modified, fileHash),
            getAssetTypes(path)[0],
            AssetProcessorFingerprint(printBy, printVersion),
            settings
        );
        stepResult = sqlite3_step(_productSourcesQuery);
    };
    if ((stepResult != SQLITE_ROW) && (stepResult != SQLITE_DONE)) {
        throwStmt(_productSourcesQuery, stepResult);
    }
    sqlite3_reset(_productSourcesQuery);
    return result;
}

void AssetDatabase::bindTxt(sqlite3_stmt *stmt, const char* name, const char* value) const {
    int bind = sqlite3_bind_parameter_index(stmt, name);
    int bindResult = sqlite3_bind_text(stmt, bind, value, -1, nullptr);
    if (bindResult != SQLITE_OK) {
        throw std::runtime_error(fmt::format(
            "Error #{:d} when binding text to field #{:d}\nStatement: {:?}\nError message: {:?}",
            bindResult, bind, sqlite3_sql(stmt), sqlite3_errmsg(_assetDB)
        ));
    }
}

void AssetDatabase::bindFingerprint(sqlite3_stmt *stmt, const FileFingerprint& fingerprint) const {
    int bind = sqlite3_bind_parameter_index(stmt, "FILE");
    int blobResult = sqlite3_bind_blob(stmt, bind, fingerprint.hash.data(), static_cast<int>(fingerprint.hash.size()), SQLITE_STATIC);
    if (blobResult != SQLITE_OK) {
        throw std::runtime_error(fmt::format(
            "Error #{} while binding file path blob\nError message: {:?}\nStatement: {:?}", 
            blobResult, sqlite3_errmsg(_assetDB), sqlite3_sql(stmt)));
    }
    auto time = std::chrono::system_clock::to_time_t(std::chrono::file_clock::to_sys(fingerprint.timestamp));
    char* timeStr; std::strftime(timeStr, 19, "%FT%T", std::gmtime(&time));
    bindTxt(stmt, "TIME", timeStr);
}

bool AssetDatabase::isKnownProductFileFingerprint(const FileFingerprint& fingerprint) {
    bindFingerprint(_productFingerprintSelect, fingerprint);
    int stepResult = sqlite3_step(_productFingerprintSelect);
    sqlite3_reset(_productFingerprintSelect);
    switch (stepResult) {
        case SQLITE_ROW: return true;
        case SQLITE_DONE: return false;
        default: throwStmt(_productFingerprintSelect, stepResult); return false;
    }
}

bool AssetDatabase::isUnknownFileFingerprint(const FileFingerprint& fingerprint) {
    bindFingerprint(_unknownFingerprintSelect, fingerprint);
    int stepResult = sqlite3_step(_unknownFingerprintSelect);
    sqlite3_reset(_unknownFingerprintSelect);
    switch (stepResult) {
        case SQLITE_ROW: return true;
        case SQLITE_DONE: return false;
        default: throwStmt(_unknownFingerprintSelect, stepResult); return false;
    }
}

bool AssetDatabase::isKnownSourceFileFingerprint(const FileFingerprint& fingerprint) {
    bindFingerprint(_sourceFingerprintSelect, fingerprint);
    int stepResult = sqlite3_step(_sourceFingerprintSelect);
    sqlite3_reset(_sourceFingerprintSelect);
    switch (stepResult) {
        case SQLITE_ROW: return true;
        case SQLITE_DONE: return false;
        default: throwStmt(_sourceFingerprintSelect, stepResult); return false;
    }
}

void AssetDatabase::addFile(const std::filesystem::path& path, const FileFingerprint& fingerprint) {
    bindPathHash(_fileInsert, "PATH", path);
    bindTxt(_fileInsert, "PATHSTR", path.c_str());
    bindFingerprint(_fileInsert, fingerprint);
    int stepResult = sqlite3_step(_fileInsert);
    if (stepResult != SQLITE_OK) {
        throwStmt(_fileInsert, stepResult);
    }
    sqlite3_reset(_fileInsert);
}

void AssetDatabase::addSource(const SourceAsset& asset) {
    bindPathHash(_sourceInsert, "PATH", asset.handle);
    int bind = sqlite3_bind_parameter_index(_sourceInsert, "FORMAT");
    int bindResult = sqlite3_bind_int(_sourceInsert, bind, static_cast<int>(asset.format));
    if (bindResult != SQLITE_OK) {
        throw std::runtime_error(fmt::format(
            "Error #{} while binding file format\nError message: {:?}\nStatement: {:?}", 
            bindResult, sqlite3_errmsg(_assetDB), sqlite3_sql(_sourceInsert)));
    }
    int stepResult = sqlite3_step(_sourceInsert);
    if (stepResult != SQLITE_OK) {
        throwStmt(_sourceInsert, stepResult);
    }
    sqlite3_reset(_sourceInsert);
}

void AssetDatabase::addProduct(const ProductAsset& product) {
    bindPathHash(_productInsert, "PATH", product.handle);
    bindTxt(_productInsert, "NAME", product.processedBy.name.c_str());
    bindTxt(_productInsert, "VERSION", product.processedBy.version.fullString().c_str());
    bindTxt(_productInsert, "SETTINGS", product.settingsUsed.as_string().c_str());
    int stepResult = sqlite3_step(_productInsert);
    if (stepResult != SQLITE_OK) {
        throwStmt(_productInsert, stepResult);
    }
    sqlite3_reset(_productInsert);
}

void AssetDatabase::addUnknown(const std::filesystem::path& path, const int& errorCode, std::optional<std::string> message) {
    bindPathHash(_unknownInsert, "PATH", path);
    int bind = sqlite3_bind_parameter_index(_unknownInsert, "ERRCODE");
    int bindResult = sqlite3_bind_int(_unknownInsert, bind, static_cast<int>(errorCode));
    if (bindResult != SQLITE_OK) {
        throw std::runtime_error(fmt::format(
            "Error #{} while binding file error code\nError message: {:?}\nStatement: {:?}", 
            bindResult, sqlite3_errmsg(_assetDB), sqlite3_sql(_unknownInsert)));
    }
    if (message.has_value()) {
        bindTxt(_unknownInsert, "MESSAGE", message->c_str());
    } else {
        bind = sqlite3_bind_parameter_index(_unknownInsert, "MESSAGE");
        bindResult = sqlite3_bind_null(_unknownInsert, bind);
    }
    int stepResult = sqlite3_step(_unknownInsert);
    if (stepResult != SQLITE_OK) {
        throwStmt(_unknownInsert, stepResult);
    }
    sqlite3_reset(_unknownInsert);
}

void AssetDatabase::addDependency(const std::filesystem::path& consumer, const std::filesystem::path& dependency) {
    bindPathHash(_depInsert, "CONSUMER", consumer);
    bindPathHash(_depInsert, "DEP", dependency);
    int stepResult = sqlite3_step(_depInsert);
    if (stepResult != SQLITE_OK) {
        throwStmt(_depInsert, stepResult);
    }
    sqlite3_reset(_depInsert);
}

void AssetDatabase::addProductSource(const std::filesystem::path& product, const std::filesystem::path& source) {
    bindPathHash(_productSourcesQuery, "PRODUCT", product);
    bindPathHash(_productSourcesQuery, "SOURCE", source);
    int stepResult = sqlite3_step(_productSourcesQuery);
    if (stepResult != SQLITE_OK) {
        throwStmt(_productSourcesQuery, stepResult);
    }
    sqlite3_reset(_productSourcesQuery);
}

void AssetDatabase::removeAsSource(const std::filesystem::path& file) {
    bindPathHash(_sourceRemove, "PATH", file);
    int stepResult = sqlite3_step(_sourceRemove);
    if (stepResult != SQLITE_OK) {
        throwStmt(_sourceRemove, stepResult);
    }
    sqlite3_reset(_sourceRemove);
}

void AssetDatabase::removeAsProduct(const std::filesystem::path& file) {
    bindPathHash(_productRemove, "PATH", file);
    int stepResult = sqlite3_step(_productRemove);
    if (stepResult != SQLITE_OK) {
        throwStmt(_productRemove, stepResult);
    }
    sqlite3_reset(_productRemove);
}

void AssetDatabase::removeAsUnknown(const std::filesystem::path& file) {
    bindPathHash(_unknownRemove, "PATH", file);
    int stepResult = sqlite3_step(_unknownRemove);
    if (stepResult != SQLITE_OK) {
        throwStmt(_unknownRemove, stepResult);
    }
    sqlite3_reset(_unknownRemove);
}

void AssetDatabase::removeFile(const std::filesystem::path& file) {
    bindPathHash(_fileRemove, "PATH", file);
    int stepResult = sqlite3_step(_fileRemove);
    if (stepResult != SQLITE_OK) {
        throwStmt(_fileRemove, stepResult);
    }
    sqlite3_reset(_fileRemove);
}

void AssetDatabase::updateName(const std::filesystem::path& oldFile, const std::filesystem::path& newFile) {
    bindPathHash(_updateName, "PATH", oldFile);
    bindPathHash(_updateName, "NEWPATH", newFile);
    bindTxt(_updateName, "PATHSTR", newFile.c_str());
    int stepResult = sqlite3_step(_updateName);
    if (stepResult != SQLITE_OK) {
        throwStmt(_updateName, stepResult);
    }
    sqlite3_reset(_updateName);
}

void AssetDatabase::updateFingerprint(const std::filesystem::path& file, const FileFingerprint& fingerprint) {
    bindPathHash(_updateFingerprint, "PATH", file);
    bindFingerprint(_updateFingerprint, fingerprint);
    int stepResult = sqlite3_step(_updateFingerprint);
    if (stepResult != SQLITE_OK) {
        throwStmt(_updateFingerprint, stepResult);
    }
    sqlite3_reset(_updateFingerprint);
}

void AssetDatabase::updateProduct(const std::filesystem::path& file, const AssetProcessorFingerprint& fingerprint, const std::string& settings) {
    bindPathHash(_updateProduct, "PATH", file);
    bindTxt(_updateProduct, "NAME", fingerprint.name.c_str());
    bindTxt(_updateProduct, "VERSION", fingerprint.version.fullString().c_str());
    bindTxt(_updateProduct, "SETTINGS", settings.c_str());
    int stepResult = sqlite3_step(_updateProduct);
    if (stepResult != SQLITE_OK) {
        throwStmt(_updateProduct, stepResult);
    }
    sqlite3_reset(_updateProduct);
}

bool AssetDatabase::hasUnknownDependencies(const std::filesystem::path& consumer) const {
    bindPathHash(_queryKnownDeps, "PATH", consumer);
    int stepResult = sqlite3_step(_queryKnownDeps);
    sqlite3_reset(_queryKnownDeps);
    switch (stepResult) {
        case SQLITE_ROW: return true;
        case SQLITE_DONE: return false;
        default: throwStmt(_queryKnownDeps, stepResult); return false;
    }
}

AssetDatabase::~AssetDatabase() {
    // Finalize all member prepared statements
    sqlite3_finalize(_filePathSelect);
    sqlite3_finalize(_fileFingerprintSelect);
    sqlite3_finalize(_sourcePathSelect);
    sqlite3_finalize(_sourceFingerprintSelect);
    sqlite3_finalize(_productPathSelect);
    sqlite3_finalize(_productFingerprintSelect);
    sqlite3_finalize(_unknownPathSelect);
    sqlite3_finalize(_unknownFingerprintSelect);
    sqlite3_finalize(_productSourcesQuery);
    sqlite3_finalize(_depsQuery);
    sqlite3_finalize(_assetTypesQuery);
    sqlite3_finalize(_fileInsert);
    sqlite3_finalize(_sourceInsert);
    sqlite3_finalize(_productInsert);
    sqlite3_finalize(_unknownInsert);
    sqlite3_finalize(_assetTypesInsert);
    sqlite3_finalize(_depInsert);
    sqlite3_finalize(_productSourcesInsert);
    sqlite3_finalize(_fileRemove);
    sqlite3_finalize(_sourceRemove);
    sqlite3_finalize(_productRemove);
    sqlite3_finalize(_unknownRemove);
    sqlite3_finalize(_assetTypesRemove);
    sqlite3_finalize(_depRemove);
    sqlite3_finalize(_productSourcesRemove);
    sqlite3_finalize(_updateFingerprint);
    sqlite3_finalize(_updateProduct);
    sqlite3_finalize(_updateName);
    sqlite3_finalize(_queryKnownDeps);
    sqlite3_finalize(_productProductQuery);

    int closeResult = sqlite3_close(_assetDB);
    if (closeResult != SQLITE_OK) {
        throw std::runtime_error(
            fmt::format("Cannot close asset DB for migration purposes: error #{}\n"
            "Tip: error #5 likely means we forgot to close some BLOB handle or finalize the prepared statement", closeResult));
    }
}

} // namespace NovaEngine