#include <fmt/format.h>

#include "include/AssetDatabaseMigration.h"

namespace NovaEngine {

AssetDatabaseMigration::AssetDatabaseMigration() {
    int openResult = sqlite3_open(fmt::format("./{}", DB_FILE_NAME).c_str(), &_assetDB);
    if (openResult != SQLITE_OK) {
        throw std::runtime_error(fmt::format("Couldn't load asset DB for migration purposes: error #{}", openResult));
    }
    executeMultipleBindlessStatements(DB_SETUP_COMMANDS.c_str());
    const char* versionStmt = "SELECT version FROM TableVersions;";
    prepStmt(versionStmt, &_checkVersion);
    const char* tableStmt = "PRAGMA table_list(:TABLENAME);";
    prepStmt(tableStmt, &_checkTable);
    const char* dropTableStmt = "DROP TABLE IF EXISTS :TABLENAME;";
    prepStmt(dropTableStmt, &_dropTable);
}

void AssetDatabaseMigration::prepStmt(const char* zSql, sqlite3_stmt** stmt) {
    const char* tail;
    int prepResult = sqlite3_prepare_v2(_assetDB, zSql, -1, stmt, &tail);
    if (prepResult != SQLITE_OK) {
        throw std::runtime_error(fmt::format("Can't prepare statement: error #{}\nStatement: {:?}", prepResult, zSql));
    }
}

void AssetDatabaseMigration::bindTxt(sqlite3_stmt *stmt, const char* name, const char* value) {
    int bind = sqlite3_bind_parameter_index(stmt, name);
    sqlite3_bind_text(stmt, bind, value, -1, nullptr);
}

void AssetDatabaseMigration::checkMigrations() {
    // check if SchemaVersion exists
    bindTxt(_checkTable, "TABLENAME", "SchemaVersion");
    int stepResult = sqlite3_step(_checkTable);
    sqlite3_reset(_checkTable);
    if (stepResult != SQLITE_ROW) {
        // Not a row returned likely means there are none, so we assume that
        // In turn, that means there is likely nothing / malformed schema, so we create one from scratch
        createCurrentVersion();
        return;
    }
    // See if we need to migrate
    stepResult = sqlite3_step(_checkVersion);
    if (stepResult != SQLITE_ROW) {
        fmt::println("Note about throw below: error #101 means SchemaVersion table is empty which must not regularly happen");
        throwStmt(_checkVersion, stepResult);
    }
    _curSchemaVersion = sqlite3_column_int64(_checkVersion, 0);
    if (_curSchemaVersion < _targetSchemaVersion) {
        migrateToCurrent();
    }
}

void AssetDatabaseMigration::createCurrentVersion() {
    for (auto& table: _schema) {
        // Drop table if it exists first
        bindTxt(_dropTable, "TABLENAME", table.name.c_str());
        sqlite3_step(_dropTable);
        sqlite3_reset(_dropTable);

        createTable(table.name, table.creationParams, table.creationCommands);
    }
}

void AssetDatabaseMigration::createTable(const std::string& name, const std::string& columns, const std::string& extraCommands) {
    sqlite3_stmt *stmt;
    const std::string sql = composeCreationCommand(name, columns);
    prepStmt(sql.c_str(), &stmt);
    int stepResult = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (stepResult != SQLITE_DONE) {
        throw std::runtime_error(fmt::format("Unable to create table: error #{}\nStatement: {:?}", stepResult, sql));
    }
    executeMultipleBindlessStatements(extraCommands.c_str());
}

void AssetDatabaseMigration::executeMultipleBindlessStatements(const char* zSql) {
    sqlite3_stmt *stmt;
    const char* tail = zSql;
    do {
        int prepResult = sqlite3_prepare_v2(_assetDB, zSql, -1, &stmt, &tail);
        if (prepResult != SQLITE_OK) {
            throw std::runtime_error(fmt::format("Unable to prep statement: error #{}\nStatement: {:?}", prepResult, zSql));
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

void AssetDatabaseMigration::throwStmt(sqlite3_stmt *stmt, const int& error) {
    // Pointer from sqlite3_expanded_sql has to be freed manually, so we output error then throw
    const char *fullSQL = sqlite3_expanded_sql(stmt);
    const char *errmsg = sqlite3_errmsg(_assetDB);
    fmt::println("Error #{0} while stepping through the statement.\nError description:\n{3}\nRaw:\n{1}\nExpanded:\n{2}", error, sqlite3_sql(stmt), fullSQL, errmsg);
    sqlite3_free((void*)fullSQL);
    throw std::runtime_error(fmt::format("SQLite statement execution error #{}", error));
}

void AssetDatabaseMigration::migrateToCurrent() {
    while (_curSchemaVersion < _targetSchemaVersion) {
        executeMultipleBindlessStatements(_migrationSequences[_curSchemaVersion].c_str());
        _curSchemaVersion++;
    }
}

AssetDatabaseMigration::~AssetDatabaseMigration() {
    // Finalize all member prepared statements
    sqlite3_finalize(_checkTable);
    sqlite3_finalize(_checkVersion);
    sqlite3_finalize(_dropTable);

    int closeResult = sqlite3_close(_assetDB);
    if (closeResult != SQLITE_OK) {
        throw std::runtime_error(
            fmt::format("Cannot close asset DB for migration purposes: error #{}\n"
            "Tip: error #5 likely means we forgot to close some BLOB handle or finalize the prepared statement", closeResult));
    }
}

} // namespace NovaEngine