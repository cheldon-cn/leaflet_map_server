#pragma once

/**
 * @file feature_defn.h
 * @brief 要素定义接口
 */

#include "export.h"
#include "ogc/feature/field_defn.h"
#include "ogc/feature/geom_field_defn.h"
#include <cstddef>
#include <string>
#include <vector>
#include <memory>
#include <atomic>

namespace ogc {

class OGC_FEATURE_API CNFeatureDefn {
public:
    static CNFeatureDefn* Create(const char* name = nullptr);

    CNFeatureDefn();
    explicit CNFeatureDefn(const char* name);
    
    virtual ~CNFeatureDefn();
    
    const char* GetName() const;
    void SetName(const char* name);
    
    size_t GetFieldCount() const;
    CNFieldDefn* GetFieldDefn(size_t index) const;
    int GetFieldIndex(const char* name) const;
    int GetFieldIndex(const char* name, size_t name_length) const;
    
    void AddFieldDefn(CNFieldDefn* field, size_t occurrence = 1);
    void DeleteFieldDefn(size_t index);
    void ClearFieldDefns();
    
    size_t GetGeomFieldCount() const;
    CNGeomFieldDefn* GetGeomFieldDefn(size_t index) const;
    int GetGeomFieldIndex(const char* name) const;
    
    void AddGeomFieldDefn(CNGeomFieldDefn* field);
    void DeleteGeomFieldDefn(size_t index);
    void ClearGeomFieldDefns();
    
    int GetReferenceCount() const;
    void AddReference() const;
    void ReleaseReference() const;
    
    CNFeatureDefn* Clone() const;
    
    bool IsValid() const;
    
    std::string ToJSON() const;
    bool FromJSON(const std::string& json);
    
    std::string ToXML() const;
    bool FromXML(const std::string& xml);
    
private:
    CNFeatureDefn(const CNFeatureDefn& other);
    CNFeatureDefn& operator=(const CNFeatureDefn& other);
    
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

using CNFeatureDefnPtr = CNFeatureDefn*;

} // namespace ogc
