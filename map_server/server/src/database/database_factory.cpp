#include "database_factory.h"
#include "sqlite_database.h"
#include "../utils/logger.h"

#ifdef HAVE_POSTGRESQL
#include "postgresql_database.h"
#endif

namespace cycle {
namespace database {

std::shared_ptr<IDatabase> DatabaseFactory::Create(const std::string& type, 
                                                   const std::string& connectionString) {
    if (type == "sqlite3") {
        return CreateSqlite(connectionString);
    }
#ifdef HAVE_POSTGRESQL
    else if (type == "postgresql") {
        return CreatePostgresql(connectionString);
    }
#endif
    else {
        LOG_ERROR("Unsupported database type: " + type);
        return nullptr;
    }
}

std::shared_ptr<IDatabase> DatabaseFactory::CreateSqlite(const std::string& dbPath) {
    auto db = std::make_shared<SqliteDatabase>(dbPath);
    
    if (!db->Open()) {
        LOG_ERROR("Failed to open SQLite database: " + dbPath);
        return nullptr;
    }
    
    LOG_INFO("SQLite database created successfully: " + dbPath);
    return db;
}

#ifdef HAVE_POSTGRESQL
std::shared_ptr<IDatabase> DatabaseFactory::CreatePostgresql(const std::string& connectionString) {
    DatabaseConfig config; config.pg_database = connectionString;
    auto db = std::make_shared<PostgresqlDatabase>(config);
    
    if (!db->Open()) {
        LOG_ERROR("Failed to open PostgreSQL database");
        return nullptr;
    }
    
    LOG_INFO("PostgreSQL database created successfully");
    return db;
}
#endif

std::vector<std::string> DatabaseFactory::GetSupportedTypes() {
    std::vector<std::string> types = {"sqlite3"};
    
#ifdef HAVE_POSTGRESQL
    types.push_back("postgresql");
#endif
    
    return types;
}

} // namespace database
} // namespace cycle
