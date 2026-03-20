#include "ogc/db/sqlite_spatial.h"
#include "ogc/db/sqlite_connection.h"
#include "ogc/db/wkb_converter.h"
#include <sstream>
#include <algorithm>

namespace ogc {
namespace db {

SpatiaLiteSpatialIndex::SpatiaLiteSpatialIndex(SpatiaLiteConnection* connection)
    : m_connection(connection) {
}

SpatiaLiteSpatialIndex::~SpatiaLiteSpatialIndex() {
}

Result SpatiaLiteSpatialIndex::CreateRTreeIndex(const std::string& table,
                                                const std::string& geomColumn,
                                                const std::string& pkColumn) {
    if (!m_connection || !m_connection->IsConnected()) {
        return Result::Error(DbResult::kNotConnected, "Not connected to database");
    }
    
    std::ostringstream sql;
    sql << "SELECT CreateSpatialIndex('" << table << "', '" << geomColumn << "')";
    
    Result result = m_connection->Execute(sql.str());
    
    if (!result.IsSuccess()) {
        sql.str("");
        sql << "CREATE VIRTUAL TABLE IF NOT EXISTS idx_" << table << "_" << geomColumn 
            << " USING rtree(id, minx, maxx, miny, maxy)";
        
        result = m_connection->Execute(sql.str());
        if (!result.IsSuccess()) {
            return result;
        }
        
        sql.str("");
        sql << "INSERT INTO idx_" << table << "_" << geomColumn 
            << "(id, minx, maxx, miny, maxy) "
            << "SELECT " << pkColumn << ", "
            << "ST_MinX(" << geomColumn << "), ST_MaxX(" << geomColumn << "), "
            << "ST_MinY(" << geomColumn << "), ST_MaxY(" << geomColumn << ") "
            << "FROM " << table;
        
        return m_connection->Execute(sql.str());
    }
    
    return Result::Success();
}

Result SpatiaLiteSpatialIndex::CreateMbrCache(const std::string& table, const std::string& geomColumn) {
    if (!m_connection || !m_connection->IsConnected()) {
        return Result::Error(DbResult::kNotConnected, "Not connected to database");
    }
    
    std::ostringstream sql;
    sql << "SELECT CreateMbrCache('" << table << "', '" << geomColumn << "')";
    
    return m_connection->Execute(sql.str());
}

Result SpatiaLiteSpatialIndex::DropIndex(const std::string& table, const std::string& geomColumn) {
    if (!m_connection || !m_connection->IsConnected()) {
        return Result::Error(DbResult::kNotConnected, "Not connected to database");
    }
    
    std::ostringstream sql;
    sql << "DROP TABLE IF EXISTS idx_" << table << "_" << geomColumn;
    
    return m_connection->Execute(sql.str());
}

Result SpatiaLiteSpatialIndex::RebuildIndex(const std::string& table, const std::string& geomColumn) {
    Result dropResult = DropIndex(table, geomColumn);
    if (!dropResult.IsSuccess() && dropResult.GetCode() != DbResult::kExecutionError) {
        return dropResult;
    }
    
    return CreateRTreeIndex(table, geomColumn);
}

Result SpatiaLiteSpatialIndex::GetIndexInfo(const std::string& table, 
                                            const std::string& geomColumn,
                                            SpatialIndexInfo& info) {
    info.tableName = table;
    info.geometryColumn = geomColumn;
    info.indexType = SpatialIndexType::kNone;
    info.isValid = false;
    info.nodeCount = 0;
    
    if (!m_connection || !m_connection->IsConnected()) {
        return Result::Error(DbResult::kNotConnected, "Not connected to database");
    }
    
    std::ostringstream sql;
    sql << "SELECT name FROM sqlite_master WHERE type='table' AND name='idx_" 
        << table << "_" << geomColumn << "'";
    
    DbResultSetPtr resultSet;
    Result result = m_connection->ExecuteQuery(sql.str(), resultSet);
    
    if (!result.IsSuccess() || !resultSet || resultSet->IsEOF()) {
        return Result::Success();
    }
    
    info.isValid = true;
    info.indexType = SpatialIndexType::kRTree;
    
    return Result::Success();
}

Result SpatiaLiteSpatialIndex::QueryByGeometry(const Geometry* filterGeometry,
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
        int64_t rowid = resultSet->GetInt64(0);
        GeometryPtr geom = resultSet->GetGeometry(1);
        
        bool cont = callback(rowid, geom.get(), userData);
        if (!cont) break;
        
        if (options.limit > 0 && ++count >= options.limit) break;
    }
    
    return Result::Success();
}

Result SpatiaLiteSpatialIndex::QueryByBoundingBox(double minX, double minY, double maxX, double maxY,
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
        int64_t rowid = resultSet->GetInt64(0);
        GeometryPtr geom = resultSet->GetGeometry(1);
        
        bool cont = callback(rowid, geom.get(), userData);
        if (!cont) break;
        
        if (options.limit > 0 && ++count >= options.limit) break;
    }
    
    return Result::Success();
}

Result SpatiaLiteSpatialIndex::QueryByRadius(double centerX, double centerY, double radius,
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
        int64_t rowid = resultSet->GetInt64(0);
        GeometryPtr geom = resultSet->GetGeometry(1);
        
        bool cont = callback(rowid, geom.get(), userData);
        if (!cont) break;
        
        if (options.limit > 0 && ++count >= options.limit) break;
    }
    
    return Result::Success();
}

Result SpatiaLiteSpatialIndex::QueryKNearest(double x, double y, int k,
                                              const SpatialQueryOptions& options,
                                              SpatialQueryCallback callback,
                                              void* userData) {
    std::string sql = BuildKNearestQuery(x, y, k, options);
    
    DbResultSetPtr resultSet;
    Result result = m_connection->ExecuteQuery(sql, resultSet);
    
    if (!result.IsSuccess()) {
        return result;
    }
    
    int count = 0;
    while (resultSet->Next()) {
        int64_t rowid = resultSet->GetInt64(0);
        GeometryPtr geom = resultSet->GetGeometry(1);
        
        bool cont = callback(rowid, geom.get(), userData);
        if (!cont) break;
        
        if (options.limit > 0 && ++count >= options.limit) break;
    }
    
    return Result::Success();
}

std::string SpatiaLiteSpatialIndex::BuildSpatialQuery(const Geometry* filterGeometry,
                                                       const SpatialQueryOptions& options) {
    std::ostringstream sql;
    
    std::vector<uint8_t> wkb;
    WkbOptions wkbOptions;
    wkbOptions.includeSRID = true;
    WkbConverter::GeometryToWkb(filterGeometry, wkb, wkbOptions);
    
    std::string hexWkb;
    WkbConverter::GeometryToHexWkb(filterGeometry, hexWkb, wkbOptions);
    
    sql << "SELECT rowid, geometry FROM SpatialIndex WHERE f_table_name = '' AND ";
    sql << GetSpatialOperatorString(options.operation) << "(geometry, GeomFromEWKB(X'" << hexWkb << "'))";
    
    return sql.str();
}

std::string SpatiaLiteSpatialIndex::BuildBoundingBoxQuery(double minX, double minY, double maxX, double maxY,
                                                           const SpatialQueryOptions& options) {
    std::ostringstream sql;
    
    sql << "SELECT rowid, geometry FROM SpatialIndex WHERE ";
    sql << "mbr=FilterMbrIntersects(" << minX << ", " << minY << ", " << maxX << ", " << maxY << ")";
    
    if (options.limit > 0) {
        sql << " LIMIT " << options.limit;
    }
    
    return sql.str();
}

std::string SpatiaLiteSpatialIndex::BuildRadiusQuery(double centerX, double centerY, double radius,
                                                     const SpatialQueryOptions& options) {
    std::ostringstream sql;
    
    sql << "SELECT rowid, geometry FROM SpatialIndex WHERE ";
    sql << "mbr=FilterMbrWithin(" << centerX << ", " << centerY << ", " << radius << ")";
    
    if (options.limit > 0) {
        sql << " LIMIT " << options.limit;
    }
    
    return sql.str();
}

std::string SpatiaLiteSpatialIndex::BuildKNearestQuery(double x, double y, int k,
                                                        const SpatialQueryOptions& options) {
    std::ostringstream sql;
    
    sql << "SELECT rowid, geometry FROM SpatialIndex ORDER BY Distance(geometry, MakePoint(" 
        << x << ", " << y << "))";
    
    if (k > 0) {
        sql << " LIMIT " << k;
    }
    
    return sql.str();
}

std::string SpatiaLiteSpatialIndex::GetSpatialOperatorString(SpatialOperator op) const {
    switch (op) {
        case SpatialOperator::kIntersects: return "Intersects";
        case SpatialOperator::kContains: return "Contains";
        case SpatialOperator::kWithin: return "Within";
        case SpatialOperator::kOverlaps: return "Overlaps";
        case SpatialOperator::kTouches: return "Touches";
        case SpatialOperator::kCrosses: return "Crosses";
        case SpatialOperator::kDisjoint: return "Disjoint";
        case SpatialOperator::kEquals: return "Equals";
        default: return "Intersects";
    }
}

bool SpatiaLiteSpatialIndex::CheckIndexExists(const std::string& table, const std::string& geomColumn) {
    std::ostringstream sql;
    sql << "SELECT name FROM sqlite_master WHERE type='table' AND name='idx_" 
        << table << "_" << geomColumn << "'";
    
    DbResultSetPtr resultSet;
    Result result = m_connection->ExecuteQuery(sql.str(), resultSet);
    
    return result.IsSuccess() && resultSet && !resultSet->IsEOF();
}

SpatiaLiteBatchInserter::SpatiaLiteBatchInserter(SpatiaLiteConnection* connection)
    : m_connection(connection), m_batchSize(100), m_useTransaction(true) {
}

SpatiaLiteBatchInserter::~SpatiaLiteBatchInserter() {
    if (!m_batch.empty()) {
        Flush();
    }
}

Result SpatiaLiteBatchInserter::Initialize(const std::string& table,
                                           const std::string& geomColumn,
                                           const std::vector<std::string>& attributeColumns) {
    m_table = table;
    m_geomColumn = geomColumn;
    m_attributeColumns = attributeColumns;
    m_batch.clear();
    
    if (!m_connection || !m_connection->IsConnected()) {
        return Result::Error(DbResult::kNotConnected, "Not connected to database");
    }
    
    return Result::Success();
}

Result SpatiaLiteBatchInserter::AddGeometry(const Geometry* geometry,
                                            const std::vector<std::string>& attributeValues) {
    if (!geometry) {
        return Result::Error(DbResult::kInvalidGeometry, "Geometry is null");
    }
    
    if (attributeValues.size() != m_attributeColumns.size()) {
        return Result::Error(DbResult::kInvalidParameter, "Attribute count mismatch");
    }
    
    m_batch.emplace_back(geometry, attributeValues);
    
    if (static_cast<int>(m_batch.size()) >= m_batchSize) {
        return Flush();
    }
    
    return Result::Success();
}

Result SpatiaLiteBatchInserter::Flush() {
    if (m_batch.empty()) {
        return Result::Success();
    }
    
    return ExecuteInsert();
}

Result SpatiaLiteBatchInserter::ExecuteInsert() {
    if (m_useTransaction) {
        Result beginResult = m_connection->BeginTransaction();
        if (!beginResult.IsSuccess()) {
            return beginResult;
        }
    }
    
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
            if (m_useTransaction) m_connection->Rollback();
            return wkbResult;
        }
        
        std::string hexWkb;
        wkbResult = WkbConverter::GeometryToHexWkb(geometry, hexWkb, options);
        if (!wkbResult.IsSuccess()) {
            if (m_useTransaction) m_connection->Rollback();
            return wkbResult;
        }
        
        if (i > 0) sql << ", ";
        sql << "(GeomFromEWKB(X'" << hexWkb << "')";
        
        for (const auto& val : values) {
            sql << ", '" << val << "'";
        }
        sql << ")";
    }
    
    Result result = m_connection->Execute(sql.str());
    
    if (!result.IsSuccess()) {
        if (m_useTransaction) m_connection->Rollback();
        m_batch.clear();
        return result;
    }
    
    if (m_useTransaction) {
        Result commitResult = m_connection->Commit();
        if (!commitResult.IsSuccess()) {
            m_batch.clear();
            return commitResult;
        }
    }
    
    m_batch.clear();
    return Result::Success();
}

Result SpatiaLiteBatchInserter::Finalize(int& totalInserted) {
    totalInserted = 0;
    
    if (m_batch.empty()) {
        return Result::Success();
    }
    
    Result result = Flush();
    if (result.IsSuccess()) {
        totalInserted = static_cast<int>(m_batch.size());
    }
    
    return result;
}

}
}
