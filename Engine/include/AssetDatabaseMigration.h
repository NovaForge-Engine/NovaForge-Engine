#pragma once

#ifndef NOVA_ASSET_DATABASE_MIGRATION_H
#define NOVA_ASSET_DATABASE_MIGRATION_H

#include <sqlite3.h>
#include <fmt/format.h>

#include <string>
#include <vector>

namespace NovaEngine {

constexpr std::string DB_FILE_NAME = "assetDb.sqlite";
const std::string DB_SETUP_COMMANDS = "PRAGMA foreign_keys = ON;";

struct TableInfo {
    std::string name;
    std::string creationParams;
    std::string creationCommands = ";";
};

struct AssetDatabaseMigration {
    AssetDatabaseMigration();
    ~AssetDatabaseMigration();
    void checkMigrations();

    private:
    sqlite3 *_assetDB;
    sqlite3_stmt *_checkVersion, *_checkTable, *_dropTable;
    long long _curSchemaVersion = 0;
    /*
     * Notes about _schema:
     * - creationParams contains only table columns and constraints 
     *   (everything inside braces of "CREATE TABLE"), for other things use creationCommands
     * - creationCommands contains stuff other than "CREATE TABLE" to make a table
     *   (triggers and such)
     * - All tables are STRICT, so only types REAL, INTEGER, TEXT, BLOB or ANY are allowed
     */
    const long long _targetSchemaVersion = 0;
    const std::vector<TableInfo> _schema = {
        {
            .name = "SchemaVersion",
            .creationParams = "version INTEGER NOT NULL DEFAULT 0",
            .creationCommands = fmt::format("INSERT VALUES INTO SchemaVersion ({:d});", _targetSchemaVersion)
        },
        {
            .name = "Files",
            .creationParams = fmt::format("pathHash BLOB PRIMARY KEY NOT NULL, path {0} UNIQUE, fileHash BLOB NOT NULL UNIQUE, modified {0}", TNN)
        },
        {
            .name = "SourceAssets",
            .creationParams = fmt::format("file {0}, format INTEGER NOT NULL", fileRefV1)
        },
        {
            .name = "ProductAssets",
            .creationParams = fmt::format("file {0}, processedBy {1}, processedVersion {1}, settings {1}", fileRefV1, TNN)
        },
        {
            .name = "AssetTypes",
            .creationParams = fmt::format("file {0}, type INTEGER NOT NULL, PRIMARY KEY file", fileRefV1)
        },
        {
            .name = "UnknownFiles",
            .creationParams = fmt::format("file {0}, error INTEGER NOT NULL, message TEXT, PRIMARY KEY file", fileRefV1)
        },
        {
            .name = "Dependencies",
            .creationParams = fmt::format("consumer {0}, dependency {0}", fileRefV1)
        },
        {
            .name = "ProductSources",
            .creationParams = fmt::format("product {0}, source {0}", fileRefV1),
            .creationCommands = 
                "CREATE TRIGGER IF NOT EXISTS productSources_check_product BEFORE INSERT ON ProductSources BEGIN\n"
                "SELECT CASE\n"
                "WHEN EXISTS (SELECT 1 FROM ProductAssets WHERE file = new.product) THEN 1\n"
                "ELSE RAISE(FAIL, 'Product not found in ProductAssets')\n"
                "END;\n"
                ""
                "CREATE TRIGGER IF NOT EXISTS productSources_check_source BEFORE INSERT ON ProductSources BEGIN\n"
                "SELECT CASE\n"
                "WHEN EXISTS (SELECT 1 FROM SourceAssets WHERE file = new.source) THEN 1\n"
                "ELSE RAISE(FAIL, 'Source not found in SourceAssets')\n"
                "END;\n"
        }
    };
    // `migrationSequences[i]` describes migration from schema version `i` to `i+1` 
    const std::vector<std::string> _migrationSequences = {  };

    inline static std::string composeCreationCommand(const std::string& name, const std::string& columns) { return fmt::format("CREATE TABLE {:s} ({:s}) STRICT;", name, columns); };
    const char * fileRefV1 = "BLOB NOT NULL REFERENCES Files(pathHash) ON UPDATE CASCADE ON DELETE CASCADE";
    const char * TNN = "TEXT NOT NULL";

    std::size_t getIndex(const int& tableIndex, const std::size_t& version);

    static void bindTxt(sqlite3_stmt *stmt, const char* name, const char* value);
    inline void throwStmt(sqlite3_stmt *stmt, const int& error);
    void executeMultipleBindlessStatements(const char * zSql);
    void createTable(const std::string& name, const std::string& columns, const std::string& extraCommands);
    void prepStmt(const char *zSql, sqlite3_stmt **stmt);

    void createCurrentVersion();
    void migrateToCurrent();
};

} // namespace NovaEngine

#endif // NOVA_ASSET_DATABASE_MIGRATION_H