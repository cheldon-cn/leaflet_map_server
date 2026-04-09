#pragma once

/**
 * @file geom_field_defn.h
 * @brief 几何字段定义接口
 */

#include "ogc/feature/export.h"
#include "ogc/geom/geometry.h"
#include <cstddef>
#include <string>

namespace ogc {

class OGC_FEATURE_API CNGeomFieldDefn {
public:
    CNGeomFieldDefn();
    virtual ~CNGeomFieldDefn();
    
    virtual const char* GetName() const = 0;
    virtual void SetName(const char* name) = 0;
    
    virtual GeomType GetGeomType() const = 0;
    virtual void SetGeomType(GeomType type) = 0;
    
    virtual void* GetSpatialRef() const = 0;
    virtual void SetSpatialRef(void* sr) = 0;
    
    virtual bool IsNullable() const = 0;
    virtual void SetNullable(bool nullable) = 0;
    
    virtual bool Is2D() const = 0;
    virtual void Set2D(bool is_2d) = 0;
    
    virtual bool IsMeasured() const = 0;
    virtual void SetMeasured(bool measured) = 0;
    
    virtual double GetXMin() const = 0;
    virtual double GetXMax() const = 0;
    virtual double GetYMin() const = 0;
    virtual double GetYMax() const = 0;
    virtual void SetExtent(double xmin, double xmax, double ymin, double ymax) = 0;
    
    virtual CNGeomFieldDefn* Clone() const = 0;
};

using CNGeomFieldDefnPtr = CNGeomFieldDefn*;

OGC_FEATURE_API CNGeomFieldDefn* CreateCNGeomFieldDefn();

OGC_FEATURE_API CNGeomFieldDefn* CreateCNGeomFieldDefn(const char* name);

} // namespace ogc
