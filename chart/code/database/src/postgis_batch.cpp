#include "ogc/db/postgis_batch.h"
#include "ogc/db/postgis_connection.h"
#include <sstream>
#include <algorithm>

namespace ogc {
namespace db {

PostGISBatchInserter::PostGISBatchInserter(PostGISConnection* connection)
    : m_connection(connection) {
}

PostGISBatchInserter::~PostGISBatchInserter() {
    if (!m_batch.empty()) {
        Flush();
    }
}

void PostGISBatchInserter::SetOptions(const BatchInsertOptions& options) {
    m_options = options;
}

Result PostGISBatchInserter::Initialize(const std::string& table,
                                       const std::string& geomColumn,
                                       const std::vector<std::string>& attributeColumns) {
    m_table = table;
    m_geomColumn = geomColumn;
    m_attributeColumns = attributeColumns;
    m_batch.clear();
    
    if (!m_connection || !m_connection->IsConnected()) {
        return Result::Error(DbResult::kNotConnected, "Not connected to database");
    }
    
    if (m_options.useCopy) {
        return PrepareCopy();
    }
    
    return Result::Success();
}

Result PostGISBatchInserter::AddGeometry(const Geometry* geometry,
                                        const std::vector<std::string>& attributeValues) {
    if (!geometry) {
        return Result::Error(DbResult::kInvalidGeometry, "Geometry is null");
    }
    
    if (attributeValues.size() != m_attributeColumns.size()) {
        return Result::Error(DbResult::kInvalidParameter, "Attribute count mismatch");
    }
    
    m_batch.emplace_back(geometry, attributeValues);
    
    if (static_cast<int>(m_batch.size()) >= m_options.batchSize) {
        return Flush();
    }
    
    return Result::Success();
}

Result PostGISBatchInserter::Flush() {
    if (m_batch.empty()) {
        return Result::Success();
    }
    
    if (m_options.useCopy) {
        return ExecuteCopy();
    }
    
    return ExecuteInsert();
}

Result PostGISBatchInserter::PrepareCopy() {
    std::ostringstream copyCommand;
    copyCommand << "COPY " << m_table << " (";
    
    copyCommand << m_geomColumn;
    for (const auto& col : m_attributeColumns) {
        copyCommand << ", " << col;
    }
    copyCommand << ") FROM STDIN WITH (FORMAT binary)";
    
    return m_connection->Execute(copyCommand.str());
}

Result PostGISBatchInserter::ExecuteCopy() {
    return Result::Success();
}

Result PostGISBatchInserter::ExecuteInsert() {
    std::ostringstream sql;
    sql << "INSERT INTO " << m_table << " (";
    
    sql << m_geomColumn;
    for (const auto& col : m_attributeColumns) {
        sql << ", " << col;
    }
    sql << ") VALUES ";
    
    for (size_t i = 0; i < m_batch.size(); ++i) {
        const auto& item = m_batch[i];
        const Geometry* geometry = item.first;
        const std::vector<std::string>& values = item.second;
        
        std::vector<uint8_t> wkb;
        WkbOptions options;
        options.includeSRID = true;
        
        Result wkbResult = WkbConverter::GeometryToWkb(geometry, wkb, options);
        if (!wkbResult.IsSuccess()) {
            if (!m_options.continueOnError) {
                return wkbResult;
            }
            continue;
        }
        
        std::string hexWkb;
        wkbResult = WkbConverter::GeometryToHexWkb(geometry, hexWkb, options);
        if (!wkbResult.IsSuccess()) {
            if (!m_options.continueOnError) {
                return wkbResult;
            }
            continue;
        }
        
        if (i > 0) sql << ", ";
        sql << "(ST_GeomFromEWKB('\\x" << hexWkb << "'::bytea)";
        
        for (const auto& val : values) {
            sql << ", '" << val << "'";
        }
        sql << ")";
    }
    
    if (!m_options.returningClause.empty()) {
        sql << " " << m_options.returningClause;
    }
    
    DbResultSetPtr resultSet;
    Result result = m_connection->ExecuteQuery(sql.str(), resultSet);
    
    if (!result.IsSuccess()) {
        if (!m_options.continueOnError) {
            return result;
        }
    }
    
    if (resultSet) {
        while (resultSet->Next()) {
        }
    }
    
    m_batch.clear();
    return Result::Success();
}

Result PostGISBatchInserter::Finalize(BatchInsertResult& result) {
    result.successCount = 0;
    result.failureCount = 0;
    result.insertedIds.clear();
    result.errors.clear();
    
    return Flush();
}

PostGISSpatialQuery::PostGISSpatialQuery(PostGISConnection* connection)
    : m_connection(connection) {
}

PostGISSpatialQuery::~PostGISSpatialQuery() {
}

Result PostGISSpatialQuery::Initialize(const std::string& table,
                                       const std::string& geomColumn,
                                       const std::string& idColumn) {
    m_table = table;
    m_geomColumn = geomColumn;
    m_idColumn = idColumn;
    
    if (!m_connection || !m_connection->IsConnected()) {
        return Result::Error(DbResult::kNotConnected, "Not connected to database");
    }
    
    return Result::Success();
}

Result PostGISSpatialQuery::Execute(const Geometry* filterGeometry,
                                    const SpatialQueryOptions& options,
                                    SpatialQueryCallback callback,
                                    void* userData) {
    if (!filterGeometry) {
        return Result::Error(DbResult::kInvalidGeometry, "Filter geometry is null");
    }
    
    std::string sql = BuildSpatialQuery(filterGeometry, options);
    
    DbResultSetPtr resultSet;
    Result result = m_connection->ExecuteQuery(sql, resultSet);
    
    if (!result.IsSuccess()) {
        return result;
    }
    
    int count = 0;
    while (resultSet->Next()) {
        int64_t id = resultSet->GetInt64(0);
        GeometryPtr geom = resultSet->GetGeometry(1);
        
        bool cont = callback(id, geom.get(), userData);
        if (!cont) break;
        
        if (options.limit > 0 && ++count >= options.limit) break;
    }
    
    return Result::Success();
}

Result PostGISSpatialQuery::ExecuteBoundingBox(double minX, double minY, double maxX, double maxY,
                                                const SpatialQueryOptions& options,
                                                SpatialQueryCallback callback,
                                                void* userData) {
    std::string sql = BuildBoundingBoxQuery(minX, minY, maxX, maxY, options);
    
    DbResultSetPtr resultSet;
    Result result = m_connection->ExecuteQuery(sql, resultSet);
    
    if (!result.IsSuccess()) {
        return result;
    }
    
    int count = 0;
    while (resultSet->Next()) {
        int64_t id = resultSet->GetInt64(0);
        GeometryPtr geom = resultSet->GetGeometry(1);
        
        bool cont = callback(id, geom.get(), userData);
        if (!cont) break;
        
        if (options.limit > 0 && ++count >= options.limit) break;
    }
    
    return Result::Success();
}

Result PostGISSpatialQuery::ExecuteRadius(double centerX, double centerY, double radius,
                                          const SpatialQueryOptions& options,
                                          SpatialQueryCallback callback,
                                          void* userData) {
    std::string sql = BuildRadiusQuery(centerX, centerY, radius, options);
    
    DbResultSetPtr resultSet;
    Result result = m_connection->ExecuteQuery(sql, resultSet);
    
    if (!result.IsSuccess()) {
        return result;
    }
    
    int count = 0;
    while (resultSet->Next()) {
        int64_t id = resultSet->GetInt64(0);
        GeometryPtr geom = resultSet->GetGeometry(1);
        
        bool cont = callback(id, geom.get(), userData);
        if (!cont) break;
        
        if (options.limit > 0 && ++count >= options.limit) break;
    }
    
    return Result::Success();
}

Result PostGISSpatialQuery::ExecuteKNN(double x, double y, int k,
                                       const SpatialQueryOptions& options,
                                       SpatialQueryCallback callback,
                                       void* userData) {
    std::string sql = BuildKNNQuery(x, y, k, options);
    
    DbResultSetPtr resultSet;
    Result result = m_connection->ExecuteQuery(sql, resultSet);
    
    if (!result.IsSuccess()) {
        return result;
    }
    
    int count = 0;
    while (resultSet->Next()) {
        int64_t id = resultSet->GetInt64(0);
        GeometryPtr geom = resultSet->GetGeometry(1);
        
        bool cont = callback(id, geom.get(), userData);
        if (!cont) break;
        
        if (options.limit > 0 && ++count >= options.limit) break;
    }
    
    return Result::Success();
}

std::string PostGISSpatialQuery::BuildSpatialQuery(const Geometry* filterGeometry,
                                                   const SpatialQueryOptions& options) {
    std::ostringstream sql;
    
    std::vector<uint8_t> wkb;
    WkbOptions wkbOptions;
    wkbOptions.includeSRID = true;
    WkbConverter::GeometryToWkb(filterGeometry, wkb, wkbOptions);
    
    std::string hexWkb;
    WkbConverter::GeometryToHexWkb(filterGeometry, hexWkb, wkbOptions);
    
    sql << "SELECT " << m_idColumn << ", " << m_geomColumn << " FROM " << m_table << " WHERE ";
    
    sql << GetSpatialOperatorString(options.operation) << "(" << m_geomColumn;
    sql << ", ST_GeomFromEWKB('\\x" << hexWkb << "'::bytea))";
    
    if (!options.useIndex) {
        sql << " = false";
    }
    
    if (options.limit > 0) {
        sql << " LIMIT " << options.limit;
    }
    
    if (options.offset > 0) {
        sql << " OFFSET " << options.offset;
    }
    
    if (!options.orderBy.empty()) {
        sql << " ORDER BY " << options.orderBy;
    }
    
    return sql.str();
}

std::string PostGISSpatialQuery::BuildBoundingBoxQuery(double minX, double minY, double maxX, double maxY,
                                                       const SpatialQueryOptions& options) {
    std::ostringstream sql;
    
    sql << "SELECT " << m_idColumn << ", " << m_geomColumn << " FROM " << m_table << " WHERE ";
    
    sql << m_geomColumn << " && ST_MakeEnvelope(";
    sql << minX << ", " << minY << ", " << maxX << ", " << maxY << ", 4326)";
    
    if (options.limit > 0) {
        sql << " LIMIT " << options.limit;
    }
    
    if (options.offset > 0) {
        sql << " OFFSET " << options.offset;
    }
    
    if (!options.orderBy.empty()) {
        sql << " ORDER BY " << options.orderBy;
    }
    
    return sql.str();
}

std::string PostGISSpatialQuery::BuildRadiusQuery(double centerX, double centerY, double radius,
                                                   const SpatialQueryOptions& options) {
    std::ostringstream sql;
    
    sql << "SELECT " << m_idColumn << ", " << m_geomColumn << " FROM " << m_table << " WHERE ";
    
    sql << "ST_DWithin(" << m_geomColumn << ", ST_Point(" << centerX << ", " << centerY << ")";
    sql << "::geography, " << radius << ")";
    
    if (options.limit > 0) {
        sql << " LIMIT " << options.limit;
    }
    
    if (options.offset > 0) {
        sql << " OFFSET " << options.offset;
    }
    
    if (!options.orderBy.empty()) {
        sql << " ORDER BY " << options.orderBy;
    }
    
    return sql.str();
}

std::string PostGISSpatialQuery::BuildKNNQuery(double x, double y, int k,
                                                const SpatialQueryOptions& options) {
    std::ostringstream sql;
    
    sql << "SELECT " << m_idColumn << ", " << m_geomColumn << " FROM " << m_table << " ORDER BY ";
    
    sql << m_geomColumn << " <-> ST_Point(" << x << ", " << y << ")";
    
    if (k > 0) {
        sql << " LIMIT " << k;
    }
    
    return sql.str();
}

std::string PostGISSpatialQuery::GetSpatialOperatorString(SpatialOperator op) const {
    switch (op) {
        case SpatialOperator::kIntersects: return "ST_Intersects";
        case SpatialOperator::kContains: return "ST_Contains";
        case SpatialOperator::kWithin: return "ST_Within";
        case SpatialOperator::kOverlaps: return "ST_Overlaps";
        case SpatialOperator::kTouches: return "ST_Touches";
        case SpatialOperator::kCrosses: return "ST_Crosses";
        case SpatialOperator::kDisjoint: return "ST_Disjoint";
        case SpatialOperator::kEquals: return "ST_Equals";
        default: return "ST_Intersects";
    }
}

}
}
