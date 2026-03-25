#pragma once

/**
 * @file common.h
 * @brief 公共定义头文件，包含几何类型枚举、维度定义、错误码和工具函数
 */

#include "export.h"
#include <cstdint>
#include <cmath>
#include <limits>
#include <string>

namespace ogc {

/**
 * @brief 几何类型枚举，遵循OGC Simple Feature Access规范
 */
enum class GeomType : uint8_t {
    kUnknown = 0,
    kPoint = 1,
    kLineString = 2,
    kPolygon = 3,
    kMultiPoint = 4,
    kMultiLineString = 5,
    kMultiPolygon = 6,
    kGeometryCollection = 7,
    kCircularString = 8,
    kCompoundCurve = 9,
    kCurvePolygon = 10,
    kMultiCurve = 11,
    kMultiSurface = 12,
    kPolyhedralSurface = 13,
    kTIN = 14,
    kTriangle = 15
};

/**
 * @brief 几何维度枚举
 */
enum class Dimension : uint8_t {
    Unknown = 255,
    Point = 0,
    Curve = 1,
    Surface = 2
};

/**
 * @brief 几何操作结果枚举
 */
enum class GeomResult : int32_t {
    kSuccess = 0,
    
    // 几何错误 (1-99)
    kInvalidGeometry = 1,
    kEmptyGeometry = 2,
    kNullGeometry = 3,
    kTopologyError = 4,
    kSelfIntersection = 5,
    kInvalidDimension = 6,
    kInvalidCoordinate = 7,
    kInvalidRing = 8,
    kNotClosedRing = 9,
    kDegenerateGeometry = 10,
    kInvalidPolygonShell = 11,
    kInvalidHole = 12,
    kHoleOutsideShell = 13,
    kNestedHoles = 14,
    kDisconnectedInterior = 15,
    kRepeatedPoint = 16,
    kTooFewPoints = 17,
    
    // 参数错误 (100-199)
    kInvalidParameters = 100,
    kOutOfRange = 101,
    kInvalidSRID = 102,
    kInvalidTolerance = 103,
    kNullPointer = 104,
    kInvalidIndex = 105,
    kInvalidWidth = 106,
    kInvalidPrecision = 107,
    kInvalidBufferSize = 108,
    
    // 操作错误 (200-299)
    kNotSupported = 200,
    kNotImplemented = 201,
    kOperationFailed = 202,
    kInvalidOperation = 203,
    kTypeMismatch = 204,
    kReadOnly = 205,
    
    // 资源错误 (300-399)
    kOutOfMemory = 300,
    kBufferOverflow = 301,
    kResourceExhausted = 302,
    
    // I/O错误 (400-499)
    kIOError = 400,
    kParseError = 401,
    kFormatError = 402,
    kFileNotFound = 403,
    kWriteFailed = 404,
    kReadFailed = 405,
    
    // 坐标转换错误 (500-599)
    kTransformError = 500,
    kCRSNotFound = 501,
    kCRSConversionError = 502,
    kInvalidProjection = 503,
    kCoordinateOutOfRange = 504
};

/**
 * @brief 默认几何计算容差
 */
constexpr double DEFAULT_TOLERANCE = 1e-9;

/**
 * @brief 默认WKT输出精度
 */
constexpr int DEFAULT_WKT_PRECISION = 15;

/**
 * @brief 圆周率常量
 */
constexpr double PI = 3.14159265358979323846;

/**
 * @brief 获取几何类型名称字符串
 * @param type 几何类型枚举
 * @return 类型名称字符串
 */
inline const char* GetGeomTypeName(GeomType type) noexcept {
    switch (type) {
        case GeomType::kPoint: return "Point";
        case GeomType::kLineString: return "LineString";
        case GeomType::kPolygon: return "Polygon";
        case GeomType::kMultiPoint: return "MultiPoint";
        case GeomType::kMultiLineString: return "MultiLineString";
        case GeomType::kMultiPolygon: return "MultiPolygon";
        case GeomType::kGeometryCollection: return "GeometryCollection";
        case GeomType::kCircularString: return "CircularString";
        case GeomType::kCompoundCurve: return "CompoundCurve";
        case GeomType::kCurvePolygon: return "CurvePolygon";
        case GeomType::kMultiCurve: return "MultiCurve";
        case GeomType::kMultiSurface: return "MultiSurface";
        case GeomType::kPolyhedralSurface: return "PolyhedralSurface";
        case GeomType::kTIN: return "TIN";
        case GeomType::kTriangle: return "Triangle";
        default: return "Unknown";
    }
}

/**
 * @brief 获取结果码描述字符串
 * @param result 结果码枚举
 * @return 描述字符串
 */
inline const char* GetResultString(GeomResult result) noexcept {
    switch (result) {
        case GeomResult::kSuccess: return "Success";
        case GeomResult::kInvalidGeometry: return "Invalid geometry";
        case GeomResult::kEmptyGeometry: return "Empty geometry";
        case GeomResult::kNullGeometry: return "Null geometry";
        case GeomResult::kTopologyError: return "Topology error";
        case GeomResult::kSelfIntersection: return "Self intersection";
        case GeomResult::kInvalidDimension: return "Invalid dimension";
        case GeomResult::kInvalidCoordinate: return "Invalid coordinate";
        case GeomResult::kInvalidRing: return "Invalid ring";
        case GeomResult::kNotClosedRing: return "Ring not closed";
        case GeomResult::kDegenerateGeometry: return "Degenerate geometry";
        case GeomResult::kInvalidPolygonShell: return "Invalid polygon shell";
        case GeomResult::kInvalidHole: return "Invalid hole";
        case GeomResult::kHoleOutsideShell: return "Hole outside shell";
        case GeomResult::kNestedHoles: return "Nested holes";
        case GeomResult::kDisconnectedInterior: return "Disconnected interior";
        case GeomResult::kRepeatedPoint: return "Repeated point";
        case GeomResult::kTooFewPoints: return "Too few points";
        case GeomResult::kInvalidParameters: return "Invalid parameters";
        case GeomResult::kOutOfRange: return "Index out of range";
        case GeomResult::kInvalidSRID: return "Invalid SRID";
        case GeomResult::kInvalidTolerance: return "Invalid tolerance";
        case GeomResult::kNullPointer: return "Null pointer";
        case GeomResult::kInvalidIndex: return "Invalid index";
        case GeomResult::kInvalidWidth: return "Invalid width";
        case GeomResult::kInvalidPrecision: return "Invalid precision";
        case GeomResult::kInvalidBufferSize: return "Invalid buffer size";
        case GeomResult::kNotSupported: return "Operation not supported";
        case GeomResult::kNotImplemented: return "Operation not implemented";
        case GeomResult::kOperationFailed: return "Operation failed";
        case GeomResult::kInvalidOperation: return "Invalid operation";
        case GeomResult::kTypeMismatch: return "Type mismatch";
        case GeomResult::kReadOnly: return "Read only";
        case GeomResult::kOutOfMemory: return "Out of memory";
        case GeomResult::kBufferOverflow: return "Buffer overflow";
        case GeomResult::kResourceExhausted: return "Resource exhausted";
        case GeomResult::kIOError: return "IO error";
        case GeomResult::kParseError: return "Parse error";
        case GeomResult::kFormatError: return "Format error";
        case GeomResult::kFileNotFound: return "File not found";
        case GeomResult::kWriteFailed: return "Write failed";
        case GeomResult::kReadFailed: return "Read failed";
        case GeomResult::kTransformError: return "Transform error";
        case GeomResult::kCRSNotFound: return "CRS not found";
        case GeomResult::kCRSConversionError: return "CRS conversion error";
        case GeomResult::kInvalidProjection: return "Invalid projection";
        case GeomResult::kCoordinateOutOfRange: return "Coordinate out of range";
        default: return "Unknown error";
    }
}

/**
 * @brief 判断两个double值是否近似相等
 * @param a 第一个值
 * @param b 第二个值
 * @param tolerance 容差
 * @return 是否近似相等
 */
inline bool IsEqual(double a, double b, double tolerance = DEFAULT_TOLERANCE) noexcept {
    return std::abs(a - b) <= tolerance;
}

/**
 * @brief 判断double值是否为零
 * @param value 要判断的值
 * @param tolerance 容差
 * @return 是否为零
 */
inline bool IsZero(double value, double tolerance = DEFAULT_TOLERANCE) noexcept {
    return std::abs(value) <= tolerance;
}

}
