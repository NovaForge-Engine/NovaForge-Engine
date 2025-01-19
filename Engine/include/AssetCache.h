#pragma once

#ifndef NOVA_ASSET_CACHE_H
#define NOVA_ASSET_CACHE_H

#include <sqlite3.h>
#include <fmt/format.h>

#include <filesystem>

#include "Singleton.h"
#include "include/ProductAsset.h"
#include "include/SourceAsset.h"

namespace NovaEngine {

struct ENGINE_DLL AssetDatabase: public Singleton<AssetDatabase> {
    AssetDatabase();
    ~AssetDatabase();

    [[nodiscard]] bool isKnownSourceFile(const std::filesystem::path& file);
    [[nodiscard]] bool isKnownProductFile(const std::filesystem::path& file);
    [[nodiscard]] bool isKnownSourceFileFingerprint(const FileFingerprint& fingerprint);
    [[nodiscard]] bool isKnownProductFileFingerprint(const FileFingerprint& fingerprint);
    [[nodiscard]] bool isUnknownFile(const std::filesystem::path& file);
    [[nodiscard]] bool isUnknownFileFingerprint(const FileFingerprint& fingerprint);
    [[nodiscard]] bool isInDatabase(const std::filesystem::path& file);
    [[nodiscard]] std::vector<AssetType> getAssetTypes(const std::filesystem::path& file) const;

    [[nodiscard]] FileFingerprint getFingerprint(const std::filesystem::path& file) const;
    [[nodiscard]] SourceAsset getSource(const std::filesystem::path& file) const;
    [[nodiscard]] ProductAsset getProduct(const std::filesystem::path& file) const;
    [[nodiscard]] SourceAsset getSourceFromFingerprint(const FileFingerprint& fingerprint) const;
    [[nodiscard]] ProductAsset getProductFromFingerprint(const FileFingerprint& fingerprint) const;
    [[nodiscard]] std::vector<SourceAsset> getAssetDependencies(const std::filesystem::path& file) const;
    [[nodiscard]] std::vector<ProductAsset> getProductsFromSource(const std::filesystem::path& file) const;
    [[nodiscard]] std::vector<SourceAsset> getSourcesFromProduct(const std::filesystem::path& file) const;
    [[nodiscard]] bool hasUnknownDependencies(const std::filesystem::path& consumer) const;

    void addFile(const std::filesystem::path& path, const FileFingerprint& fingerprint);
    // Addition for changing asset category
    void addSource(const SourceAsset& asset);
    void addProduct(const ProductAsset& product);
    void addUnknown(const std::filesystem::path& path, const int& errorCode, std::optional<std::string> message);

    void addNewSource(const SourceAsset& asset) { addFile(asset.handle, asset.fingerprint); addSource(asset); };
    void addNewProduct(const ProductAsset& asset) { addFile(asset.handle, asset.fingerprint); addProduct(asset); };
    void addNewUnknown(const std::filesystem::path& path, const FileFingerprint& fingerprint, const int& errorCode, std::optional<std::string> message) { 
        addFile(path, fingerprint); addUnknown(path, errorCode, std::move(message)); 
    };

    void addDependency(const std::filesystem::path& consumer, const std::filesystem::path& dependency);
    void addProductSource(const std::filesystem::path& product, const std::filesystem::path& source);

    void removeFile(const std::filesystem::path& file);
    // Removal for changing asset category
    void removeAsSource(const std::filesystem::path& file);
    void removeAsProduct(const std::filesystem::path& file);
    void removeAsUnknown(const std::filesystem::path& file);

    void updateFingerprint(const std::filesystem::path& file, const FileFingerprint& fingerprint);
    void updateName(const std::filesystem::path& oldFIle, const std::filesystem::path& newFile);
    void updateProduct(const std::filesystem::path& file, const AssetProcessorFingerprint& fingerprint, const std::string& settings);

    private:
    sqlite3* _assetDB;
    // Queries
    const std::string sf = "SELECT * FROM ";
    const std::string sff = "SELECT pathHash, path, fileHash, modified, ";
    const std::string fij = " INNER JOIN Files on Files.pathHash = ";
    const std::string fprint = " WHERE (Files.fileHash = :FILE AND Files.modified = :TIME)";
    const std::string wph = " WHERE file = :PATH";

    sqlite3_stmt *_filePathSelect, *_fileFingerprintSelect;
    const std::string _fps = sf + "Files WHERE pathHash = :PATH;";
    const std::string _ffs = sf + "Files" + fprint + ";";
    sqlite3_stmt *_sourcePathSelect, *_sourceFingerprintSelect;
    const std::string _sps = sf + "SourceAssets" + wph + ";";
    const std::string _sfs = sff + "format FROM SourceAssets" + fij + "SourceAssets.file" + fprint + ";";
    sqlite3_stmt *_productPathSelect, *_productFingerprintSelect;
    const std::string _pps = sf + "ProductAssets" + wph + ";";
    const std::string _pfs = sff + "processedBy, processedVersion, settings FROM ProductAssets" + fij + "ProductAssets.file" + fprint + ";";
    sqlite3_stmt *_unknownPathSelect, *_unknownFingerprintSelect;
    const std::string _ups = sf + "UnknownFiles" + wph + ";";
    const std::string _ufs = sff + "error, message FROM UnknownFiles" + fij + "UnknownFiles.file" + fprint + ";";
    sqlite3_stmt *_productSourcesQuery, *_productProductQuery, *_depsQuery;
    const std::string _psq = sff + "processedBy, processedVersion, settings FROM ProductSources INNER JOIN ProductAssets ON (ProductAssets.file = ProductSources.product) INNER JOIN Files ON (Files.pathHash = ProductSources.product) WHERE source = :PATH;";
    const std::string _ppq = sff + "format FROM ProductSources INNER JOIN SourceAssets ON (SourceAssets.file = ProductSources.source) INNER JOIN Files ON (Files.pathHash = ProductSources.source) WHERE product = :PATH;";
    const std::string _dq  = sff + "format FROM Dependencies INNER JOIN SourceAssets on (SourceAssets.file = Dependencies.dependency) INNER JOIN Files ON (Files.pathHash = Dependencies.dependency) WHERE consumer = :PATH;";
    sqlite3_stmt *_assetTypesQuery;
    const std::string _atq = sf + "AssetTypes" + wph + ";";
    // Add
    const std::string ii = "INSERT INTO ";
    sqlite3_stmt *_fileInsert, *_sourceInsert, *_productInsert, *_unknownInsert, *_assetTypesInsert;
    const std::string _fi = ii + "Files VALUES (:PATH, :PATHSTR, :FILE, :TIME);";
    const std::string _si = ii + "SourceAssets VALUES (:PATH, :FORMAT);";
    const std::string _pi = ii + "ProductAssets VALUES (:PATH, :NAME, :VERSION, :SETTINGS);";
    const std::string _ui = ii + "UnknownFiles VALUES (:PATH, :ERRCODE, :MESSAGE);";
    const std::string _ati = ii + "AssetTypes VALUES (:PATH, :TYPE)";
    sqlite3_stmt *_depInsert, *_productSourcesInsert;
    const std::string _di = ii + "Dependencies VALUES (:CONSUMER, :DEP);";
    const std::string _psi = ii + "ProductSources VALUES (:PRODUCT, :SOURCE);";
    // Remove
    const std::string df = "DELETE FROM ";
    sqlite3_stmt *_fileRemove, *_sourceRemove, *_productRemove, *_unknownRemove, *_assetTypesRemove;
    const std::string _fr = df + "Files WHERE pathHash = :PATH";
    const std::string _sr = df + "SourceAssets" + wph + ";";
    const std::string _pr = df + "ProductAssets" + wph + ";";
    const std::string _ur = df + "UnknownFiles" + wph + ";";
    const std::string _atr = df + "AssetTypes" + wph + ";";
    sqlite3_stmt *_depRemove, *_productSourcesRemove;
    const std::string _dr = df + "Dependencies WHERE consumer = :PATH;";
    const std::string _psr = df + "ProductSources WHERE product = :PATH;";
    sqlite3_stmt *_updateFingerprint, *_updateProduct, *_updateName;
    const std::string _uf = "UPDATE Files SET fileHash = :FILE, modified = :TIME WHERE pathHash = :PATH;";
    const std::string _un = "UPDATE Files SET pathHash = :NEWPATH, path = :PATHSTR WHERE pathHash = :PATH;";
    const std::string _up = "UPDATE ProductAssets SET processedBy = :NAME, processedVersion = :VERSION, settings = :SETTINGS" + wph + ";";
    sqlite3_stmt *_queryKnownDeps;
    const std::string _qkd = "SELECT COUNT(*) FROM UnknownFiles INNER JOIN Dependencies ON (Dependencies.dependency = UnknownFiles.pathHash) WHERE Dependencies.consumer = :PATH;";

    inline void prepLongtermStmt(const char *zSql, sqlite3_stmt **stmt);
    void prepStmt(const char *zSql, sqlite3_stmt **stmt);
    void throwStmt(sqlite3_stmt *stmt, const int& error) const;
    void bindBlob(sqlite3_stmt *stmt, const char * name, const void *ptr, const int& size) const;
    void bindPathHash(sqlite3_stmt *stmt, const char* name, const std::filesystem::path& path) const;
    void bindFingerprint(sqlite3_stmt *stmt, const FileFingerprint& fingerprint) const;
    void bindTxt(sqlite3_stmt *stmt, const char* name, const char* value) const;
    static int findColumn(sqlite3_stmt *stmt, const char* colName);

    template<typename T>
    T getColumn(sqlite3_stmt *stmt, const char* colName) const;
};

template<> inline std::vector<uint8_t> AssetDatabase::getColumn<std::vector<uint8_t>>(sqlite3_stmt *stmt, const char* colName) const {
    int iCol = findColumn(stmt, colName);
    int size = sqlite3_column_bytes(stmt, iCol);
    std::string res{(const char *)sqlite3_column_text(stmt, iCol)};
    std::vector<uint8_t> result;
    result.resize(size);
    std::copy(res.begin(), res.end(), result.begin());
    return result;
};
template<> inline const void * AssetDatabase::getColumn<const void *>(sqlite3_stmt *stmt, const char* colName) const {
    int iCol = findColumn(stmt, colName);
    int size = sqlite3_column_bytes(stmt, iCol);
    auto result = (unsigned char *)malloc(size);
    memcpy((char *)result, (const char *)sqlite3_column_blob(stmt, iCol), size);
    return result;
};
template<> inline std::filesystem::file_time_type AssetDatabase::getColumn<std::filesystem::file_time_type>(sqlite3_stmt *stmt, const char* colName) const {
    int iCol = findColumn(stmt, colName);
    auto result = sqlite3_column_text(stmt, iCol);
    // maaxxaam @ 08.01.25
    // TODO: can we avoid casting to std::tm?
    std::tm tm = {};
    std::istringstream ss{std::string(reinterpret_cast<const char *>(result))};
    ss >> std::get_time(&tm, "%FT%T");
    auto res = std::chrono::file_clock::from_sys(std::chrono::system_clock::from_time_t(std::mktime(&tm)));
    return res;
};
template<> inline std::string AssetDatabase::getColumn<std::string>(sqlite3_stmt *stmt, const char* colName) const {
    int iCol = findColumn(stmt, colName);
    return {(const char *)sqlite3_column_text(stmt, iCol)};
};
template<> inline const unsigned char * AssetDatabase::getColumn<const unsigned char *>(sqlite3_stmt *stmt, const char* colName) const {
    int iCol = findColumn(stmt, colName);
    // Add one to account for '\0'
    int size = sqlite3_column_bytes(stmt, iCol) + 1;
    auto result = (unsigned char *)malloc(size);
    strncpy((char *)result, (const char *)sqlite3_column_text(stmt, iCol), size);
    return result;
};
template<> inline double AssetDatabase::getColumn<double>(sqlite3_stmt *stmt, const char* colName) const {
    int iCol = findColumn(stmt, colName);
    return sqlite3_column_double(stmt, iCol);
};
template<> inline int AssetDatabase::getColumn<int>(sqlite3_stmt *stmt, const char* colName) const {
    int iCol = findColumn(stmt, colName);
    return sqlite3_column_int(stmt, iCol);
};
template<> inline long long AssetDatabase::getColumn<long long>(sqlite3_stmt *stmt, const char* colName) const {
    int iCol = findColumn(stmt, colName);
    return sqlite3_column_int64(stmt, iCol);
};

} // namespace NovaEngine

#endif // NOVA_ASSET_CACHE_H