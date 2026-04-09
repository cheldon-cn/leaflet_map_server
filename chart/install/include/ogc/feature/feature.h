#pragma once

/**
 * @file feature.h
 * @brief 核心要素�? */

#include "export.h"
#include "ogc/feature/feature_defn.h"
#include "ogc/feature/field_value.h"
#include "ogc/geom/geometry.h"
#include <cstdint>
#include <memory>
#include <vector>

namespace ogc {

class OGC_FEATURE_API CNFeature {
public:
    CNFeature();
    explicit CNFeature(CNFeatureDefn* definition);
    
    ~CNFeature();
    
    CNFeature(const CNFeature& other);
    CNFeature(CNFeature&& other) noexcept;
    
    CNFeature& operator=(const CNFeature& other);
    CNFeature& operator=(CNFeature&& other) noexcept;
    
    CNFeatureDefn* GetFeatureDefn() const;
    void SetFeatureDefn(CNFeatureDefn* definition);
    
    int64_t GetFID() const;
    void SetFID(int64_t fid);
    
    size_t GetFieldCount() const;
    CNFieldValue& GetField(size_t index);
    const CNFieldValue& GetField(size_t index) const;
    
    bool IsFieldSet(size_t index) const;
    bool IsFieldNull(size_t index) const;
    
    void SetField(size_t index, const CNFieldValue& value);
    void SetField(size_t index, CNFieldValue&& value);
    
    void SetFieldNull(size_t index);
    void UnsetField(size_t index);
    
    int32_t GetFieldAsInteger(size_t index) const;
    int64_t GetFieldAsInteger64(size_t index) const;
    double GetFieldAsReal(size_t index) const;
    std::string GetFieldAsString(size_t index) const;
    CNDateTime GetFieldAsDateTime(size_t index) const;
    std::vector<uint8_t> GetFieldAsBinary(size_t index) const;
    
    int32_t GetFieldAsInteger(const char* name) const;
    int64_t GetFieldAsInteger64(const char* name) const;
    double GetFieldAsReal(const char* name) const;
    std::string GetFieldAsString(const char* name) const;
    CNDateTime GetFieldAsDateTime(const char* name) const;
    std::vector<uint8_t> GetFieldAsBinary(const char* name) const;
    
    void SetFieldInteger(size_t index, int32_t value);
    void SetFieldInteger64(size_t index, int64_t value);
    void SetFieldReal(size_t index, double value);
    void SetFieldString(size_t index, const char* value);
    void SetFieldString(size_t index, const std::string& value);
    void SetFieldDateTime(size_t index, const CNDateTime& value);
    void SetFieldBinary(size_t index, const std::vector<uint8_t>& value);
    
    void SetFieldInteger(const char* name, int32_t value);
    void SetFieldInteger64(const char* name, int64_t value);
    void SetFieldReal(const char* name, double value);
    void SetFieldString(const char* name, const char* value);
    void SetFieldString(const char* name, const std::string& value);
    void SetFieldDateTime(const char* name, const CNDateTime& value);
    void SetFieldBinary(const char* name, const std::vector<uint8_t>& value);
    void SetFieldNull(const char* name);
    
    size_t GetGeomFieldCount() const;
    GeometryPtr GetGeometry(size_t index = 0) const;
    void SetGeometry(GeometryPtr geometry, size_t index = 0);
    
    GeometryPtr StealGeometry(size_t index = 0);
    
    std::unique_ptr<Envelope> GetEnvelope() const;
    
    CNFeature* Clone() const;
    
    bool Equal(const CNFeature& other) const;
    bool operator==(const CNFeature& other) const;
    bool operator!=(const CNFeature& other) const;
    
    void Swap(CNFeature& other) noexcept;
    
private:
    void EnsureFieldsInitialized();
    void EnsureGeomFieldsInitialized();
    
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

using CNFeaturePtr = CNFeature*;

} // namespace ogc
