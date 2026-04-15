#pragma once

/**
 * @file field_value.h
 * @brief 字段值容�? */

#include "export.h"
#include "ogc/feature/field_type.h"
#include "ogc/feature/datetime.h"
#include "ogc/geom/geometry.h"
#include <cstdint>
#include <string>
#include <vector>
#include <memory>

namespace ogc {

class OGC_FEATURE_API CNFieldValue {
public:
    CNFieldValue();
    
    explicit CNFieldValue(CNFieldType type);
    
    CNFieldValue(int32_t value);
    CNFieldValue(int64_t value);
    CNFieldValue(double value);
    CNFieldValue(bool value);
    CNFieldValue(const char* value);
    CNFieldValue(const std::string& value);
    CNFieldValue(const CNDateTime& value);
    CNFieldValue(const std::vector<uint8_t>& value);
    CNFieldValue(const std::vector<int32_t>& value);
    CNFieldValue(const std::vector<int64_t>& value);
    CNFieldValue(const std::vector<double>& value);
    CNFieldValue(const std::vector<std::string>& value);
    CNFieldValue(GeometryPtr value);
    
    CNFieldValue(const CNFieldValue& other);
    CNFieldValue(CNFieldValue&& other) noexcept;
    
    CNFieldValue& operator=(const CNFieldValue& other);
    CNFieldValue& operator=(CNFieldValue&& other) noexcept;
    
    ~CNFieldValue();
    
    CNFieldType GetType() const;
    
    bool IsNull() const;
    bool IsUnset() const;
    bool IsSet() const;
    
    void SetNull();
    void Unset();
    
    int32_t GetInteger() const;
    int64_t GetInteger64() const;
    double GetReal() const;
    bool GetBoolean() const;
    std::string GetString() const;
    CNDateTime GetDateTime() const;
    std::vector<uint8_t> GetBinary() const;
    std::vector<int32_t> GetIntegerList() const;
    std::vector<int64_t> GetInteger64List() const;
    std::vector<double> GetRealList() const;
    std::vector<std::string> GetStringList() const;
    GeometryPtr GetGeometry() const;
    
    void SetInteger(int32_t value);
    void SetInteger64(int64_t value);
    void SetReal(double value);
    void SetBoolean(bool value);
    void SetString(const char* value);
    void SetString(const std::string& value);
    void SetDateTime(const CNDateTime& value);
    void SetBinary(const std::vector<uint8_t>& value);
    void SetIntegerList(const std::vector<int32_t>& value);
    void SetInteger64List(const std::vector<int64_t>& value);
    void SetRealList(const std::vector<double>& value);
    void SetStringList(const std::vector<std::string>& value);
    void SetGeometry(GeometryPtr value);
    
    void Swap(CNFieldValue& other) noexcept;
    
    bool TryGetInteger(int32_t& out) const;
    bool TryGetInteger64(int64_t& out) const;
    bool TryGetReal(double& out) const;
    bool TryGetString(std::string& out) const;
    bool TryGetDateTime(CNDateTime& out) const;
    
    bool ConvertToInteger(int32_t& out) const;
    bool ConvertToInteger64(int64_t& out) const;
    bool ConvertToReal(double& out) const;
    bool ConvertToString(std::string& out) const;
    bool ConvertToDateTime(CNDateTime& out) const;
    
private:
    void Clear();
    
    enum class StorageType : uint8_t {
        kNone,
        kInteger,
        kInteger64,
        kReal,
        kBoolean,
        kString,
        kDateTime,
        kBinary,
        kIntegerList,
        kInteger64List,
        kRealList,
        kStringList,
        kGeometry
    };
    
    struct Storage {
        uint8_t buffer[32];
        
        Storage() { std::memset(buffer, 0, sizeof(buffer)); }
        ~Storage() {}
    };
    
    CNFieldType type_;
    StorageType storage_type_;
    Storage storage_;
    std::string* str_ptr_;
    CNDateTime* dt_ptr_;
    std::vector<uint8_t>* bin_vec_;
    std::vector<int32_t>* i32_vec_;
    std::vector<int64_t>* i64_vec_;
    std::vector<double>* d_vec_;
    std::vector<std::string>* str_vec_;
    GeometryPtr* geom_ptr_;
};

} // namespace ogc
