#pragma once

#include "common.h"
#include "result.h"
#include "ogc/geometry.h"
#include <memory>
#include <string>
#include <vector>
#include <cstdint>

namespace ogc {
    class Geometry;
    class Coordinate;
    class Point;
    class LineString;
    class LinearRing;
    class Polygon;
    class MultiPoint;
    class MultiLineString;
    class MultiPolygon;
    class GeometryCollection;
}

namespace ogc {
namespace db {

constexpr uint32_t kWkbByteOrderMask = 0x01;
constexpr uint32_t kWkbTypeMask = 0x0FFFFFFF;
constexpr uint32_t kWkbHasZ = 0x80000000;
constexpr uint32_t kWkbHasM = 0x40000000;
constexpr uint32_t kWkbHasSRID = 0x20000000;

constexpr uint32_t kWkbPoint = 1;
constexpr uint32_t kWkbLineString = 2;
constexpr uint32_t kWkbPolygon = 3;
constexpr uint32_t kWkbMultiPoint = 4;
constexpr uint32_t kWkbMultiLineString = 5;
constexpr uint32_t kWkbMultiPolygon = 6;
constexpr uint32_t kWkbGeometryCollection = 7;

enum class ByteOrder : uint8_t {
    kBigEndian = 0,
    kLittleEndian = 1
};

struct WkbOptions {
    bool includeSRID = true;
    bool includeZ = true;
    bool includeM = false;
    bool bigEndian = false;
    
    WkbOptions() = default;
};

class OGC_DB_API WkbConverter {
public:
    static Result GeometryToWkb(const Geometry* geometry, std::vector<uint8_t>& wkb, const WkbOptions& options = WkbOptions());
    
    static Result GeometryToHexWkb(const Geometry* geometry, std::string& hexWkb, const WkbOptions& options = WkbOptions());
    
    static Result WkbToGeometry(const std::vector<uint8_t>& wkb, std::unique_ptr<Geometry>& geometry, const WkbOptions& options = WkbOptions());
    
    static Result HexWkbToGeometry(const std::string& hexWkb, std::unique_ptr<Geometry>& geometry, const WkbOptions& options = WkbOptions());
    
    static bool IsEwkb(const std::vector<uint8_t>& wkb);
    
    static bool IsLittleEndian(const std::vector<uint8_t>& wkb);
    
    static bool ExtractSrid(const std::vector<uint8_t>& ewkb, int& srid);
    
    static Result GetGeometryType(const std::vector<uint8_t>& wkb, uint32_t& type);
    
    static size_t GetWkbSize(const std::vector<uint8_t>& wkb);
    
    static size_t EstimateWkbSize(const Geometry* geometry);

private:
    static void WriteByteOrder(std::vector<uint8_t>& buffer, ByteOrder order);
    static ByteOrder ReadByteOrder(const uint8_t* data);
    
    static void WriteUInt32(std::vector<uint8_t>& buffer, uint32_t value, ByteOrder order);
    static uint32_t ReadUInt32(const uint8_t* data, ByteOrder order);
    
    static void WriteInt32(std::vector<uint8_t>& buffer, int32_t value, ByteOrder order);
    static int32_t ReadInt32(const uint8_t* data, ByteOrder order);
    
    static void WriteDouble(std::vector<uint8_t>& buffer, double value, ByteOrder order);
    static double ReadDouble(const uint8_t* data, ByteOrder order);
    
    static void WriteCoordinate(std::vector<uint8_t>& buffer, const Coordinate& coord, ByteOrder order, bool hasZ, bool hasM);
    static Coordinate ReadCoordinate(const uint8_t* data, ByteOrder order, bool hasZ, bool hasM);
    
    static Result WriteGeometry(std::vector<uint8_t>& buffer, const Geometry* geometry, ByteOrder order, bool includeSRID);
    static Result ReadGeometry(const uint8_t* data, size_t size, std::unique_ptr<Geometry>& geometry, int& srid);
    
    static Result WritePoint(std::vector<uint8_t>& buffer, const Point* point, ByteOrder order, bool includeSRID);
    static Result WriteLineString(std::vector<uint8_t>& buffer, const LineString* line, ByteOrder order, bool includeSRID);
    static Result WritePolygon(std::vector<uint8_t>& buffer, const Polygon* polygon, ByteOrder order, bool includeSRID);
    static Result WriteMultiPoint(std::vector<uint8_t>& buffer, const MultiPoint* mpoint, ByteOrder order, bool includeSRID);
    static Result WriteMultiLineString(std::vector<uint8_t>& buffer, const MultiLineString* mline, ByteOrder order, bool includeSRID);
    static Result WriteMultiPolygon(std::vector<uint8_t>& buffer, const MultiPolygon* mpoly, ByteOrder order, bool includeSRID);
    static Result WriteGeometryCollection(std::vector<uint8_t>& buffer, const GeometryCollection* collection, ByteOrder order, bool includeSRID);
    
    static Result ReadPoint(const uint8_t* data, size_t size, std::unique_ptr<Geometry>& geometry, bool hasZ, bool hasM, ByteOrder order);
    static Result ReadLineString(const uint8_t* data, size_t size, std::unique_ptr<Geometry>& geometry, bool hasZ, bool hasM, ByteOrder order);
    static Result ReadPolygon(const uint8_t* data, size_t size, std::unique_ptr<Geometry>& geometry, bool hasZ, bool hasM, ByteOrder order);
    static Result ReadMultiPoint(const uint8_t* data, size_t size, std::unique_ptr<Geometry>& geometry, bool hasZ, bool hasM, ByteOrder order);
    static Result ReadMultiLineString(const uint8_t* data, size_t size, std::unique_ptr<Geometry>& geometry, bool hasZ, bool hasM, ByteOrder order);
    static Result ReadMultiPolygon(const uint8_t* data, size_t size, std::unique_ptr<Geometry>& geometry, bool hasZ, bool hasM, ByteOrder order);
    static Result ReadGeometryCollection(const uint8_t* data, size_t size, std::unique_ptr<Geometry>& geometry, bool hasZ, bool hasM, ByteOrder order);
    
    static uint32_t GetWkbTypeCode(GeomType type, bool hasZ, bool hasM);
    static GeomType GetGeomTypeFromWkb(uint32_t typeCode);
    static bool HasZFromWkb(uint32_t typeCode);
    static bool HasMFromWkb(uint32_t typeCode);
};

}
}
