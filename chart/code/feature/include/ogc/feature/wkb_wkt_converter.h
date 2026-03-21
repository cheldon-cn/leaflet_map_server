#pragma once

/**
 * @file wkb_wkt_converter.h
 * @brief WKB/WKT序列�? */

#include "export.h"
#include "ogc/feature/feature.h"
#include "ogc/feature/feature_collection.h"
#include <memory>
#include <string>
#include <vector>

namespace ogc {

enum class ByteOrder {
    kXDR,
    kNDR
};

class OGC_FEATURE_API CNWkbWktConverter {
public:
    CNWkbWktConverter();
    ~CNWkbWktConverter();
    
    std::string ToWKT(const CNFeature* feature);
    std::string ToWKT(const CNFeatureCollection* collection);
    
    std::vector<uint8_t> ToWKB(CNFeature* feature);
    std::vector<uint8_t> ToWKB(GeometryPtr geometry);
    
    CNFeature* FromWKB(const std::vector<uint8_t>& wkb);
    CNFeature* FromWKT(const std::string& wkt);
    
    void SetByteOrder(ByteOrder order);
    ByteOrder GetByteOrder() const;
    
    void SetIncludeSRID(bool include);
    bool GetIncludeSRID() const;
    
private:
    std::string GeometryToWKT(GeometryPtr geometry);
    std::vector<uint8_t> GeometryToWKB(GeometryPtr geometry);
    
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace ogc
