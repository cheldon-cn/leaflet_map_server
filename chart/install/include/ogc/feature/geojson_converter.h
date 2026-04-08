#pragma once

/**
 * @file geojson_converter.h
 * @brief GeoJSON序列�? */

#include "export.h"
#include "ogc/feature/feature.h"
#include "ogc/feature/feature_collection.h"
#include <memory>
#include <string>
#include <vector>

namespace ogc {

class OGC_FEATURE_API CNGeoJSONConverter {
public:
    CNGeoJSONConverter();
    ~CNGeoJSONConverter();
    
    std::string ToGeoJSON(CNFeature* feature);
    std::string ToGeoJSON(const CNFeatureCollection* collection);
    
    CNFeature* FromGeoJSON(const std::string& json);
    CNFeatureCollection* FromGeoJSONToCollection(const std::string& json);
    
    void SetIndent(bool indent);
    bool GetIndent() const;
    
    void SetPrecision(int precision);
    int GetPrecision() const;
    
private:
    std::string FeatureToGeoJSON(const CNFeature* feature, bool as_collection);
    std::string GeometryToGeoJSON(GeometryPtr geometry);
    
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace ogc
