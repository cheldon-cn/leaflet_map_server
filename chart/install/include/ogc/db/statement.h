#pragma once

#include "common.h"
#include "result.h"
#include <memory>
#include <string>
#include <vector>
#include <cstdint>

namespace ogc {
    class Geometry;
    class Envelope;
}

namespace ogc {
namespace db {

class DbResultSet;
using DbResultSetPtr = std::unique_ptr<DbResultSet>;

class OGC_DB_API DbStatement {
public:
    virtual ~DbStatement() = default;
    
    DbStatement(const DbStatement&) = delete;
    DbStatement& operator=(const DbStatement&) = delete;
    
    DbStatement(DbStatement&&) noexcept = default;
    DbStatement& operator=(DbStatement&&) noexcept = default;
    
    virtual const std::string& GetName() const = 0;
    virtual const std::string& GetSql() const = 0;
    
    virtual Result BindInt(int paramIndex, int32_t value) = 0;
    virtual Result BindInt64(int paramIndex, int64_t value) = 0;
    virtual Result BindDouble(int paramIndex, double value) = 0;
    virtual Result BindString(int paramIndex, const std::string& value) = 0;
    virtual Result BindBool(int paramIndex, bool value) = 0;
    virtual Result BindNull(int paramIndex) = 0;
    virtual Result BindBlob(int paramIndex, const std::vector<uint8_t>& data) = 0;
    virtual Result BindBlob(int paramIndex, const uint8_t* data, size_t size) = 0;
    virtual Result BindGeometry(int paramIndex, const Geometry* geometry) = 0;
    virtual Result BindEnvelope(int paramIndex, const ::ogc::Envelope& envelope) = 0;
    
    virtual Result BindInt(const std::string& paramName, int32_t value) = 0;
    virtual Result BindInt64(const std::string& paramName, int64_t value) = 0;
    virtual Result BindDouble(const std::string& paramName, double value) = 0;
    virtual Result BindString(const std::string& paramName, const std::string& value) = 0;
    virtual Result BindBool(const std::string& paramName, bool value) = 0;
    virtual Result BindNull(const std::string& paramName) = 0;
    virtual Result BindBlob(const std::string& paramName, const std::vector<uint8_t>& data) = 0;
    virtual Result BindGeometry(const std::string& paramName, const Geometry* geometry) = 0;
    
    virtual Result Execute() = 0;
    virtual Result ExecuteQuery(DbResultSetPtr& result) = 0;
    
    virtual Result Reset() = 0;
    virtual Result ClearBindings() = 0;
    
    virtual int64_t GetLastInsertId() const = 0;
    virtual int64_t GetRowsAffected() const = 0;
    
    virtual int GetParameterCount() const = 0;
    virtual int GetParameterIndex(const std::string& paramName) const = 0;

protected:
    DbStatement() = default;
};

using DbStatementPtr = std::unique_ptr<DbStatement>;

}
}
