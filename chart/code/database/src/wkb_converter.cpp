#include "ogc/db/wkb_converter.h"
#include "ogc/geometry.h"
#include "ogc/point.h"
#include "ogc/linestring.h"
#include "ogc/linearring.h"
#include "ogc/polygon.h"
#include "ogc/multipoint.h"
#include "ogc/multilinestring.h"
#include "ogc/multipolygon.h"
#include "ogc/geometrycollection.h"
#include <algorithm>
#include <cstring>

namespace ogc {
namespace db {

Result WkbConverter::GeometryToWkb(const Geometry* geometry, std::vector<uint8_t>& wkb, const WkbOptions& options) {
    if (!geometry) {
        return Result::Error(DbResult::kInvalidGeometry, "Geometry is null");
    }
    
    wkb.clear();
    ByteOrder order = options.bigEndian ? ByteOrder::kBigEndian : ByteOrder::kLittleEndian;
    
    WriteByteOrder(wkb, order);
    
    uint32_t typeCode = GetWkbTypeCode(geometry->GetGeometryType(), geometry->Is3D(), geometry->IsMeasured());
    
    if (options.includeSRID && geometry->GetSRID() > 0) {
        typeCode |= kWkbHasSRID;
    }
    
    WriteUInt32(wkb, typeCode, order);
    
    if (options.includeSRID && geometry->GetSRID() > 0) {
        WriteInt32(wkb, static_cast<int32_t>(geometry->GetSRID()), order);
    }
    
    return WriteGeometry(wkb, geometry, order, options.includeSRID && geometry->GetSRID() > 0);
}

Result WkbConverter::GeometryToHexWkb(const Geometry* geometry, std::string& hexWkb, const WkbOptions& options) {
    std::vector<uint8_t> wkb;
    Result result = GeometryToWkb(geometry, wkb, options);
    if (!result.IsSuccess()) {
        return result;
    }
    
    static const char hexDigits[] = "0123456789ABCDEF";
    hexWkb.reserve(wkb.size() * 2);
    
    for (uint8_t byte : wkb) {
        hexWkb += hexDigits[(byte >> 4) & 0x0F];
        hexWkb += hexDigits[byte & 0x0F];
    }
    
    return Result::Success();
}

Result WkbConverter::WkbToGeometry(const std::vector<uint8_t>& wkb, std::unique_ptr<Geometry>& geometry, const WkbOptions& options) {
    if (wkb.empty()) {
        return Result::Error(DbResult::kInvalidWkb, "WKB is empty");
    }
    
    int srid = 0;
    return ReadGeometry(wkb.data(), wkb.size(), geometry, srid);
}

Result WkbConverter::HexWkbToGeometry(const std::string& hexWkb, std::unique_ptr<Geometry>& geometry, const WkbOptions& options) {
    if (hexWkb.empty() || hexWkb.length() % 2 != 0) {
        return Result::Error(DbResult::kInvalidWkb, "Invalid hex WKB length");
    }
    
    std::vector<uint8_t> wkb;
    wkb.reserve(hexWkb.length() / 2);
    
    for (size_t i = 0; i < hexWkb.length(); i += 2) {
        char high = hexWkb[i];
        char low = hexWkb[i + 1];
        
        int highVal = 0, lowVal = 0;
        
        if (high >= '0' && high <= '9') highVal = high - '0';
        else if (high >= 'A' && high <= 'F') highVal = high - 'A' + 10;
        else if (high >= 'a' && high <= 'f') highVal = high - 'a' + 10;
        else return Result::Error(DbResult::kInvalidWkb, "Invalid hex character");
        
        if (low >= '0' && low <= '9') lowVal = low - '0';
        else if (low >= 'A' && low <= 'F') lowVal = low - 'A' + 10;
        else if (low >= 'a' && low <= 'f') lowVal = low - 'a' + 10;
        else return Result::Error(DbResult::kInvalidWkb, "Invalid hex character");
        
        wkb.push_back(static_cast<uint8_t>((highVal << 4) | lowVal));
    }
    
    return WkbToGeometry(wkb, geometry);
}

bool WkbConverter::IsEwkb(const std::vector<uint8_t>& wkb) {
    if (wkb.size() < 5) return false;
    
    ByteOrder order = ReadByteOrder(wkb.data());
    uint32_t typeCode = ReadUInt32(wkb.data() + 1, order);
    
    return (typeCode & kWkbHasSRID) != 0;
}

bool WkbConverter::IsLittleEndian(const std::vector<uint8_t>& wkb) {
    if (wkb.empty()) return true;
    return ReadByteOrder(wkb.data()) == ByteOrder::kLittleEndian;
}

bool WkbConverter::ExtractSrid(const std::vector<uint8_t>& ewkb, int& srid) {
    if (ewkb.size() < 9) return false;
    
    ByteOrder order = ReadByteOrder(ewkb.data());
    uint32_t typeCode = ReadUInt32(ewkb.data() + 1, order);
    
    if ((typeCode & kWkbHasSRID) == 0) {
        srid = 0;
        return false;
    }
    
    srid = static_cast<int>(ReadInt32(ewkb.data() + 5, order));
    return true;
}

Result WkbConverter::GetGeometryType(const std::vector<uint8_t>& wkb, uint32_t& type) {
    if (wkb.size() < 5) {
        return Result::Error(DbResult::kInvalidWkb, "WKB too short");
    }
    
    ByteOrder order = ReadByteOrder(wkb.data());
    type = ReadUInt32(wkb.data() + 1, order);
    
    return Result::Success();
}

size_t WkbConverter::GetWkbSize(const std::vector<uint8_t>& wkb) {
    return wkb.size();
}

size_t WkbConverter::EstimateWkbSize(const Geometry* geometry) {
    if (!geometry) return 0;
    
    size_t size = 5;
    
    if (geometry->GetSRID() > 0) {
        size += 4;
    }
    
    switch (geometry->GetGeometryType()) {
        case GeomType::kPoint:
            size += geometry->Is3D() ? 24 : 16;
            break;
        case GeomType::kLineString:
            size += 4 + geometry->GetNumCoordinates() * (geometry->Is3D() ? 24 : 16);
            break;
        default:
            size *= 2;
            break;
    }
    
    return size;
}

void WkbConverter::WriteByteOrder(std::vector<uint8_t>& buffer, ByteOrder order) {
    buffer.push_back(static_cast<uint8_t>(order));
}

ByteOrder WkbConverter::ReadByteOrder(const uint8_t* data) {
    return (data[0] == 1) ? ByteOrder::kLittleEndian : ByteOrder::kBigEndian;
}

void WkbConverter::WriteUInt32(std::vector<uint8_t>& buffer, uint32_t value, ByteOrder order) {
    if (order == ByteOrder::kLittleEndian) {
        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&value);
        buffer.insert(buffer.end(), bytes, bytes + 4);
    } else {
        for (int i = 3; i >= 0; --i) {
            buffer.push_back(static_cast<uint8_t>((value >> (i * 8)) & 0xFF));
        }
    }
}

uint32_t WkbConverter::ReadUInt32(const uint8_t* data, ByteOrder order) {
    if (order == ByteOrder::kLittleEndian) {
        uint32_t value;
        std::memcpy(&value, data, 4);
        return value;
    } else {
        return (static_cast<uint32_t>(data[0]) << 24) |
               (static_cast<uint32_t>(data[1]) << 16) |
               (static_cast<uint32_t>(data[2]) << 8) |
               static_cast<uint32_t>(data[3]);
    }
}

void WkbConverter::WriteInt32(std::vector<uint8_t>& buffer, int32_t value, ByteOrder order) {
    WriteUInt32(buffer, static_cast<uint32_t>(value), order);
}

int32_t WkbConverter::ReadInt32(const uint8_t* data, ByteOrder order) {
    return static_cast<int32_t>(ReadUInt32(data, order));
}

void WkbConverter::WriteDouble(std::vector<uint8_t>& buffer, double value, ByteOrder order) {
    if (order == ByteOrder::kLittleEndian) {
        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&value);
        buffer.insert(buffer.end(), bytes, bytes + 8);
    } else {
        for (int i = 7; i >= 0; --i) {
            buffer.push_back(reinterpret_cast<const uint8_t*>(&value)[i]);
        }
    }
}

double WkbConverter::ReadDouble(const uint8_t* data, ByteOrder order) {
    double value;
    if (order == ByteOrder::kLittleEndian) {
        std::memcpy(&value, data, 8);
    } else {
        uint8_t bytes[8];
        for (int i = 7; i >= 0; --i) {
            bytes[7 - i] = data[i];
        }
        std::memcpy(&value, bytes, 8);
    }
    return value;
}

void WkbConverter::WriteCoordinate(std::vector<uint8_t>& buffer, const Coordinate& coord, ByteOrder order, bool hasZ, bool hasM) {
    WriteDouble(buffer, coord.x, order);
    WriteDouble(buffer, coord.y, order);
    
    if (hasZ) {
        WriteDouble(buffer, coord.Is3D() ? coord.z : 0.0, order);
    }
    
    if (hasM) {
        WriteDouble(buffer, coord.IsMeasured() ? coord.m : 0.0, order);
    }
}

Coordinate WkbConverter::ReadCoordinate(const uint8_t* data, ByteOrder order, bool hasZ, bool hasM) {
    double x = ReadDouble(data, order);
    double y = ReadDouble(data + 8, order);
    
    size_t offset = 16;
    double z = std::numeric_limits<double>::quiet_NaN();
    double m = std::numeric_limits<double>::quiet_NaN();
    
    if (hasZ) {
        z = ReadDouble(data + offset, order);
        offset += 8;
    }
    
    if (hasM) {
        m = ReadDouble(data + offset, order);
    }
    
    if (hasZ && hasM) {
        return Coordinate(x, y, z, m);
    } else if (hasZ) {
        return Coordinate(x, y, z);
    } else {
        return Coordinate(x, y);
    }
}

Result WkbConverter::WriteGeometry(std::vector<uint8_t>& buffer, const Geometry* geometry, ByteOrder order, bool includeSRID) {
    if (!geometry) {
        return Result::Error(DbResult::kInvalidGeometry);
    }
    
    switch (geometry->GetGeometryType()) {
        case GeomType::kPoint:
            return WritePoint(buffer, static_cast<const Point*>(geometry), order, includeSRID);
        case GeomType::kLineString:
            return WriteLineString(buffer, static_cast<const LineString*>(geometry), order, includeSRID);
        case GeomType::kPolygon:
            return WritePolygon(buffer, static_cast<const Polygon*>(geometry), order, includeSRID);
        case GeomType::kMultiPoint:
            return WriteMultiPoint(buffer, static_cast<const MultiPoint*>(geometry), order, includeSRID);
        case GeomType::kMultiLineString:
            return WriteMultiLineString(buffer, static_cast<const MultiLineString*>(geometry), order, includeSRID);
        case GeomType::kMultiPolygon:
            return WriteMultiPolygon(buffer, static_cast<const MultiPolygon*>(geometry), order, includeSRID);
        case GeomType::kGeometryCollection:
            return WriteGeometryCollection(buffer, static_cast<const GeometryCollection*>(geometry), order, includeSRID);
        default:
            return Result::Error(DbResult::kNotSupported, "Unsupported geometry type");
    }
}

Result WkbConverter::ReadGeometry(const uint8_t* data, size_t size, std::unique_ptr<Geometry>& geometry, int& srid) {
    if (size < 5) {
        return Result::Error(DbResult::kInvalidWkb, "WKB too short");
    }
    
    ByteOrder order = ReadByteOrder(data);
    uint32_t typeCode = ReadUInt32(data + 1, order);
    
    bool hasSRID = (typeCode & kWkbHasSRID) != 0;
    bool hasZ = (typeCode & kWkbHasZ) != 0;
    bool hasM = (typeCode & kWkbHasM) != 0;
    
    size_t offset = 5;
    
    if (hasSRID) {
        if (size < 9) {
            return Result::Error(DbResult::kInvalidWkb, "WKB too short for SRID");
        }
        srid = static_cast<int>(ReadInt32(data + offset, order));
        offset += 4;
    } else {
        srid = 0;
    }
    
    uint32_t baseType = typeCode & kWkbTypeMask;
    
    Result result = Result::Error(DbResult::kInvalidWkb, "Unknown geometry type");
    
    switch (baseType) {
        case kWkbPoint:
            result = ReadPoint(data + offset, size - offset, geometry, hasZ, hasM, order);
            break;
        case kWkbLineString:
            result = ReadLineString(data + offset, size - offset, geometry, hasZ, hasM, order);
            break;
        case kWkbPolygon:
            result = ReadPolygon(data + offset, size - offset, geometry, hasZ, hasM, order);
            break;
        case kWkbMultiPoint:
            result = ReadMultiPoint(data + offset, size - offset, geometry, hasZ, hasM, order);
            break;
        case kWkbMultiLineString:
            result = ReadMultiLineString(data + offset, size - offset, geometry, hasZ, hasM, order);
            break;
        case kWkbMultiPolygon:
            result = ReadMultiPolygon(data + offset, size - offset, geometry, hasZ, hasM, order);
            break;
        case kWkbGeometryCollection:
            result = ReadGeometryCollection(data + offset, size - offset, geometry, hasZ, hasM, order);
            break;
        default:
            return Result::Error(DbResult::kInvalidWkb, "Unknown geometry type");
    }
    
    if (result.IsSuccess() && geometry && srid > 0) {
        geometry->SetSRID(srid);
    }
    
    return result;
}

Result WkbConverter::WritePoint(std::vector<uint8_t>& buffer, const Point* point, ByteOrder order, bool includeSRID) {
    if (!point) {
        return Result::Error(DbResult::kInvalidGeometry);
    }
    
    const Coordinate& coord = point->GetCoordinate();
    WriteCoordinate(buffer, coord, order, point->Is3D(), point->IsMeasured());
    
    return Result::Success();
}

Result WkbConverter::WriteLineString(std::vector<uint8_t>& buffer, const LineString* line, ByteOrder order, bool includeSRID) {
    if (!line) {
        return Result::Error(DbResult::kInvalidGeometry);
    }
    
    const auto& coords = line->GetCoordinates();
    WriteUInt32(buffer, static_cast<uint32_t>(coords.size()), order);
    
    for (const auto& coord : coords) {
        WriteCoordinate(buffer, coord, order, line->Is3D(), line->IsMeasured());
    }
    
    return Result::Success();
}

Result WkbConverter::WritePolygon(std::vector<uint8_t>& buffer, const Polygon* polygon, ByteOrder order, bool includeSRID) {
    if (!polygon) {
        return Result::Error(DbResult::kInvalidGeometry);
    }
    
    const LinearRing* exterior = polygon->GetExteriorRing();
    size_t numRings = 1 + polygon->GetNumInteriorRings();
    
    WriteUInt32(buffer, static_cast<uint32_t>(numRings), order);
    
    if (exterior) {
        const auto& coords = exterior->GetCoordinates();
        WriteUInt32(buffer, static_cast<uint32_t>(coords.size()), order);
        for (const auto& coord : coords) {
            WriteCoordinate(buffer, coord, order, polygon->Is3D(), polygon->IsMeasured());
        }
    }
    
    for (size_t i = 0; i < polygon->GetNumInteriorRings(); ++i) {
        const LinearRing* interior = polygon->GetInteriorRingN(i);
        if (interior) {
            const auto& coords = interior->GetCoordinates();
            WriteUInt32(buffer, static_cast<uint32_t>(coords.size()), order);
            for (const auto& coord : coords) {
                WriteCoordinate(buffer, coord, order, polygon->Is3D(), polygon->IsMeasured());
            }
        }
    }
    
    return Result::Success();
}

Result WkbConverter::WriteMultiPoint(std::vector<uint8_t>& buffer, const MultiPoint* mpoint, ByteOrder order, bool includeSRID) {
    if (!mpoint) {
        return Result::Error(DbResult::kInvalidGeometry);
    }
    
    WriteUInt32(buffer, static_cast<uint32_t>(mpoint->GetNumGeometries()), order);
    
    for (size_t i = 0; i < mpoint->GetNumGeometries(); ++i) {
        const Point* pt = static_cast<const Point*>(mpoint->GetGeometryN(i));
        if (pt) {
            WriteByteOrder(buffer, order);
            WriteUInt32(buffer, kWkbPoint, order);
            WriteCoordinate(buffer, pt->GetCoordinate(), order, pt->Is3D(), pt->IsMeasured());
        }
    }
    
    return Result::Success();
}

Result WkbConverter::WriteMultiLineString(std::vector<uint8_t>& buffer, const MultiLineString* mline, ByteOrder order, bool includeSRID) {
    if (!mline) {
        return Result::Error(DbResult::kInvalidGeometry);
    }
    
    WriteUInt32(buffer, static_cast<uint32_t>(mline->GetNumGeometries()), order);
    
    for (size_t i = 0; i < mline->GetNumGeometries(); ++i) {
        const LineString* line = static_cast<const LineString*>(mline->GetGeometryN(i));
        if (line) {
            WriteByteOrder(buffer, order);
            WriteUInt32(buffer, kWkbLineString, order);
            
            const auto& coords = line->GetCoordinates();
            WriteUInt32(buffer, static_cast<uint32_t>(coords.size()), order);
            for (const auto& coord : coords) {
                WriteCoordinate(buffer, coord, order, line->Is3D(), line->IsMeasured());
            }
        }
    }
    
    return Result::Success();
}

Result WkbConverter::WriteMultiPolygon(std::vector<uint8_t>& buffer, const MultiPolygon* mpoly, ByteOrder order, bool includeSRID) {
    if (!mpoly) {
        return Result::Error(DbResult::kInvalidGeometry);
    }
    
    WriteUInt32(buffer, static_cast<uint32_t>(mpoly->GetNumGeometries()), order);
    
    for (size_t i = 0; i < mpoly->GetNumGeometries(); ++i) {
        const Polygon* poly = static_cast<const Polygon*>(mpoly->GetGeometryN(i));
        if (poly) {
            WriteByteOrder(buffer, order);
            WriteUInt32(buffer, kWkbPolygon, order);
            
            const LinearRing* exterior = poly->GetExteriorRing();
            size_t numRings = 1 + poly->GetNumInteriorRings();
            
            WriteUInt32(buffer, static_cast<uint32_t>(numRings), order);
            
            if (exterior) {
                const auto& coords = exterior->GetCoordinates();
                WriteUInt32(buffer, static_cast<uint32_t>(coords.size()), order);
                for (const auto& coord : coords) {
                    WriteCoordinate(buffer, coord, order, poly->Is3D(), poly->IsMeasured());
                }
            }
            
            for (size_t j = 0; j < poly->GetNumInteriorRings(); ++j) {
                const LinearRing* interior = poly->GetInteriorRingN(j);
                if (interior) {
                    const auto& coords = interior->GetCoordinates();
                    WriteUInt32(buffer, static_cast<uint32_t>(coords.size()), order);
                    for (const auto& coord : coords) {
                        WriteCoordinate(buffer, coord, order, poly->Is3D(), poly->IsMeasured());
                    }
                }
            }
        }
    }
    
    return Result::Success();
}

Result WkbConverter::WriteGeometryCollection(std::vector<uint8_t>& buffer, const GeometryCollection* collection, ByteOrder order, bool includeSRID) {
    if (!collection) {
        return Result::Error(DbResult::kInvalidGeometry);
    }
    
    WriteUInt32(buffer, static_cast<uint32_t>(collection->GetNumGeometries()), order);
    
    for (size_t i = 0; i < collection->GetNumGeometries(); ++i) {
        const Geometry* geom = collection->GetGeometryN(i);
        if (geom) {
            Result result = WriteGeometry(buffer, geom, order, includeSRID && geom->GetSRID() > 0);
            if (!result.IsSuccess()) {
                return result;
            }
        }
    }
    
    return Result::Success();
}

Result WkbConverter::ReadPoint(const uint8_t* data, size_t size, std::unique_ptr<Geometry>& geometry, bool hasZ, bool hasM, ByteOrder order) {
    size_t coordSize = 16 + (hasZ ? 8 : 0) + (hasM ? 8 : 0);
    if (size < coordSize) {
        return Result::Error(DbResult::kInvalidWkb, "Point WKB too short");
    }
    
    Coordinate coord = ReadCoordinate(data, order, hasZ, hasM);
    geometry = Point::Create(coord);
    
    return Result::Success();
}

Result WkbConverter::ReadLineString(const uint8_t* data, size_t size, std::unique_ptr<Geometry>& geometry, bool hasZ, bool hasM, ByteOrder order) {
    if (size < 4) {
        return Result::Error(DbResult::kInvalidWkb, "LineString WKB too short");
    }
    
    uint32_t numPoints = ReadUInt32(data, order);
    
    size_t coordSize = 16 + (hasZ ? 8 : 0) + (hasM ? 8 : 0);
    if (size < 4 + numPoints * coordSize) {
        return Result::Error(DbResult::kInvalidWkb, "LineString WKB incomplete");
    }
    
    CoordinateList coords;
    size_t offset = 4;
    
    for (uint32_t i = 0; i < numPoints; ++i) {
        coords.push_back(ReadCoordinate(data + offset, order, hasZ, hasM));
        offset += coordSize;
    }
    
    geometry = LineString::Create(std::move(coords));
    
    return Result::Success();
}

Result WkbConverter::ReadPolygon(const uint8_t* data, size_t size, std::unique_ptr<Geometry>& geometry, bool hasZ, bool hasM, ByteOrder order) {
    if (size < 4) {
        return Result::Error(DbResult::kInvalidWkb, "Polygon WKB too short");
    }
    
    uint32_t numRings = ReadUInt32(data, order);
    size_t offset = 4;
    
    size_t coordSize = 16 + (hasZ ? 8 : 0) + (hasM ? 8 : 0);
    CoordinateList exteriorCoords;
    
    if (numRings > 0 && offset + 4 <= size) {
        uint32_t numPoints = ReadUInt32(data + offset, order);
        offset += 4;
        
        if (offset + numPoints * coordSize <= size) {
            for (uint32_t i = 0; i < numPoints; ++i) {
                exteriorCoords.push_back(ReadCoordinate(data + offset, order, hasZ, hasM));
                offset += coordSize;
            }
        }
    }
    
    auto exteriorRing = LinearRing::Create(exteriorCoords, true);
    auto polygon = Polygon::Create(std::move(exteriorRing));
    
    geometry = std::move(polygon);
    
    return Result::Success();
}

Result WkbConverter::ReadMultiPoint(const uint8_t* data, size_t size, std::unique_ptr<Geometry>& geometry, bool hasZ, bool hasM, ByteOrder order) {
    if (size < 4) {
        return Result::Error(DbResult::kInvalidWkb, "MultiPoint WKB too short");
    }
    
    uint32_t numPoints = ReadUInt32(data, order);
    size_t offset = 4;
    
    auto mpoint = MultiPoint::Create();
    
    for (uint32_t i = 0; i < numPoints && offset < size; ++i) {
        std::unique_ptr<Geometry> pointGeom;
        int srid = 0;
        size_t startOffset = offset;
        Result result = ReadGeometry(data + offset, size - offset, pointGeom, srid);
        if (result.IsSuccess() && pointGeom) {
            auto point = dynamic_cast<Point*>(pointGeom.get());
            if (point) {
                mpoint->AddPoint(Point::Create(point->GetCoordinate()));
            }
            size_t coordSize = 16 + (pointGeom->Is3D() ? 8 : 0) + (pointGeom->IsMeasured() ? 8 : 0);
            offset += 5 + (srid > 0 ? 4 : 0) + coordSize;
        } else {
            break;
        }
    }
    
    geometry = std::move(mpoint);
    
    return Result::Success();
}

Result WkbConverter::ReadMultiLineString(const uint8_t* data, size_t size, std::unique_ptr<Geometry>& geometry, bool hasZ, bool hasM, ByteOrder order) {
    if (size < 4) {
        return Result::Error(DbResult::kInvalidWkb, "MultiLineString WKB too short");
    }
    
    uint32_t numLines = ReadUInt32(data, order);
    size_t offset = 4;
    
    auto mline = MultiLineString::Create();
    
    for (uint32_t i = 0; i < numLines && offset < size; ++i) {
        std::unique_ptr<Geometry> lineGeom;
        int srid = 0;
        Result result = ReadGeometry(data + offset, size - offset, lineGeom, srid);
        if (result.IsSuccess() && lineGeom) {
            auto line = dynamic_cast<LineString*>(lineGeom.get());
            if (line) {
                CoordinateList coords = line->GetCoordinates();
                mline->AddLineString(LineString::Create(coords));
            }
            offset += (data + offset - data);
        } else {
            break;
        }
    }
    
    geometry = std::move(mline);
    
    return Result::Success();
}

Result WkbConverter::ReadMultiPolygon(const uint8_t* data, size_t size, std::unique_ptr<Geometry>& geometry, bool hasZ, bool hasM, ByteOrder order) {
    if (size < 4) {
        return Result::Error(DbResult::kInvalidWkb, "MultiPolygon WKB too short");
    }
    
    uint32_t numPolys = ReadUInt32(data, order);
    size_t offset = 4;
    
    auto mpoly = MultiPolygon::Create();
    
    for (uint32_t i = 0; i < numPolys && offset < size; ++i) {
        std::unique_ptr<Geometry> polyGeom;
        int srid = 0;
        Result result = ReadGeometry(data + offset, size - offset, polyGeom, srid);
        if (result.IsSuccess() && polyGeom) {
            auto polygon = dynamic_cast<Polygon*>(polyGeom.get());
            if (polygon) {
                const LinearRing* ring = polygon->GetExteriorRing();
                if (ring) {
                    auto newRing = LinearRing::Create(ring->GetCoordinates(), true);
                    mpoly->AddPolygon(Polygon::Create(std::move(newRing)));
                }
            }
        } else {
            break;
        }
    }
    
    geometry = std::move(mpoly);
    
    return Result::Success();
}

Result WkbConverter::ReadGeometryCollection(const uint8_t* data, size_t size, std::unique_ptr<Geometry>& geometry, bool hasZ, bool hasM, ByteOrder order) {
    if (size < 4) {
        return Result::Error(DbResult::kInvalidWkb, "GeometryCollection WKB too short");
    }
    
    uint32_t numGeoms = ReadUInt32(data, order);
    size_t offset = 4;
    
    auto collection = GeometryCollection::Create();
    
    for (uint32_t i = 0; i < numGeoms && offset < size; ++i) {
        std::unique_ptr<Geometry> childGeom;
        int srid = 0;
        Result result = ReadGeometry(data + offset, size - offset, childGeom, srid);
        if (result.IsSuccess() && childGeom) {
            collection->AddGeometry(std::move(childGeom));
        } else {
            break;
        }
    }
    
    geometry = std::move(collection);
    
    return Result::Success();
}

uint32_t WkbConverter::GetWkbTypeCode(GeomType type, bool hasZ, bool hasM) {
    uint32_t code = 0;
    
    switch (type) {
        case GeomType::kPoint: code = kWkbPoint; break;
        case GeomType::kLineString: code = kWkbLineString; break;
        case GeomType::kPolygon: code = kWkbPolygon; break;
        case GeomType::kMultiPoint: code = kWkbMultiPoint; break;
        case GeomType::kMultiLineString: code = kWkbMultiLineString; break;
        case GeomType::kMultiPolygon: code = kWkbMultiPolygon; break;
        case GeomType::kGeometryCollection: code = kWkbGeometryCollection; break;
        default: code = 0; break;
    }
    
    if (hasZ) code |= kWkbHasZ;
    if (hasM) code |= kWkbHasM;
    
    return code;
}

GeomType WkbConverter::GetGeomTypeFromWkb(uint32_t typeCode) {
    uint32_t baseType = typeCode & kWkbTypeMask;
    
    switch (baseType) {
        case kWkbPoint: return GeomType::kPoint;
        case kWkbLineString: return GeomType::kLineString;
        case kWkbPolygon: return GeomType::kPolygon;
        case kWkbMultiPoint: return GeomType::kMultiPoint;
        case kWkbMultiLineString: return GeomType::kMultiLineString;
        case kWkbMultiPolygon: return GeomType::kMultiPolygon;
        case kWkbGeometryCollection: return GeomType::kGeometryCollection;
        default: return GeomType::kUnknown;
    }
}

bool WkbConverter::HasZFromWkb(uint32_t typeCode) {
    return (typeCode & kWkbHasZ) != 0;
}

bool WkbConverter::HasMFromWkb(uint32_t typeCode) {
    return (typeCode & kWkbHasM) != 0;
}

}
}
