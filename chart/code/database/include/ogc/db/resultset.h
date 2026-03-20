#pragma once

#include "common.h"
#include "result.h"
#include <memory>
#include <string>
#include <vector>
#include <cstdint>

namespace ogc {
    class Geometry;
}

namespace ogc {
namespace db {

using GeometryPtr = std::unique_ptr<Geometry>;

class OGC_DB_API DbResultSet {
public:
    virtual ~DbResultSet() = default;
    
    DbResultSet(const DbResultSet&) = delete;
    DbResultSet& operator=(const DbResultSet&) = delete;
    
    DbResultSet(DbResultSet&&) noexcept = default;
    DbResultSet& operator=(DbResultSet&&) noexcept = default;
    
    virtual bool Next() = 0;
    virtual bool IsEOF() const = 0;
    virtual void Reset() = 0;
    
    virtual int GetColumnCount() const = 0;
    virtual std::string GetColumnName(int columnIndex) const = 0;
    virtual ColumnType GetColumnType(int columnIndex) const = 0;
    virtual int GetColumnIndex(const std::string& columnName) const = 0;
    
    virtual bool IsNull(int columnIndex) const = 0;
    virtual bool IsNull(const std::string& columnName) const = 0;
    
    virtual int32_t GetInt(int columnIndex) const = 0;
    virtual int32_t GetInt(const std::string& columnName) const = 0;
    virtual int64_t GetInt64(int columnIndex) const = 0;
    virtual int64_t GetInt64(const std::string& columnName) const = 0;
    virtual double GetDouble(int columnIndex) const = 0;
    virtual double GetDouble(const std::string& columnName) const = 0;
    virtual std::string GetString(int columnIndex) const = 0;
    virtual std::string GetString(const std::string& columnName) const = 0;
    virtual bool GetBool(int columnIndex) const = 0;
    virtual bool GetBool(const std::string& columnName) const = 0;
    virtual std::vector<uint8_t> GetBlob(int columnIndex) const = 0;
    virtual std::vector<uint8_t> GetBlob(const std::string& columnName) const = 0;
    
    virtual GeometryPtr GetGeometry(int columnIndex) const = 0;
    virtual GeometryPtr GetGeometry(const std::string& columnName) const = 0;
    
    virtual int64_t GetRowCount() const = 0;
    virtual int64_t GetCurrentRow() const = 0;
    
    virtual Result GetLastError() const = 0;

protected:
    DbResultSet() = default;
};

using DbResultSetPtr = std::unique_ptr<DbResultSet>;

}
}
