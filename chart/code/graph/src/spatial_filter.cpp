#include "ogc/draw/spatial_filter.h"
#include "ogc/feature/feature.h"
#include <sstream>

namespace ogc {
namespace draw {

SpatialFilter::SpatialFilter(SpatialOperator op, const Geometry* geometry)
    : m_operator(op)
    , m_hasEnvelope(false)
{
    if (geometry) {
        m_geometry = std::shared_ptr<Geometry>(geometry->Clone());
    }
}

SpatialFilter::SpatialFilter(SpatialOperator op, const Envelope& envelope)
    : m_operator(op)
    , m_envelope(envelope)
    , m_hasEnvelope(true)
{
}

SpatialFilter::SpatialFilter(SpatialOperator op, std::shared_ptr<Geometry> geometry)
    : m_operator(op)
    , m_geometry(geometry)
    , m_hasEnvelope(false)
{
}

bool SpatialFilter::Evaluate(const CNFeature* feature) const {
    if (!feature) {
        return false;
    }
    
    GeometryPtr geom = feature->GetGeometry();
    if (!geom) {
        return false;
    }
    
    return EvaluateGeometry(geom.get());
}

bool SpatialFilter::Evaluate(const Geometry* geometry) const {
    if (!geometry) {
        return false;
    }
    
    return EvaluateGeometry(geometry);
}

std::string SpatialFilter::ToString() const {
    std::ostringstream oss;
    oss << OperatorToString(m_operator) << "(";
    
    if (m_hasEnvelope) {
        oss << "BBOX(" << m_envelope.GetMinX() << "," 
            << m_envelope.GetMinY() << ","
            << m_envelope.GetMaxX() << ","
            << m_envelope.GetMaxY() << ")";
    } else if (m_geometry) {
        oss << "Geometry";
    }
    
    oss << ")";
    return oss.str();
}

FilterPtr SpatialFilter::Clone() const {
    if (m_hasEnvelope) {
        return std::make_shared<SpatialFilter>(m_operator, m_envelope);
    } else if (m_geometry) {
        return std::make_shared<SpatialFilter>(m_operator, m_geometry);
    }
    return std::make_shared<SpatialFilter>(m_operator, nullptr);
}

void SpatialFilter::SetGeometry(const Geometry* geometry) {
    if (geometry) {
        m_geometry = std::shared_ptr<Geometry>(geometry->Clone());
    } else {
        m_geometry.reset();
    }
    m_hasEnvelope = false;
}

void SpatialFilter::SetGeometry(std::shared_ptr<Geometry> geometry) {
    m_geometry = geometry;
    m_hasEnvelope = false;
}

void SpatialFilter::SetEnvelope(const Envelope& envelope) {
    m_envelope = envelope;
    m_hasEnvelope = true;
    m_geometry.reset();
}

bool SpatialFilter::EvaluateGeometry(const Geometry* testGeom) const {
    if (!testGeom) {
        return false;
    }
    
    if (m_hasEnvelope) {
        return EvaluateEnvelope(testGeom);
    }
    
    if (!m_geometry) {
        return false;
    }
    
    switch (m_operator) {
        case SpatialOperator::kBbox:
        case SpatialOperator::kIntersects:
            return testGeom->Intersects(m_geometry.get());
            
        case SpatialOperator::kWithin:
            return testGeom->Within(m_geometry.get());
            
        case SpatialOperator::kContains:
            return testGeom->Contains(m_geometry.get());
            
        case SpatialOperator::kEquals:
            return testGeom->Equals(m_geometry.get());
            
        case SpatialOperator::kOverlaps:
            return testGeom->Overlaps(m_geometry.get());
            
        case SpatialOperator::kTouches:
            return testGeom->Touches(m_geometry.get());
            
        case SpatialOperator::kCrosses:
            return testGeom->Crosses(m_geometry.get());
            
        case SpatialOperator::kDisjoint:
            return testGeom->Disjoint(m_geometry.get());
            
        default:
            return false;
    }
}

bool SpatialFilter::EvaluateEnvelope(const Geometry* testGeom) const {
    if (!testGeom) {
        return false;
    }
    
    Envelope testEnv = testGeom->GetEnvelope();
    
    switch (m_operator) {
        case SpatialOperator::kBbox:
        case SpatialOperator::kIntersects:
            return testEnv.Intersects(m_envelope);
            
        case SpatialOperator::kWithin:
            return m_envelope.Contains(testEnv);
            
        case SpatialOperator::kContains:
            return testEnv.Contains(m_envelope);
            
        case SpatialOperator::kEquals:
            return testEnv.Equals(m_envelope);
            
        case SpatialOperator::kOverlaps:
            return testEnv.Intersects(m_envelope) && 
                   !testEnv.Contains(m_envelope) &&
                   !m_envelope.Contains(testEnv);
            
        case SpatialOperator::kTouches: {
            bool touchesX = (testEnv.GetMinX() == m_envelope.GetMaxX()) ||
                           (testEnv.GetMaxX() == m_envelope.GetMinX());
            bool touchesY = (testEnv.GetMinY() == m_envelope.GetMaxY()) ||
                           (testEnv.GetMaxY() == m_envelope.GetMinY());
            return testEnv.Intersects(m_envelope) && (touchesX || touchesY);
        }
            
        case SpatialOperator::kCrosses:
            return testEnv.Intersects(m_envelope);
            
        case SpatialOperator::kDisjoint:
            return !testEnv.Intersects(m_envelope);
            
        default:
            return false;
    }
}

std::string SpatialFilter::OperatorToString(SpatialOperator op) {
    switch (op) {
        case SpatialOperator::kBbox: return "BBOX";
        case SpatialOperator::kIntersects: return "Intersects";
        case SpatialOperator::kWithin: return "Within";
        case SpatialOperator::kContains: return "Contains";
        case SpatialOperator::kEquals: return "Equals";
        case SpatialOperator::kOverlaps: return "Overlaps";
        case SpatialOperator::kTouches: return "Touches";
        case SpatialOperator::kCrosses: return "Crosses";
        case SpatialOperator::kDisjoint: return "Disjoint";
        default: return "?";
    }
}

SpatialOperator SpatialFilter::StringToOperator(const std::string& str) {
    if (str == "BBOX" || str == "bbox") {
        return SpatialOperator::kBbox;
    } else if (str == "Intersects" || str == "intersects") {
        return SpatialOperator::kIntersects;
    } else if (str == "Within" || str == "within") {
        return SpatialOperator::kWithin;
    } else if (str == "Contains" || str == "contains") {
        return SpatialOperator::kContains;
    } else if (str == "Equals" || str == "equals") {
        return SpatialOperator::kEquals;
    } else if (str == "Overlaps" || str == "overlaps") {
        return SpatialOperator::kOverlaps;
    } else if (str == "Touches" || str == "touches") {
        return SpatialOperator::kTouches;
    } else if (str == "Crosses" || str == "crosses") {
        return SpatialOperator::kCrosses;
    } else if (str == "Disjoint" || str == "disjoint") {
        return SpatialOperator::kDisjoint;
    }
    return SpatialOperator::kIntersects;
}

}
}
