#include "ogc/symbology/filter/spatial_filter.h"
#include "ogc/feature/feature.h"
#include <sstream>

namespace ogc {
namespace symbology {

struct SpatialFilter::Impl {
    SpatialOperator op = SpatialOperator::kIntersects;
    std::shared_ptr<Geometry> geometry;
    Envelope envelope;
    bool hasEnvelope = false;
    std::string propertyName;
};

SpatialFilter::SpatialFilter(SpatialOperator op, const Geometry* geometry)
    : impl_(std::make_unique<Impl>())
{
    impl_->op = op;
    if (geometry) {
        impl_->geometry = std::shared_ptr<Geometry>(geometry->Clone());
    }
}

SpatialFilter::SpatialFilter(SpatialOperator op, const Envelope& envelope)
    : impl_(std::make_unique<Impl>())
{
    impl_->op = op;
    impl_->envelope = envelope;
    impl_->hasEnvelope = true;
}

SpatialFilter::SpatialFilter(SpatialOperator op, std::shared_ptr<Geometry> geometry)
    : impl_(std::make_unique<Impl>())
{
    impl_->op = op;
    impl_->geometry = geometry;
}

SpatialFilter::~SpatialFilter() = default;

SpatialOperator SpatialFilter::GetOperator() const {
    return impl_->op;
}

void SpatialFilter::SetOperator(SpatialOperator op) {
    impl_->op = op;
}

const Geometry* SpatialFilter::GetGeometry() const {
    return impl_->geometry.get();
}

void SpatialFilter::SetGeometry(const Geometry* geometry) {
    if (geometry) {
        impl_->geometry = std::shared_ptr<Geometry>(geometry->Clone());
    } else {
        impl_->geometry.reset();
    }
    impl_->hasEnvelope = false;
}

void SpatialFilter::SetGeometry(std::shared_ptr<Geometry> geometry) {
    impl_->geometry = geometry;
    impl_->hasEnvelope = false;
}

const Envelope& SpatialFilter::GetEnvelope() const {
    return impl_->envelope;
}

void SpatialFilter::SetEnvelope(const Envelope& envelope) {
    impl_->envelope = envelope;
    impl_->hasEnvelope = true;
    impl_->geometry.reset();
}

bool SpatialFilter::HasGeometry() const {
    return impl_->geometry != nullptr;
}

bool SpatialFilter::HasEnvelope() const {
    return impl_->hasEnvelope;
}

void SpatialFilter::SetPropertyName(const std::string& name) {
    impl_->propertyName = name;
}

std::string SpatialFilter::GetPropertyName() const {
    return impl_->propertyName;
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
    oss << OperatorToString(impl_->op) << "(";
    
    if (impl_->hasEnvelope) {
        oss << "BBOX(" << impl_->envelope.GetMinX() << "," 
            << impl_->envelope.GetMinY() << ","
            << impl_->envelope.GetMaxX() << ","
            << impl_->envelope.GetMaxY() << ")";
    } else if (impl_->geometry) {
        oss << "Geometry";
    }
    
    oss << ")";
    return oss.str();
}

FilterPtr SpatialFilter::Clone() const {
    if (impl_->hasEnvelope) {
        return std::make_shared<SpatialFilter>(impl_->op, impl_->envelope);
    } else if (impl_->geometry) {
        return std::make_shared<SpatialFilter>(impl_->op, impl_->geometry);
    }
    return std::make_shared<SpatialFilter>(impl_->op, nullptr);
}

bool SpatialFilter::EvaluateGeometry(const Geometry* testGeom) const {
    if (!testGeom) {
        return false;
    }
    
    if (impl_->hasEnvelope) {
        return EvaluateEnvelope(testGeom);
    }
    
    if (!impl_->geometry) {
        return false;
    }
    
    switch (impl_->op) {
        case SpatialOperator::kBbox:
        case SpatialOperator::kIntersects:
            return testGeom->Intersects(impl_->geometry.get());
            
        case SpatialOperator::kWithin:
            return testGeom->Within(impl_->geometry.get());
            
        case SpatialOperator::kContains:
            return testGeom->Contains(impl_->geometry.get());
            
        case SpatialOperator::kEquals:
            return testGeom->Equals(impl_->geometry.get());
            
        case SpatialOperator::kOverlaps:
            return testGeom->Overlaps(impl_->geometry.get());
            
        case SpatialOperator::kTouches:
            return testGeom->Touches(impl_->geometry.get());
            
        case SpatialOperator::kCrosses:
            return testGeom->Crosses(impl_->geometry.get());
            
        case SpatialOperator::kDisjoint:
            return testGeom->Disjoint(impl_->geometry.get());
            
        default:
            return false;
    }
}

bool SpatialFilter::EvaluateEnvelope(const Geometry* testGeom) const {
    if (!testGeom) {
        return false;
    }
    
    Envelope testEnv = testGeom->GetEnvelope();
    
    switch (impl_->op) {
        case SpatialOperator::kBbox:
        case SpatialOperator::kIntersects:
            return testEnv.Intersects(impl_->envelope);
            
        case SpatialOperator::kWithin:
            return impl_->envelope.Contains(testEnv);
            
        case SpatialOperator::kContains:
            return testEnv.Contains(impl_->envelope);
            
        case SpatialOperator::kEquals:
            return testEnv.Equals(impl_->envelope);
            
        case SpatialOperator::kOverlaps:
            return testEnv.Intersects(impl_->envelope) && 
                   !testEnv.Contains(impl_->envelope) &&
                   !impl_->envelope.Contains(testEnv);
            
        case SpatialOperator::kTouches: {
            bool touchesX = (testEnv.GetMinX() == impl_->envelope.GetMaxX()) ||
                           (testEnv.GetMaxX() == impl_->envelope.GetMinX());
            bool touchesY = (testEnv.GetMinY() == impl_->envelope.GetMaxY()) ||
                           (testEnv.GetMaxY() == impl_->envelope.GetMinY());
            return testEnv.Intersects(impl_->envelope) && (touchesX || touchesY);
        }
            
        case SpatialOperator::kCrosses:
            return testEnv.Intersects(impl_->envelope);
            
        case SpatialOperator::kDisjoint:
            return !testEnv.Intersects(impl_->envelope);
            
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
