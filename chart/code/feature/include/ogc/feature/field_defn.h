#pragma once

/**
 * @file field_defn.h
 * @brief 字段定义接口
 */

#include "ogc/feature/export.h"
#include "ogc/feature/field_type.h"
#include <cstddef>
#include <string>

namespace ogc {

class OGC_FEATURE_API CNFieldDefn {
public:
    CNFieldDefn();
    virtual ~CNFieldDefn();
    
    virtual const char* GetName() const = 0;
    virtual void SetName(const char* name) = 0;
    
    virtual CNFieldType GetType() const = 0;
    virtual void SetType(CNFieldType type) = 0;
    
    virtual CNFieldSubType GetSubType() const = 0;
    virtual void SetSubType(CNFieldSubType sub_type) = 0;
    
    virtual int GetWidth() const = 0;
    virtual void SetWidth(int width) = 0;
    
    virtual int GetPrecision() const = 0;
    virtual void SetPrecision(int precision) = 0;
    
    virtual bool IsNullable() const = 0;
    virtual void SetNullable(bool nullable) = 0;
    
    virtual bool IsUnique() const = 0;
    virtual void SetUnique(bool unique) = 0;
    
    virtual const char* GetDefaultValue() const = 0;
    virtual void SetDefaultValue(const char* value) = 0;
    
    virtual const char* GetAlternativeName() const = 0;
    virtual void SetAlternativeName(const char* name) = 0;
    
    virtual const char* GetComment() const = 0;
    virtual void SetComment(const char* comment) = 0;
    
    virtual const char* GetDomainName() const = 0;
    virtual void SetDomainName(const char* domain) = 0;
    
    virtual CNFieldDefn* Clone() const = 0;
};

using CNFieldDefnPtr = CNFieldDefn*;

OGC_FEATURE_API CNFieldDefn* CreateCNFieldDefn();

OGC_FEATURE_API CNFieldDefn* CreateCNFieldDefn(const char* name);

} // namespace ogc
