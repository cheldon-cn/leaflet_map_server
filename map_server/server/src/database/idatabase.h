#ifndef CYCLE_DATABASE_IDATABASE_H
#define CYCLE_DATABASE_IDATABASE_H

#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include "../config/config.h"

namespace cycle {
namespace database {

struct SqlParameter {
    enum Type { INTEGER, REAL, TEXT, BLOB, NULL_VALUE };
    
    Type type;
    union {
        int int_value;
        double real_value;
        const char* text_value;
        struct {
            const void* blob_data;
            size_t blob_size;
        };
    };
    
    static SqlParameter Integer(int value) {
        SqlParameter p;
        p.type = INTEGER;
        p.int_value = value;
        return p;
    }
    
    static SqlParameter Real(double value) {
        SqlParameter p;
        p.type = REAL;
        p.real_value = value;
        return p;
    }
    
    static SqlParameter Text(const char* value) {
        SqlParameter p;
        p.type = TEXT;
        p.text_value = value;
        return p;
    }
    
    static SqlParameter Blob(const void* data, size_t size) {
        SqlParameter p;
        p.type = BLOB;
        p.blob_data = data;
        p.blob_size = size;
        return p;
    }
    
    static SqlParameter Null() {
        SqlParameter p;
        p.type = NULL_VALUE;
        return p;
    }
};

class DatabaseRow {
public:
    virtual ~DatabaseRow() = default;
    
    virtual int GetColumnCount() const = 0;
    virtual std::string GetColumnName(int index) const = 0;
    
    virtual int GetInt(int index) const = 0;
    virtual double GetDouble(int index) const = 0;
    virtual std::string GetString(int index) const = 0;
    virtual std::vector<uint8_t> GetBlob(int index) const = 0;
    virtual bool IsNull(int index) const = 0;
};

class ResultSet {
public:
    virtual ~ResultSet() = default;
    
    virtual bool Next() = 0;
    virtual std::unique_ptr<DatabaseRow> GetCurrentRow() const = 0;
    virtual int GetRowCount() const = 0;
    virtual void Close() = 0;
};

class IDatabase {
public:
    virtual ~IDatabase() = default;
    
    virtual bool Open() = 0;
    virtual void Close() = 0;
    virtual bool IsOpen() const = 0;
    
    virtual bool Execute(const std::string& sql) = 0;
    virtual bool Execute(const std::string& sql, const std::vector<SqlParameter>& params) = 0;
    
    virtual std::unique_ptr<ResultSet> Query(const std::string& sql) = 0;
    virtual std::unique_ptr<ResultSet> Query(const std::string& sql, 
                                             const std::vector<SqlParameter>& params) = 0;
    
    virtual bool BeginTransaction() = 0;
    virtual bool CommitTransaction() = 0;
    virtual bool RollbackTransaction() = 0;
    
    virtual std::string GetLastError() const = 0;
    virtual int GetLastErrorCode() const = 0;
    
    virtual std::unique_ptr<ResultSet> QuerySpatial(
        const std::string& table,
        const BoundingBox& envelope,
        const std::string& geometryColumn = "geometry") = 0;
    
    virtual std::string GetDatabaseType() const = 0;
};

} // namespace database
} // namespace cycle

#endif // CYCLE_DATABASE_IDATABASE_H
