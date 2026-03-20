#pragma once

#include "export.h"
#include <cstdint>
#include <string>

namespace ogc {
namespace db {

enum class DatabaseType : uint8_t {
    kUnknown = 0,
    kPostGIS = 1,
    kSpatiaLite = 2,
    kOracleSpatial = 3,
    kSQLServerSpatial = 4
};

enum class SpatialOperator : uint8_t {
    kIntersects = 0,
    kContains = 1,
    kWithin = 2,
    kTouches = 3,
    kCrosses = 4,
    kOverlaps = 5,
    kDisjoint = 6,
    kEquals = 7,
    kDWithin = 8
};

enum class DbResult : int32_t {
    kSuccess = 0,
    
    kConnectionFailed = 1,
    kConnectionLost = 2,
    kConnectionTimeout = 3,
    kConnectionPoolExhausted = 4,
    kInvalidConnectionString = 5,
    
    kSqlError = 100,
    kSyntaxError = 101,
    kTableNotFound = 102,
    kColumnNotFound = 103,
    kConstraintViolation = 104,
    kDuplicateKey = 105,
    kForeignKeyViolation = 106,
    kNullConstraintViolation = 107,
    
    kInvalidGeometry = 200,
    kGeometryParseError = 201,
    kSridMismatch = 202,
    kGeometryTypeMismatch = 203,
    kInvalidWkb = 204,
    kInvalidWkt = 205,
    kInvalidGeoJson = 206,
    kInvalidParameter = 207,
    
    kTransactionError = 300,
    kDeadlock = 301,
    kSerializationFailure = 302,
    kTransactionTimeout = 303,
    kNoActiveTransaction = 304,
    kNestedTransactionNotSupported = 305,
    
    kOutOfMemory = 400,
    kResourceExhausted = 401,
    kBufferOverflow = 402,
    kFileNotFound = 403,
    kPermissionDenied = 404,
    
    kNotSupported = 999,
    kNotImplemented = 1000,
    kUnknownError = 1001
};

enum class ColumnType : uint8_t {
    kUnknown = 0,
    kInteger = 1,
    kBigInt = 2,
    kReal = 3,
    kDouble = 4,
    kText = 5,
    kBlob = 6,
    kGeometry = 7,
    kBoolean = 8,
    kDate = 9,
    kDateTime = 10,
    kTimeStamp = 11
};

enum class TransactionIsolation : uint8_t {
    kReadUncommitted = 0,
    kReadCommitted = 1,
    kRepeatableRead = 2,
    kSerializable = 3
};

constexpr int DEFAULT_CONNECTION_TIMEOUT_MS = 30000;
constexpr int DEFAULT_QUERY_TIMEOUT_MS = 60000;
constexpr int DEFAULT_POOL_MIN_SIZE = 2;
constexpr int DEFAULT_POOL_MAX_SIZE = 16;
constexpr int DEFAULT_POOL_TIMEOUT_MS = 5000;

inline const char* GetDatabaseTypeName(DatabaseType type) noexcept {
    switch (type) {
        case DatabaseType::kPostGIS: return "PostGIS";
        case DatabaseType::kSpatiaLite: return "SpatiaLite";
        case DatabaseType::kOracleSpatial: return "OracleSpatial";
        case DatabaseType::kSQLServerSpatial: return "SQLServerSpatial";
        default: return "Unknown";
    }
}

inline const char* GetSpatialOperatorName(SpatialOperator op) noexcept {
    switch (op) {
        case SpatialOperator::kIntersects: return "ST_Intersects";
        case SpatialOperator::kContains: return "ST_Contains";
        case SpatialOperator::kWithin: return "ST_Within";
        case SpatialOperator::kTouches: return "ST_Touches";
        case SpatialOperator::kCrosses: return "ST_Crosses";
        case SpatialOperator::kOverlaps: return "ST_Overlaps";
        case SpatialOperator::kDisjoint: return "ST_Disjoint";
        case SpatialOperator::kEquals: return "ST_Equals";
        case SpatialOperator::kDWithin: return "ST_DWithin";
        default: return "Unknown";
    }
}

inline const char* GetDbResultString(DbResult result) noexcept {
    switch (result) {
        case DbResult::kSuccess: return "Success";
        case DbResult::kConnectionFailed: return "Connection failed";
        case DbResult::kConnectionLost: return "Connection lost";
        case DbResult::kConnectionTimeout: return "Connection timeout";
        case DbResult::kConnectionPoolExhausted: return "Connection pool exhausted";
        case DbResult::kInvalidConnectionString: return "Invalid connection string";
        case DbResult::kSqlError: return "SQL error";
        case DbResult::kSyntaxError: return "Syntax error";
        case DbResult::kTableNotFound: return "Table not found";
        case DbResult::kColumnNotFound: return "Column not found";
        case DbResult::kConstraintViolation: return "Constraint violation";
        case DbResult::kDuplicateKey: return "Duplicate key";
        case DbResult::kForeignKeyViolation: return "Foreign key violation";
        case DbResult::kNullConstraintViolation: return "Null constraint violation";
        case DbResult::kInvalidGeometry: return "Invalid geometry";
        case DbResult::kGeometryParseError: return "Geometry parse error";
        case DbResult::kSridMismatch: return "SRID mismatch";
        case DbResult::kGeometryTypeMismatch: return "Geometry type mismatch";
        case DbResult::kInvalidWkb: return "Invalid WKB";
        case DbResult::kInvalidWkt: return "Invalid WKT";
        case DbResult::kInvalidGeoJson: return "Invalid GeoJSON";
        case DbResult::kTransactionError: return "Transaction error";
        case DbResult::kDeadlock: return "Deadlock detected";
        case DbResult::kSerializationFailure: return "Serialization failure";
        case DbResult::kTransactionTimeout: return "Transaction timeout";
        case DbResult::kNoActiveTransaction: return "No active transaction";
        case DbResult::kNestedTransactionNotSupported: return "Nested transaction not supported";
        case DbResult::kOutOfMemory: return "Out of memory";
        case DbResult::kResourceExhausted: return "Resource exhausted";
        case DbResult::kBufferOverflow: return "Buffer overflow";
        case DbResult::kFileNotFound: return "File not found";
        case DbResult::kPermissionDenied: return "Permission denied";
        case DbResult::kNotSupported: return "Operation not supported";
        case DbResult::kNotImplemented: return "Operation not implemented";
        default: return "Unknown error";
    }
}

inline const char* GetColumnTypeName(ColumnType type) noexcept {
    switch (type) {
        case ColumnType::kInteger: return "INTEGER";
        case ColumnType::kBigInt: return "BIGINT";
        case ColumnType::kReal: return "REAL";
        case ColumnType::kDouble: return "DOUBLE";
        case ColumnType::kText: return "TEXT";
        case ColumnType::kBlob: return "BLOB";
        case ColumnType::kGeometry: return "GEOMETRY";
        case ColumnType::kBoolean: return "BOOLEAN";
        case ColumnType::kDate: return "DATE";
        case ColumnType::kDateTime: return "DATETIME";
        case ColumnType::kTimeStamp: return "TIMESTAMP";
        default: return "UNKNOWN";
    }
}

}
}
