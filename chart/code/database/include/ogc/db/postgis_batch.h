#pragma once

#include "common.h"
#include "connection.h"
#include "result.h"
#include "statement.h"
#include "resultset.h"
#include "wkb_converter.h"
#include "postgis_connection.h"
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>

namespace ogc {
namespace db {

struct BatchInsertOptions {
    int batchSize = 100;
    bool useCopy = false;
    bool continueOnError = false;
    std::string returningClause = "RETURNING id";
};

struct BatchInsertResult {
    int successCount = 0;
    int failureCount = 0;
    std::vector<int64_t> insertedIds;
    std::vector<std::string> errors;
};

struct SpatialQueryOptions {
    SpatialOperator operation = SpatialOperator::kIntersects;
    bool useIndex = true;
    bool includeBoundary = false;
    int limit = -1;
    int offset = 0;
    std::string orderBy;
};

using SpatialQueryCallback = std::function<bool(int64_t id, const Geometry* geometry, void* userData)>;

class OGC_DB_API PostGISBatchInserter {
public:
    explicit PostGISBatchInserter(PostGISConnection* connection);
    ~PostGISBatchInserter();
    
    Result Initialize(const std::string& table, 
                      const std::string& geomColumn,
                      const std::vector<std::string>& attributeColumns);
    
    Result AddGeometry(const Geometry* geometry,
                      const std::vector<std::string>& attributeValues);
    
    Result Flush();
    
    Result Finalize(BatchInsertResult& result);
    
    void SetOptions(const BatchInsertOptions& options);
    
private:
    PostGISConnection* m_connection;
    std::string m_table;
    std::string m_geomColumn;
    std::vector<std::string> m_attributeColumns;
    BatchInsertOptions m_options;
    
    std::vector<std::pair<const Geometry*, std::vector<std::string>>> m_batch;
    
    Result PrepareCopy();
    Result ExecuteCopy();
    Result ExecuteInsert();
};

class OGC_DB_API PostGISSpatialQuery {
public:
    explicit PostGISSpatialQuery(PostGISConnection* connection);
    ~PostGISSpatialQuery();
    
    Result Initialize(const std::string& table,
                     const std::string& geomColumn,
                     const std::string& idColumn = "id");
    
    Result Execute(const Geometry* filterGeometry,
                  const SpatialQueryOptions& options,
                  SpatialQueryCallback callback,
                  void* userData = nullptr);
    
    Result ExecuteBoundingBox(double minX, double minY, double maxX, double maxY,
                              const SpatialQueryOptions& options,
                              SpatialQueryCallback callback,
                              void* userData = nullptr);
    
    Result ExecuteRadius(double centerX, double centerY, double radius,
                        const SpatialQueryOptions& options,
                        SpatialQueryCallback callback,
                        void* userData = nullptr);
    
    Result ExecuteKNN(double x, double y, int k,
                      const SpatialQueryOptions& options,
                      SpatialQueryCallback callback,
                      void* userData = nullptr);

private:
    PostGISConnection* m_connection;
    std::string m_table;
    std::string m_geomColumn;
    std::string m_idColumn;
    
    std::string BuildSpatialQuery(const Geometry* filterGeometry,
                                  const SpatialQueryOptions& options);
    
    std::string BuildBoundingBoxQuery(double minX, double minY, double maxX, double maxY,
                                      const SpatialQueryOptions& options);
    
    std::string BuildRadiusQuery(double centerX, double centerY, double radius,
                                const SpatialQueryOptions& options);
    
    std::string BuildKNNQuery(double x, double y, int k,
                             const SpatialQueryOptions& options);
    
    std::string GetSpatialOperatorString(SpatialOperator op) const;
};

}
}
