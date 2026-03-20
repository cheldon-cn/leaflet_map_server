#pragma once

#include "envelope.h"
#include "coordinate.h"
#include <algorithm>

namespace ogc {

class OGC_API Envelope3D : public Envelope {
public:
    Envelope3D() noexcept : m_minZ(0.0), m_maxZ(0.0), m_hasZ(false) {}
    
    Envelope3D(double minX, double minY, double minZ,
               double maxX, double maxY, double maxZ) noexcept
        : Envelope(minX, minY, maxX, maxY),
          m_minZ(minZ), m_maxZ(maxZ), m_hasZ(true) {
        NormalizeZ();
    }
    
    explicit Envelope3D(const Envelope& env) noexcept
        : Envelope(env), m_minZ(0.0), m_maxZ(0.0), m_hasZ(false) {}
    
    Envelope3D(const Coordinate& p1, const Coordinate& p2) noexcept
        : Envelope(p1, p2) {
        if (p1.Is3D() && p2.Is3D()) {
            m_minZ = std::min(p1.z, p2.z);
            m_maxZ = std::max(p1.z, p2.z);
            m_hasZ = true;
        } else {
            m_minZ = m_maxZ = 0.0;
            m_hasZ = false;
        }
    }
    
    explicit Envelope3D(const CoordinateList& coords) noexcept : Envelope() {
        for (const auto& coord : coords) {
            ExpandToInclude(coord);
        }
    }
    
    double GetMinZ() const noexcept { return m_minZ; }
    double GetMaxZ() const noexcept { return m_maxZ; }
    bool HasZ() const noexcept { return m_hasZ; }
    
    double GetDepth() const noexcept {
        return m_hasZ ? m_maxZ - m_minZ : 0.0;
    }
    
    double GetVolume() const noexcept {
        if (IsNull() || !m_hasZ) return 0.0;
        return GetWidth() * GetHeight() * GetDepth();
    }
    
    double GetSurfaceArea() const noexcept {
        if (IsNull()) return 0.0;
        
        double w = GetWidth();
        double h = GetHeight();
        double d = GetDepth();
        
        if (!m_hasZ) {
            return GetArea();
        }
        
        return 2.0 * (w * h + w * d + h * d);
    }
    
    Coordinate GetCentre() const noexcept {
        if (IsNull()) return Coordinate::Empty();
        
        double cx = (GetMinX() + GetMaxX()) / 2.0;
        double cy = (GetMinY() + GetMaxY()) / 2.0;
        
        if (m_hasZ) {
            double cz = (m_minZ + m_maxZ) / 2.0;
            return Coordinate(cx, cy, cz);
        }
        
        return Coordinate(cx, cy);
    }
    
    void SetNull() noexcept {
        Envelope::SetNull();
        m_minZ = m_maxZ = 0.0;
        m_hasZ = false;
    }
    
    void ExpandToInclude(const Coordinate& coord) noexcept {
        Envelope::ExpandToInclude(coord);
        
        if (coord.Is3D()) {
            if (!m_hasZ) {
                m_minZ = m_maxZ = coord.z;
                m_hasZ = true;
            } else {
                m_minZ = std::min(m_minZ, coord.z);
                m_maxZ = std::max(m_maxZ, coord.z);
            }
        }
    }
    
    void ExpandToInclude(const CoordinateList& coords) noexcept {
        for (const auto& coord : coords) {
            ExpandToInclude(coord);
        }
    }
    
    void ExpandToInclude(const Envelope3D& other) noexcept {
        Envelope::ExpandToInclude(static_cast<const Envelope&>(other));
        
        if (other.m_hasZ) {
            if (!m_hasZ) {
                m_minZ = other.m_minZ;
                m_maxZ = other.m_maxZ;
                m_hasZ = true;
            } else {
                m_minZ = std::min(m_minZ, other.m_minZ);
                m_maxZ = std::max(m_maxZ, other.m_maxZ);
            }
        }
    }
    
    void ExpandBy(double deltaX, double deltaY, double deltaZ) noexcept {
        Envelope::ExpandBy(deltaX, deltaY);
        
        if (m_hasZ) {
            m_minZ -= deltaZ;
            m_maxZ += deltaZ;
        }
    }
    
    bool Contains(const Coordinate& coord) const noexcept {
        if (!Envelope::Contains(coord)) return false;
        
        if (m_hasZ && coord.Is3D()) {
            return coord.z >= m_minZ && coord.z <= m_maxZ;
        }
        
        return true;
    }
    
    bool Contains(const Envelope3D& other) const noexcept {
        if (!Envelope::Contains(static_cast<const Envelope&>(other))) return false;
        
        if (m_hasZ && other.m_hasZ) {
            return other.m_minZ >= m_minZ && other.m_maxZ <= m_maxZ;
        }
        
        return true;
    }
    
    bool Intersects(const Envelope3D& other) const noexcept {
        if (!Envelope::Intersects(static_cast<const Envelope&>(other))) return false;
        
        if (m_hasZ && other.m_hasZ) {
            return !(other.m_maxZ < m_minZ || other.m_minZ > m_maxZ);
        }
        
        return true;
    }
    
    bool IntersectsZ(double z) const noexcept {
        if (!m_hasZ) return true;
        return z >= m_minZ && z <= m_maxZ;
    }
    
    Envelope3D Intersection(const Envelope3D& other) const noexcept {
        if (!Intersects(other)) return Envelope3D();
        
        Envelope3D result;
        result.m_minX = std::max(GetMinX(), other.GetMinX());
        result.m_maxX = std::min(GetMaxX(), other.GetMaxX());
        result.m_minY = std::max(GetMinY(), other.GetMinY());
        result.m_maxY = std::min(GetMaxY(), other.GetMaxY());
        result.m_isNull = false;
        
        if (m_hasZ && other.m_hasZ) {
            result.m_minZ = std::max(m_minZ, other.m_minZ);
            result.m_maxZ = std::min(m_maxZ, other.m_maxZ);
            result.m_hasZ = true;
        }
        
        return result;
    }
    
    Envelope3D Union(const Envelope3D& other) const noexcept {
        if (IsNull()) return other;
        if (other.IsNull()) return *this;
        
        Envelope3D result;
        result.m_minX = std::min(GetMinX(), other.GetMinX());
        result.m_maxX = std::max(GetMaxX(), other.GetMaxX());
        result.m_minY = std::min(GetMinY(), other.GetMinY());
        result.m_maxY = std::max(GetMaxY(), other.GetMaxY());
        result.m_isNull = false;
        
        if (m_hasZ || other.m_hasZ) {
            result.m_hasZ = true;
            if (m_hasZ && other.m_hasZ) {
                result.m_minZ = std::min(m_minZ, other.m_minZ);
                result.m_maxZ = std::max(m_maxZ, other.m_maxZ);
            } else if (m_hasZ) {
                result.m_minZ = m_minZ;
                result.m_maxZ = m_maxZ;
            } else {
                result.m_minZ = other.m_minZ;
                result.m_maxZ = other.m_maxZ;
            }
        }
        
        return result;
    }
    
    double Distance(const Coordinate& coord) const noexcept {
        double dist2D = Envelope::Distance(coord);
        
        if (!m_hasZ || !coord.Is3D()) {
            return dist2D;
        }
        
        double dz = 0.0;
        if (coord.z < m_minZ) dz = m_minZ - coord.z;
        else if (coord.z > m_maxZ) dz = coord.z - m_maxZ;
        
        if (dist2D == 0.0) return dz;
        if (dz == 0.0) return dist2D;
        
        return std::sqrt(dist2D * dist2D + dz * dz);
    }
    
    double Distance3D(const Envelope3D& other) const noexcept {
        double dist2D = Envelope::Distance(static_cast<const Envelope&>(other));
        
        if (!m_hasZ || !other.m_hasZ) {
            return dist2D;
        }
        
        double dz = 0.0;
        if (m_maxZ < other.m_minZ) dz = other.m_minZ - m_maxZ;
        else if (m_minZ > other.m_maxZ) dz = m_minZ - other.m_maxZ;
        
        if (dist2D == 0.0) return dz;
        if (dz == 0.0) return dist2D;
        
        return std::sqrt(dist2D * dist2D + dz * dz);
    }
    
    Coordinate GetMinCoordinate() const noexcept {
        if (IsNull()) return Coordinate::Empty();
        if (m_hasZ) return Coordinate(GetMinX(), GetMinY(), m_minZ);
        return Coordinate(GetMinX(), GetMinY());
    }
    
    Coordinate GetMaxCoordinate() const noexcept {
        if (IsNull()) return Coordinate::Empty();
        if (m_hasZ) return Coordinate(GetMaxX(), GetMaxY(), m_maxZ);
        return Coordinate(GetMaxX(), GetMaxY());
    }
    
    bool Equals(const Envelope3D& other, double tolerance = DEFAULT_TOLERANCE) const noexcept {
        if (!Envelope::Equals(other, tolerance)) return false;
        
        if (m_hasZ != other.m_hasZ) return false;
        if (!m_hasZ) return true;
        
        return std::abs(m_minZ - other.m_minZ) <= tolerance &&
               std::abs(m_maxZ - other.m_maxZ) <= tolerance;
    }
    
    std::string ToWKT() const;
    std::string ToString() const;
    
    bool operator==(const Envelope3D& rhs) const noexcept { return Equals(rhs); }
    bool operator!=(const Envelope3D& rhs) const noexcept { return !Equals(rhs); }
    
private:
    double m_minZ;
    double m_maxZ;
    bool m_hasZ;
    
    void NormalizeZ() noexcept {
        if (m_minZ > m_maxZ) std::swap(m_minZ, m_maxZ);
    }
};

}
