#pragma once

#include "common.h"
#include "connection.h"
#include "result.h"
#include "sqlite_connection.h"
#include <string>
#include <memory>
#include <vector>
#include <functional>

namespace ogc {
namespace db {

enum class SpatialIndexType {
    kRTree,
    kMbrCache,
    kNone
};

struct SpatialIndexInfo {
    std::string tableName;
    std::string geometryColumn;
    SpatialIndexType indexType;
    bool isValid;
    int nodeCount;
};

struct SpatialQueryOptions {
    SpatialOperator operation = SpatialOperator::kIntersects;
    bool useIndex = true;
    bool includeBoundary = false;
    int limit = -1;
    int offset = 0;
    std::string orderBy;
};

using SpatialQueryCallback = std::function<bool(int64_t rowid, const Geometry* geometry, void* userData)>;

class OGC_DB_API SpatiaLiteSpatialIndex {
public:
    explicit SpatiaLiteSpatialIndex(SpatiaLiteConnection* connection);
    ~SpatiaLiteSpatialIndex();
    
    Result CreateRTreeIndex(const std::string& table, 
                          const std::string& geomColumn,
                          const std::string& pkColumn = "rowid");
    
    Result CreateMbrCache(const std::string& table, const std::string& geomColumn);
    
    Result DropIndex(const std::string& table, const std::string& geomColumn);
    
    Result RebuildIndex(const std::string& table, const std::string& geomColumn);
    
    Result GetIndexInfo(const std::string& table, const std::string& geomColumn, SpatialIndexInfo& info);
    
    Result QueryByGeometry(const Geometry* filterGeometry,
                          const SpatialQueryOptions& options,
                          SpatialQueryCallback callback,
                          void* userData = nullptr);
    
    Result QueryByBoundingBox(double minX, double minY, double maxX, double maxY,
                             const SpatialQueryOptions& options,
                             SpatialQueryCallback callback,
                             void* userData = nullptr);
    
    Result QueryByRadius(double centerX, double centerY, double radius,
                        const SpatialQueryOptions& options,
                        SpatialQueryCallback callback,
                        void* userData = nullptr);
    
    Result QueryKNearest(double x, double y, int k,
                        const SpatialQueryOptions& options,
                        SpatialQueryCallback callback,
                        void* userData = nullptr);

private:
    SpatiaLiteConnection* m_connection;
    
    std::string BuildSpatialQuery(const Geometry* filterGeometry, const SpatialQueryOptions& options);
    std::string BuildBoundingBoxQuery(double minX, double minY, double maxX, double maxY, const SpatialQueryOptions& options);
    std::string BuildRadiusQuery(double centerX, double centerY, double radius, const SpatialQueryOptions& options);
    std::string BuildKNearestQuery(double x, double y, int k, const SpatialQueryOptions& options);
    
    std::string GetSpatialOperatorString(SpatialOperator op) const;
    bool CheckIndexExists(const std::string& table, const std::string& geomColumn);
};

class OGC_DB_API SpatiaLiteBatchInserter {
public:
    explicit SpatiaLiteBatchInserter(SpatiaLiteConnection* connection);
    ~SpatiaLiteBatchInserter();
    
    Result Initialize(const std::string& table,
                     const std::string& geomColumn,
                     const std::vector<std::string>& attributeColumns);
    
    Result AddGeometry(const Geometry* geometry,
                       const std::vector<std::string>& attributeValues);
    
    Result Flush();
    
    Result Finalize(int& totalInserted);
    
    void SetBatchSize(int batchSize) { m_batchSize = batchSize; }
    void SetUseTransaction(bool use) { m_useTransaction = use; }

private:
    SpatiaLiteConnection* m_connection;
    std::string m_table;
    std::string m_geomColumn;
    std::vector<std::string> m_attributeColumns;
    int m_batchSize;
    bool m_useTransaction;
    
    std::vector<std::pair<const Geometry*, std::vector<std::string>>> m_batch;
    
    Result ExecuteInsert();
};

}
}
