#include "ogc/db/srid_manager.h"
#include "ogc/db/connection.h"
#include "ogc/db/wkb_converter.h"
#include "ogc/db/postgis_connection.h"
#include "ogc/db/sqlite_connection.h"
#include <sstream>
#include <algorithm>

namespace ogc {
namespace db {

std::unique_ptr<SpatialIndexManager> SpatialIndexManager::Create(DbConnection* connection) {
    return std::make_unique<SpatialIndexManager>(connection);
}

SpatialIndexManager::SpatialIndexManager(DbConnection* connection)
    : m_connection(connection) {
    m_dbType = DatabaseType::kUnknown;
}

SpatialIndexManager::~SpatialIndexManager() {
}

Result SpatialIndexManager::CreateIndex(const std::string& table,
                                       const std::string& geometryColumn,
                                       const SpatialIndexConfig& config) {
    if (!m_connection || !m_connection->IsConnected()) {
        return Result::Error(DbResult::kNotConnected, "Not connected to database");
    }
    
    IndexStrategy strategy = config.strategy;
    if (strategy == IndexStrategy::kAuto) {
        strategy = RecommendStrategy(table, geometryColumn);
    }
    
    switch (m_dbType) {
        case DatabaseType::kPostGIS:
            return CreatePostGISIndex(table, geometryColumn, config);
        case DatabaseType::kSpatiaLite:
            return CreateSpatiaLiteIndex(table, geometryColumn, config);
        default:
            return Result::Error(DbResult::kNotSupported, "Unsupported database type");
    }
}

Result SpatialIndexManager::CreatePostGISIndex(const std::string& table,
                                               const std::string& geometryColumn,
                                               const SpatialIndexConfig& config) {
    std::ostringstream sql;
    
    sql << "CREATE INDEX IF NOT EXISTS idx_" << table << "_" << geometryColumn << " "
        << "ON " << table << " USING GIST (" << geometryColumn << ")";
    
    if (config.fillFactor < 1.0f) {
        sql << " WITH (fillfactor = " << static_cast<int>(config.fillFactor * 100) << ")";
    }
    
    return m_connection->Execute(sql.str());
}

Result SpatialIndexManager::CreateSpatiaLiteIndex(const std::string& table,
                                                  const std::string& geometryColumn,
                                                  const SpatialIndexConfig& config) {
    std::ostringstream sql;
    
    switch (config.strategy) {
        case IndexStrategy::kRTree:
            sql << "SELECT CreateSpatialIndex('" << table << "', '" << geometryColumn << "')";
            break;
        case IndexStrategy::kGrid:
        default:
            sql << "SELECT CreateMbrCache('" << table << "', '" << geometryColumn << "')";
            break;
    }
    
    return m_connection->Execute(sql.str());
}

Result SpatialIndexManager::DropIndex(const std::string& table, const std::string& geometryColumn) {
    if (!m_connection || !m_connection->IsConnected()) {
        return Result::Error(DbResult::kNotConnected, "Not connected to database");
    }
    
    switch (m_dbType) {
        case DatabaseType::kPostGIS:
            return DropPostGISIndex(table, geometryColumn);
        case DatabaseType::kSpatiaLite:
            return DropSpatiaLiteIndex(table, geometryColumn);
        default:
            return Result::Error(DbResult::kNotSupported, "Unsupported database type");
    }
}

Result SpatialIndexManager::DropPostGISIndex(const std::string& table, const std::string& geometryColumn) {
    std::ostringstream sql;
    sql << "DROP INDEX IF EXISTS idx_" << table << "_" << geometryColumn;
    return m_connection->Execute(sql.str());
}

Result SpatialIndexManager::DropSpatiaLiteIndex(const std::string& table, const std::string& geometryColumn) {
    std::ostringstream sql;
    sql << "DROP TABLE IF EXISTS idx_" << table << "_" << geometryColumn;
    return m_connection->Execute(sql.str());
}

Result SpatialIndexManager::RebuildIndex(const std::string& table, const std::string& geometryColumn) {
    Result dropResult = DropIndex(table, geometryColumn);
    if (!dropResult.IsSuccess()) {
        return dropResult;
    }
    return CreateIndex(table, geometryColumn, SpatialIndexConfig());
}

Result SpatialIndexManager::GetMetadata(const std::string& table,
                                       const std::string& geometryColumn,
                                       SpatialIndexMetadata& metadata) {
    metadata.tableName = table;
    metadata.geometryColumn = geometryColumn;
    metadata.isValid = IndexExists(table, geometryColumn);
    metadata.strategy = IndexStrategy::kRTree;
    metadata.rowCount = 0;
    metadata.indexSizeBytes = 0;
    metadata.coverage = 0.0;
    
    if (!metadata.isValid) {
        return Result::Success();
    }
    
    std::ostringstream countSql;
    countSql << "SELECT COUNT(*) FROM " << table;
    
    DbResultSetPtr resultSet;
    Result result = m_connection->ExecuteQuery(countSql.str(), resultSet);
    
    if (result.IsSuccess() && resultSet && resultSet->Next()) {
        metadata.rowCount = resultSet->GetInt64(0);
    }
    
    return Result::Success();
}

Result SpatialIndexManager::AnalyzeIndex(const std::string& table,
                                        const std::string& geometryColumn,
                                        double& selectivity,
                                        int64_t& estimatedRows) {
    selectivity = 0.0;
    estimatedRows = 0;
    
    if (!IndexExists(table, geometryColumn)) {
        return Result::Error(DbResult::kExecutionError, "Index does not exist");
    }
    
    std::ostringstream sql;
    sql << "SELECT reltuples::bigint FROM pg_class "
        << "WHERE relname = '" << table << "'";
    
    DbResultSetPtr resultSet;
    Result result = m_connection->ExecuteQuery(sql.str(), resultSet);
    
    if (result.IsSuccess() && resultSet && resultSet->Next()) {
        estimatedRows = resultSet->GetInt64(0);
        selectivity = estimatedRows > 0 ? 1.0 / estimatedRows : 0.0;
    }
    
    return Result::Success();
}

Result SpatialIndexManager::OptimizeIndex(const std::string& table, const std::string& geometryColumn) {
    if (!IndexExists(table, geometryColumn)) {
        return Result::Error(DbResult::kExecutionError, "Index does not exist");
    }
    
    if (m_dbType == DatabaseType::kPostGIS) {
        std::ostringstream sql;
        sql << "REINDEX INDEX idx_" << table << "_" << geometryColumn;
        return m_connection->Execute(sql.str());
    }
    
    return RebuildIndex(table, geometryColumn);
}

bool SpatialIndexManager::IndexExists(const std::string& table, const std::string& geometryColumn) {
    std::ostringstream sql;
    
    if (m_dbType == DatabaseType::kPostGIS) {
        sql << "SELECT 1 FROM pg_indexes "
            << "WHERE tablename = '" << table << "' "
            << "AND indexname = 'idx_" << table << "_" << geometryColumn << "'";
    } else {
        sql << "SELECT name FROM sqlite_master "
            << "WHERE type='table' AND name='idx_" << table << "_" << geometryColumn << "'";
    }
    
    DbResultSetPtr resultSet;
    Result result = m_connection->ExecuteQuery(sql.str(), resultSet);
    
    return result.IsSuccess() && resultSet && !resultSet->IsEOF();
}

std::vector<std::string> SpatialIndexManager::GetIndexedColumns(const std::string& table) {
    std::vector<std::string> columns;
    
    std::ostringstream sql;
    
    if (m_dbType == DatabaseType::kPostGIS) {
        sql << "SELECT a.attname "
            << "FROM pg_index i "
            << "JOIN pg_attribute a ON a.attrelid = i.indrelid AND a.attnum = ANY(i.indindex) "
            << "JOIN pg_class c ON c.oid = i.indrelid "
            << "WHERE c.relname = '" << table << "'";
    }
    
    DbResultSetPtr resultSet;
    Result result = m_connection->ExecuteQuery(sql.str(), resultSet);
    
    if (result.IsSuccess() && resultSet) {
        while (resultSet->Next()) {
            columns.push_back(resultSet->GetString(0));
        }
    }
    
    return columns;
}

IndexStrategy SpatialIndexManager::RecommendStrategy(const std::string& table,
                                                     const std::string& geometryColumn) const {
    return IndexStrategy::kRTree;
}

std::unique_ptr<SridManager> SridManager::Create(DbConnection* connection) {
    return std::make_unique<SridManager>(connection);
}

SridManager::SridManager(DbConnection* connection)
    : m_connection(connection) {
    m_dbType = DatabaseType::kUnknown;
}

SridManager::~SridManager() {
}

Result SridManager::ValidateSRID(int srid, bool& isValid) {
    isValid = false;
    
    std::ostringstream sql;
    
    if (m_dbType == DatabaseType::kPostGIS) {
        sql << "SELECT COUNT(*) FROM spatial_ref_sys WHERE srid = " << srid;
    } else if (m_dbType == DatabaseType::kSpatiaLite) {
        sql << "SELECT COUNT(*) FROM spatial_ref_sys WHERE srid = " << srid;
    } else {
        isValid = (srid > 0 && srid < 1000000);
        return Result::Success();
    }
    
    DbResultSetPtr resultSet;
    Result result = m_connection->ExecuteQuery(sql.str(), resultSet);
    
    if (result.IsSuccess() && resultSet && resultSet->Next()) {
        isValid = resultSet->GetInt64(0) > 0;
    }
    
    return Result::Success();
}

Result SridManager::GetSridInfo(int srid, std::string& proj4, std::string& wkt, std::string& name) {
    proj4.clear();
    wkt.clear();
    name.clear();
    
    std::ostringstream sql;
    
    if (m_dbType == DatabaseType::kPostGIS) {
        sql << "SELECT proj4text, srtext, ref_sys_name "
            << "FROM spatial_ref_sys WHERE srid = " << srid;
    } else {
        return Result::Error(DbResult::kNotSupported, "Not supported for this database type");
    }
    
    DbResultSetPtr resultSet;
    Result result = m_connection->ExecuteQuery(sql.str(), resultSet);
    
    if (result.IsSuccess() && resultSet && resultSet->Next()) {
        proj4 = resultSet->GetString(0);
        wkt = resultSet->GetString(1);
        name = resultSet->GetString(2);
    }
    
    return Result::Success();
}

Result SridManager::Transform(const Geometry* geometry,
                            int fromSrid,
                            int toSrid,
                            GeometryPtr& outGeometry) {
    if (!geometry) {
        return Result::Error(DbResult::kInvalidGeometry, "Geometry is null");
    }
    
    if (fromSrid == toSrid) {
        outGeometry.reset(geometry->Clone());
        return Result::Success();
    }
    
    switch (m_dbType) {
        case DatabaseType::kPostGIS:
            return TransformPostGIS(geometry, fromSrid, toSrid, outGeometry);
        case DatabaseType::kSpatiaLite:
            return TransformSpatiaLite(geometry, fromSrid, toSrid, outGeometry);
        default:
            return Result::Error(DbResult::kNotSupported, "Unsupported database type");
    }
}

Result SridManager::TransformPostGIS(const Geometry* geometry,
                                      int fromSrid,
                                      int toSrid,
                                      GeometryPtr& outGeometry) {
    std::vector<uint8_t> wkb;
    WkbOptions options;
    options.includeSRID = true;
    
    Result wkbResult = WkbConverter::GeometryToWkb(geometry, wkb, options);
    if (!wkbResult.IsSuccess()) {
        return wkbResult;
    }
    
    std::string hexWkb;
    wkbResult = WkbConverter::GeometryToHexWkb(geometry, hexWkb, options);
    if (!wkbResult.IsSuccess()) {
        return wkbResult;
    }
    
    std::ostringstream sql;
    sql << "SELECT ST_AsBinary(ST_Transform(ST_GeomFromEWKB(X'" << hexWkb << "'), " << toSrid << "))";
    
    DbResultSetPtr resultSet;
    Result result = m_connection->ExecuteQuery(sql.str(), resultSet);
    
    if (!result.IsSuccess() || !resultSet || resultSet->IsEOF()) {
        return Result::Error(DbResult::kExecutionError, "Transform failed");
    }
    
    return Result::Success();
}

Result SridManager::TransformSpatiaLite(const Geometry* geometry,
                                        int fromSrid,
                                        int toSrid,
                                        GeometryPtr& outGeometry) {
    return TransformPostGIS(geometry, fromSrid, toSrid, outGeometry);
}

Result SridManager::GetTransformSQL(const std::string& geometryColumn,
                                    int fromSrid,
                                    int toSrid,
                                    std::string& sql) {
    std::ostringstream oss;
    
    if (m_dbType == DatabaseType::kPostGIS) {
        oss << "ST_Transform(" << geometryColumn << ", " << toSrid << ")";
    } else if (m_dbType == DatabaseType::kSpatiaLite) {
        oss << "Transform(" << geometryColumn << ", " << toSrid << ")";
    } else {
        return Result::Error(DbResult::kNotSupported, "Unsupported database type");
    }
    
    sql = oss.str();
    return Result::Success();
}

Result SridManager::ImportFromEPSG(int epsgCode, int& srid) {
    srid = epsgCode;
    
    bool isValid = false;
    return ValidateSRID(srid, isValid);
}

Result SridManager::ExportToEPSG(int srid, int& epsgCode) {
    epsgCode = srid;
    return Result::Success();
}

bool SridManager::IsGeographic(int srid) {
    return srid == 4326 || srid == 4322 || srid == 4267 || srid == 4269;
}

bool SridManager::IsProjected(int srid) {
    return srid != 4326 && srid != 4322 && srid != 4267 && srid != 4269 && srid > 0 && srid < 100000;
}

}
}
