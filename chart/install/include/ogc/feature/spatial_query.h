#pragma once

/**
 * @file spatial_query.h
 * @brief 空间查询构建�? */

#include "export.h"
#include "ogc/geometry.h"
#include <memory>
#include <vector>
#include <string>

namespace ogc {

enum class SpatialOperation {
    kIntersects,
    kContains,
    kWithin,
    kOverlaps,
    kTouches,
    kCrosses,
    kDisjoint,
    kEquals
};

class OGC_FEATURE_API CNSpatialQuery {
public:
    CNSpatialQuery();
    ~CNSpatialQuery();
    
    CNSpatialQuery(const CNSpatialQuery& other) = delete;
    CNSpatialQuery& operator=(const CNSpatialQuery& other) = delete;
    CNSpatialQuery(CNSpatialQuery&& other) noexcept;
    CNSpatialQuery& operator=(CNSpatialQuery&& other) noexcept;
    
    CNSpatialQuery& SetGeometry(GeometryPtr geometry);
    CNSpatialQuery& SetOperation(SpatialOperation op);
    CNSpatialQuery& SetBufferDistance(double distance);
    CNSpatialQuery& SetSRID(int srid);
    
    GeometryPtr GetGeometry() const;
    SpatialOperation GetOperation() const;
    double GetBufferDistance() const;
    int GetSRID() const;
    
    std::string ToWKT() const;
    
    static CNSpatialQuery Intersects(GeometryPtr geometry);
    static CNSpatialQuery Contains(GeometryPtr geometry);
    static CNSpatialQuery Within(GeometryPtr geometry);
    static CNSpatialQuery Overlaps(GeometryPtr geometry);
    static CNSpatialQuery WithinDistance(GeometryPtr geometry, double distance);
    
private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace ogc
