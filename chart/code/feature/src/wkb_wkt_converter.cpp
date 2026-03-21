#include "ogc/feature/wkb_wkt_converter.h"
#include "ogc/feature/feature_defn.h"
#include "ogc/feature/field_defn.h"
#include "ogc/feature/field_type.h"
#include "ogc/geometry.h"
#include "ogc/point.h"
#include "ogc/coordinate.h"
#include "ogc/polygon.h"
#include "ogc/linestring.h"
#include "ogc/linearring.h"
#include <sstream>
#include <iomanip>
#include <cstring>

namespace ogc {

struct CNWkbWktConverter::Impl {
    ByteOrder byte_order_;
    bool include_srid_;
    
    Impl() : byte_order_(ByteOrder::kNDR), include_srid_(true) {}
};

CNWkbWktConverter::CNWkbWktConverter()
    : impl_(new Impl()) {
}

CNWkbWktConverter::~CNWkbWktConverter() {
}

void CNWkbWktConverter::SetByteOrder(ByteOrder order) {
    impl_->byte_order_ = order;
}

ByteOrder CNWkbWktConverter::GetByteOrder() const {
    return impl_->byte_order_;
}

void CNWkbWktConverter::SetIncludeSRID(bool include) {
    impl_->include_srid_ = include;
}

bool CNWkbWktConverter::GetIncludeSRID() const {
    return impl_->include_srid_;
}

std::string CNWkbWktConverter::GeometryToWKT(GeometryPtr geometry) {
    if (!geometry) return "EMPTY";
    
    std::ostringstream oss;
    GeomType type = geometry->GetGeometryType();
    
    switch (type) {
        case GeomType::kPoint: {
            auto point = static_cast<const class Point*>(geometry.get());
            auto coord = point->GetCoordinate();
            oss << "POINT(" << coord.x << " " << coord.y << ")";
            break;
        }
        case GeomType::kLineString: {
            auto line = static_cast<const class LineString*>(geometry.get());
            oss << "LINESTRING(";
            size_t num_points = line->GetNumPoints();
            for (size_t i = 0; i < num_points; ++i) {
                if (i > 0) oss << ",";
                auto coord = line->GetCoordinateN(i);
                oss << coord.x << " " << coord.y;
            }
            oss << ")";
            break;
        }
        case GeomType::kPolygon: {
            auto polygon = static_cast<const class Polygon*>(geometry.get());
            oss << "POLYGON((";
            auto exterior = polygon->GetExteriorRing();
            size_t num_points = exterior->GetNumPoints();
            for (size_t i = 0; i < num_points; ++i) {
                if (i > 0) oss << ",";
                auto coord = exterior->GetCoordinateN(i);
                oss << coord.x << " " << coord.y;
            }
            oss << "))";
            break;
        }
        default:
            oss << "UNSUPPORTED";
            break;
    }
    
    return oss.str();
}

std::string CNWkbWktConverter::ToWKT(const CNFeature* feature) {
    if (!feature) return "EMPTY";
    GeometryPtr geom = feature->GetGeometry();
    return GeometryToWKT(std::move(geom));
}

std::string CNWkbWktConverter::ToWKT(const CNFeatureCollection* collection) {
    if (!collection) return "EMPTY";
    std::ostringstream oss;
    oss << "GEOMETRYCOLLECTION(";
    size_t count = collection->GetFeatureCount();
    for (size_t i = 0; i < count; ++i) {
        if (i > 0) oss << ",";
        const CNFeature* feature = collection->GetFeature(i);
        oss << ToWKT(feature);
    }
    oss << ")";
    return oss.str();
}

namespace {

void WriteByte(uint8_t byte, std::vector<uint8_t>& buffer) {
    buffer.push_back(byte);
}

void WriteUInt32(uint32_t value, std::vector<uint8_t>& buffer, ByteOrder order) {
    uint8_t bytes[4];
    std::memcpy(bytes, &value, 4);
    if (order == ByteOrder::kNDR) {
        buffer.push_back(bytes[0]);
        buffer.push_back(bytes[1]);
        buffer.push_back(bytes[2]);
        buffer.push_back(bytes[3]);
    } else {
        buffer.push_back(bytes[3]);
        buffer.push_back(bytes[2]);
        buffer.push_back(bytes[1]);
        buffer.push_back(bytes[0]);
    }
}

void WriteDouble(double value, std::vector<uint8_t>& buffer, ByteOrder order) {
    uint8_t bytes[8];
    std::memcpy(bytes, &value, 8);
    if (order == ByteOrder::kNDR) {
        for (int i = 0; i < 8; ++i) {
            buffer.push_back(bytes[i]);
        }
    } else {
        for (int i = 7; i >= 0; --i) {
            buffer.push_back(bytes[i]);
        }
    }
}

} // namespace

std::vector<uint8_t> CNWkbWktConverter::GeometryToWKB(GeometryPtr geometry) {
    std::vector<uint8_t> wkb;
    if (!geometry) return wkb;
    
    WriteByte(static_cast<uint8_t>(impl_->byte_order_), wkb);
    
    uint32_t type = 0;
    GeomType geom_type = geometry->GetGeometryType();
    switch (geom_type) {
        case GeomType::kPoint: type = 1; break;
        case GeomType::kLineString: type = 2; break;
        case GeomType::kPolygon: type = 3; break;
        case GeomType::kMultiPoint: type = 4; break;
        case GeomType::kMultiLineString: type = 5; break;
        case GeomType::kMultiPolygon: type = 6; break;
        case GeomType::kGeometryCollection: type = 7; break;
        default: type = 0; break;
    }
    WriteUInt32(type, wkb, impl_->byte_order_);
    
    switch (geom_type) {
        case GeomType::kPoint: {
            auto point = static_cast<const class Point*>(geometry.get());
            auto coord = point->GetCoordinate();
            WriteDouble(coord.x, wkb, impl_->byte_order_);
            WriteDouble(coord.y, wkb, impl_->byte_order_);
            break;
        }
        case GeomType::kLineString: {
            auto line = static_cast<const class LineString*>(geometry.get());
            uint32_t num_points = static_cast<uint32_t>(line->GetNumPoints());
            WriteUInt32(num_points, wkb, impl_->byte_order_);
            for (size_t i = 0; i < line->GetNumPoints(); ++i) {
                auto coord = line->GetCoordinateN(i);
                WriteDouble(coord.x, wkb, impl_->byte_order_);
                WriteDouble(coord.y, wkb, impl_->byte_order_);
            }
            break;
        }
        case GeomType::kPolygon: {
            auto polygon = static_cast<const class Polygon*>(geometry.get());
            uint32_t num_rings = static_cast<uint32_t>(polygon->GetNumRings() + 1);
            WriteUInt32(num_rings, wkb, impl_->byte_order_);
            
            auto exterior = polygon->GetExteriorRing();
            uint32_t num_exterior = static_cast<uint32_t>(exterior->GetNumPoints());
            WriteUInt32(num_exterior, wkb, impl_->byte_order_);
            for (size_t i = 0; i < exterior->GetNumPoints(); ++i) {
                auto coord = exterior->GetCoordinateN(i);
                WriteDouble(coord.x, wkb, impl_->byte_order_);
                WriteDouble(coord.y, wkb, impl_->byte_order_);
            }
            break;
        }
        default:
            break;
    }
    
    return wkb;
}

std::vector<uint8_t> CNWkbWktConverter::ToWKB(CNFeature* feature) {
    if (!feature) return std::vector<uint8_t>();
    GeometryPtr geom = feature->GetGeometry();
    return GeometryToWKB(std::move(geom));
}

std::vector<uint8_t> CNWkbWktConverter::ToWKB(GeometryPtr geometry) {
    return GeometryToWKB(std::move(geometry));
}

namespace {

uint8_t ReadByte(const std::vector<uint8_t>& buffer, size_t& pos) {
    return buffer[pos++];
}

uint32_t ReadUInt32(const std::vector<uint8_t>& buffer, size_t& pos, ByteOrder order) {
    uint8_t bytes[4];
    if (order == ByteOrder::kNDR) {
        bytes[0] = buffer[pos++];
        bytes[1] = buffer[pos++];
        bytes[2] = buffer[pos++];
        bytes[3] = buffer[pos++];
    } else {
        bytes[3] = buffer[pos++];
        bytes[2] = buffer[pos++];
        bytes[1] = buffer[pos++];
        bytes[0] = buffer[pos++];
    }
    uint32_t value;
    std::memcpy(&value, bytes, 4);
    return value;
}

double ReadDouble(const std::vector<uint8_t>& buffer, size_t& pos, ByteOrder order) {
    uint8_t bytes[8];
    if (order == ByteOrder::kNDR) {
        for (int i = 0; i < 8; ++i) {
            bytes[i] = buffer[pos++];
        }
    } else {
        for (int i = 7; i >= 0; --i) {
            bytes[i] = buffer[pos++];
        }
    }
    double value;
    std::memcpy(&value, bytes, 8);
    return value;
}

} // namespace

CNFeature* CNWkbWktConverter::FromWKB(const std::vector<uint8_t>& wkb) {
    if (wkb.empty()) return nullptr;
    
    size_t pos = 0;
    ByteOrder order = static_cast<ByteOrder>(wkb[pos++]);
    uint32_t type = ReadUInt32(wkb, pos, order);
    
    GeometryPtr geom;
    
    switch (type) {
        case 1: {
            double x = ReadDouble(wkb, pos, order);
            double y = ReadDouble(wkb, pos, order);
            geom = Point::Create(x, y);
            break;
        }
        case 2: {
            uint32_t num_points = ReadUInt32(wkb, pos, order);
            LineStringPtr line = LineString::Create();
            for (uint32_t i = 0; i < num_points; ++i) {
                double x = ReadDouble(wkb, pos, order);
                double y = ReadDouble(wkb, pos, order);
                line->AddPoint(Coordinate(x, y));
            }
            geom.reset(line.release());
            break;
        }
        case 3: {
            uint32_t num_rings = ReadUInt32(wkb, pos, order);
            PolygonPtr polygon = Polygon::Create();
            for (uint32_t r = 0; r < num_rings; ++r) {
                uint32_t num_ring_points = ReadUInt32(wkb, pos, order);
                LinearRingPtr ring = LinearRing::Create();
                for (uint32_t i = 0; i < num_ring_points; ++i) {
                    double x = ReadDouble(wkb, pos, order);
                    double y = ReadDouble(wkb, pos, order);
                    ring->AddPoint(Coordinate(x, y));
                }
                if (r == 0) {
                    polygon->SetExteriorRing(std::move(ring));
                } else {
                    polygon->AddInteriorRing(std::move(ring));
                }
            }
            geom.reset(polygon.release());
            break;
        }
        default:
            break;
    }
    
    CNFeature* feature = new CNFeature();
    feature->SetGeometry(std::move(geom));
    return feature;
}

CNFeature* CNWkbWktConverter::FromWKT(const std::string& wkt) {
    if (wkt.empty()) return nullptr;
    return nullptr;
}

} // namespace ogc
