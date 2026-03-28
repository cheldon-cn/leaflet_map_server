#include "ogc/mokrender/database_manager.h"
#include "ogc/db/sqlite_connection.h"
#include "ogc/geometry.h"
#include "ogc/point.h"
#include "ogc/linestring.h"
#include "ogc/polygon.h"
#include <string>
#include <map>
#include <sstream>
#include <sqlite3.h>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#undef GetMessage
#endif

namespace ogc {
namespace mokrender {

static std::string GetExecutableDirectory() {
#ifdef _WIN32
    char path[MAX_PATH] = {0};
    GetModuleFileNameA(NULL, path, sizeof(path));
    PathRemoveFileSpecA(path);
    return std::string(path);
#else
    return ".";
#endif
}

static bool CheckSpatialMetaDataExists(sqlite3* db) {
    if (!db) return false;
    
    const char* sql = "SELECT name FROM sqlite_master WHERE type='table' AND name='geometry_columns'";
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        return false;
    }
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_ROW;
}

static bool CheckSpatialiteFunctionsExist(sqlite3* db) {
    if (!db) return false;
    
    const char* sql = "SELECT GeomFromEWKB(X'010100000000000000000000000000000000000000')";
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        return false;
    }
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_ROW;
}

static bool TryLoadSpatialiteExtension(sqlite3* db) {
    if (!db) return false;
    
    int rc = sqlite3_enable_load_extension(db, 1);
    if (rc != SQLITE_OK) {
        return false;
    }
    
    std::string exeDir = GetExecutableDirectory();
    
    std::vector<std::string> extensionPaths = {
        exeDir + "\\mod_spatialite.dll",
        exeDir + "\\mod_spatialite5.dll",
        exeDir + "\\spatialite.dll",
        exeDir + "\\spatialite5.dll",
        "mod_spatialite",
        "mod_spatialite5",
        "spatialite",
        "spatialite5"
    };
    
    for (const auto& extPath : extensionPaths) {
        char* errMsg = nullptr;
        rc = sqlite3_load_extension(db, extPath.c_str(), nullptr, &errMsg);
        if (rc == SQLITE_OK) {
            return true;
        }
        if (errMsg) {
            sqlite3_free(errMsg);
        }
    }
    
    return false;
}

class DatabaseManager::Impl {
public:
    std::string dbPath;
    bool isOpen;
    db::SpatiaLiteConnectionPtr connection;
    
    Impl() : isOpen(false) {}
};

DatabaseManager::DatabaseManager() : m_impl(new Impl()) {
}

DatabaseManager::~DatabaseManager() {
    Close();
}

MokRenderResult DatabaseManager::Initialize(const std::string& dbPath) {
    m_impl->dbPath = dbPath;
    
    m_impl->connection = db::SpatiaLiteConnection::Create();
    if (!m_impl->connection) {
        return MokRenderResult(MokRenderErrorCode::OUT_OF_MEMORY, 
                              "Failed to create connection");
    }
    
    db::SpatiaLiteOptions options;
    options.databasePath = dbPath;
    options.createIfNotExist = true;
    
    db::Result result = m_impl->connection->Connect(options);
    if (result.IsError()) {
        return MokRenderResult(MokRenderErrorCode::DATABASE_ERROR, 
                              "Failed to connect: " + result.GetMessage());
    }
    
    sqlite3* db = m_impl->connection->GetRawConnection();
    if (!db) {
        return MokRenderResult(MokRenderErrorCode::DATABASE_ERROR,
                              "Failed to get raw database connection");
    }
    
    bool spatialReady = CheckSpatialMetaDataExists(db);
    bool spatialiteFunctionsReady = CheckSpatialiteFunctionsExist(db);
    
    if (!spatialiteFunctionsReady) {
        if (!TryLoadSpatialiteExtension(db)) {
            return MokRenderResult(MokRenderErrorCode::DATABASE_ERROR,
                                  "SpatiaLite extension not loaded. Please use init_spatialite_db.ps1 to create a pre-initialized database, or ensure mod_spatialite.dll and its dependencies are available.");
        }
    }
    
    if (!spatialReady) {
        result = m_impl->connection->InitializeSpatialMetaData();
        if (result.IsError()) {
            std::string errMsg = result.GetMessage();
            return MokRenderResult(MokRenderErrorCode::DATABASE_ERROR,
                                  "Failed to initialize spatial metadata: " + errMsg);
        }
    }
    
    m_impl->isOpen = true;
    return MokRenderResult();
}

int GeometryTypeToInt(const std::string& geometryType) {
    if (geometryType == "Point") {
        return 1;
    } else if (geometryType == "LineString") {
        return 2;
    } else if (geometryType == "Polygon") {
        return 3;
    } else if (geometryType == "MultiPoint") {
        return 4;
    } else if (geometryType == "MultiLineString") {
        return 5;
    } else if (geometryType == "MultiPolygon") {
        return 6;
    }
    return 0;
}

MokRenderResult DatabaseManager::CreateTable(const std::string& tableName,
                                             const std::string& geometryType) {
    if (!m_impl->isOpen) {
        return MokRenderResult(MokRenderErrorCode::DATABASE_ERROR, 
                              "Database not open");
    }
    
    if (m_impl->connection->SpatialTableExists(tableName)) {
        return MokRenderResult();
    }
    
    int geomType = GeometryTypeToInt(geometryType);
    
    db::Result result = m_impl->connection->CreateSpatialTable(
        tableName, "geom", geomType, 4326, 2);
    
    if (result.IsError()) {
        return MokRenderResult(MokRenderErrorCode::DATABASE_ERROR,
                              "Failed to create table: " + result.GetMessage());
    }
    
    std::vector<std::string> alterSqls = {
        "ALTER TABLE " + tableName + " ADD COLUMN name TEXT",
        "ALTER TABLE " + tableName + " ADD COLUMN category TEXT",
        "ALTER TABLE " + tableName + " ADD COLUMN code INTEGER",
        "ALTER TABLE " + tableName + " ADD COLUMN value REAL",
        "ALTER TABLE " + tableName + " ADD COLUMN active INTEGER",
        "ALTER TABLE " + tableName + " ADD COLUMN description TEXT",
        "ALTER TABLE " + tableName + " ADD COLUMN created_at TEXT"
    };
    
    for (const auto& sql : alterSqls) {
        result = m_impl->connection->Execute(sql);
        if (result.IsError()) {
        }
    }
    
    result = m_impl->connection->CreateSpatialIndex(tableName, "geom");
    if (result.IsError()) {
    }
    
    return MokRenderResult();
}

MokRenderResult DatabaseManager::InsertFeature(const std::string& tableName,
                                               void* feature) {
    if (!m_impl->isOpen) {
        return MokRenderResult(MokRenderErrorCode::DATABASE_ERROR,
                              "Database not open");
    }
    
    if (!feature) {
        return MokRenderResult(MokRenderErrorCode::INVALID_PARAMETER,
                              "Feature is null");
    }
    
    Geometry* geometry = static_cast<Geometry*>(feature);
    
    std::map<std::string, std::string> attributes;
    attributes["name"] = "feature_" + std::to_string(static_cast<int>(geometry->GetGeometryType()));
    
    int64_t outId = 0;
    db::Result result = m_impl->connection->InsertGeometry(
        tableName, "geom", geometry, attributes, outId);
    
    if (result.IsError()) {
        return MokRenderResult(MokRenderErrorCode::DATABASE_ERROR,
                              "Failed to insert feature: " + result.GetMessage());
    }
    
    return MokRenderResult();
}

MokRenderResult DatabaseManager::InsertFeatures(const std::string& tableName,
                                                const std::vector<void*>& features) {
    if (!m_impl->isOpen) {
        return MokRenderResult(MokRenderErrorCode::DATABASE_ERROR,
                              "Database not open");
    }
    
    std::vector<const Geometry*> geometries;
    std::vector<std::map<std::string, std::string>> attributesList;
    
    static int featureCounter = 0;
    
    std::vector<std::string> categories = {"A", "B", "C", "D"};
    std::vector<std::string> descriptions = {
        "Sample feature description 1",
        "Sample feature description 2",
        "Sample feature description 3"
    };
    
    for (size_t i = 0; i < features.size(); ++i) {
        Geometry* geometry = static_cast<Geometry*>(features[i]);
        if (geometry) {
            geometries.push_back(geometry);
            std::map<std::string, std::string> attributes;
            
            attributes["name"] = "feature_" + std::to_string(featureCounter);
            attributes["category"] = categories[featureCounter % categories.size()];
            attributes["code"] = std::to_string(featureCounter * 100);
            attributes["value"] = std::to_string(static_cast<double>(featureCounter) * 1.5);
            attributes["active"] = std::to_string(featureCounter % 2);
            attributes["description"] = descriptions[featureCounter % descriptions.size()];
            attributes["created_at"] = "2026-03-28 12:00:00";
            
            attributesList.push_back(attributes);
            featureCounter++;
        }
    }
    
    std::vector<int64_t> outIds;
    db::Result result = m_impl->connection->InsertGeometries(
        tableName, "geom", geometries, attributesList, outIds);
    
    if (result.IsError()) {
        return MokRenderResult(MokRenderErrorCode::DATABASE_ERROR,
                              "Failed to insert features: " + result.GetMessage());
    }
    
    return MokRenderResult();
}

void DatabaseManager::Close() {
    if (m_impl->connection) {
        m_impl->connection->Disconnect();
        m_impl->connection.reset();
    }
    m_impl->isOpen = false;
}

}
}
