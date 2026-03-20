#pragma once

#include "common.h"
#include "connection.h"
#include "result.h"
#include <string>
#include <memory>
#include <vector>
#include <map>

namespace ogc {
namespace db {

enum class IndexStrategy {
    kAuto,
    kRTree,
    kQuadtree,
    kGrid,
    kGeohash,
    kNone
};

struct SpatialIndexConfig {
    IndexStrategy strategy = IndexStrategy::kAuto;
    int gridSize = 256;
    int maxDepth = 12;
    float fillFactor = 0.7f;
    bool useGeometricBounds = true;
};

struct SpatialIndexMetadata {
    std::string tableName;
    std::string geometryColumn;
    IndexStrategy strategy;
    bool isValid;
    int64_t rowCount;
    int64_t indexSizeBytes;
    double coverage;
    std::string createdAt;
    std::string lastRebuilt;
};

class OGC_DB_API SpatialIndexManager {
public:
    static std::unique_ptr<SpatialIndexManager> Create(DbConnection* connection);
    explicit SpatialIndexManager(DbConnection* connection);
    ~SpatialIndexManager();
    
    Result CreateIndex(const std::string& table,
                     const std::string& geometryColumn,
                     const SpatialIndexConfig& config = SpatialIndexConfig());
    
    Result DropIndex(const std::string& table, const std::string& geometryColumn);
    
    Result RebuildIndex(const std::string& table, const std::string& geometryColumn);
    
    Result GetMetadata(const std::string& table,
                     const std::string& geometryColumn,
                     SpatialIndexMetadata& metadata);
    
    Result AnalyzeIndex(const std::string& table,
                       const std::string& geometryColumn,
                       double& selectivity,
                       int64_t& estimatedRows);
    
    Result OptimizeIndex(const std::string& table, const std::string& geometryColumn);
    
    bool IndexExists(const std::string& table, const std::string& geometryColumn);
    
    std::vector<std::string> GetIndexedColumns(const std::string& table);

private:
    DbConnection* m_connection;
    DatabaseType m_dbType;
    
    Result CreatePostGISIndex(const std::string& table,
                             const std::string& geometryColumn,
                             const SpatialIndexConfig& config);
    
    Result CreateSpatiaLiteIndex(const std::string& table,
                                const std::string& geometryColumn,
                                const SpatialIndexConfig& config);
    
    Result DropPostGISIndex(const std::string& table, const std::string& geometryColumn);
    Result DropSpatiaLiteIndex(const std::string& table, const std::string& geometryColumn);
    
    IndexStrategy RecommendStrategy(const std::string& table,
                                   const std::string& geometryColumn) const;
};

class OGC_DB_API SridManager {
public:
    static std::unique_ptr<SridManager> Create(DbConnection* connection);
    explicit SridManager(DbConnection* connection);
    ~SridManager();
    
    Result ValidateSRID(int srid, bool& isValid);
    
    Result GetSridInfo(int srid, std::string& proj4, std::string& wkt, std::string& name);
    
    Result Transform(const Geometry* geometry,
                    int fromSrid,
                    int toSrid,
                    GeometryPtr& outGeometry);
    
    Result GetTransformSQL(const std::string& geometryColumn,
                          int fromSrid,
                          int toSrid,
                          std::string& sql);
    
    Result ImportFromEPSG(int epsgCode, int& srid);
    
    Result ExportToEPSG(int srid, int& epsgCode);
    
    static bool IsGeographic(int srid);
    static bool IsProjected(int srid);
    static int GetDefaultSrid() { return 4326; }

private:
    DbConnection* m_connection;
    DatabaseType m_dbType;
    
    Result TransformPostGIS(const Geometry* geometry,
                           int fromSrid,
                           int toSrid,
                           GeometryPtr& outGeometry);
    
    Result TransformSpatiaLite(const Geometry* geometry,
                              int fromSrid,
                              int toSrid,
                              GeometryPtr& outGeometry);
};

}
}
