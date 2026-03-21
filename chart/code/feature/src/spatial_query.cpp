#include "ogc/feature/spatial_query.h"

namespace ogc {

struct CNSpatialQuery::Impl {
    GeometryPtr geometry_;
    SpatialOperation operation_;
    double buffer_distance_;
    int srid_;
    
    Impl() : operation_(SpatialOperation::kIntersects), buffer_distance_(0.0), srid_(0) {}
};

CNSpatialQuery::CNSpatialQuery()
    : impl_(new Impl()) {
}

CNSpatialQuery::~CNSpatialQuery() {
}

CNSpatialQuery::CNSpatialQuery(CNSpatialQuery&& other) noexcept
    : impl_(std::move(other.impl_)) {
}

CNSpatialQuery& CNSpatialQuery::operator=(CNSpatialQuery&& other) noexcept {
    if (this != &other) {
        impl_ = std::move(other.impl_);
    }
    return *this;
}

CNSpatialQuery& CNSpatialQuery::SetGeometry(GeometryPtr geometry) {
    impl_->geometry_ = std::move(geometry);
    return *this;
}

CNSpatialQuery& CNSpatialQuery::SetOperation(SpatialOperation op) {
    impl_->operation_ = op;
    return *this;
}

CNSpatialQuery& CNSpatialQuery::SetBufferDistance(double distance) {
    impl_->buffer_distance_ = distance;
    return *this;
}

CNSpatialQuery& CNSpatialQuery::SetSRID(int srid) {
    impl_->srid_ = srid;
    return *this;
}

GeometryPtr CNSpatialQuery::GetGeometry() const {
    if (!impl_->geometry_) return GeometryPtr();
    return impl_->geometry_->Clone();
}

SpatialOperation CNSpatialQuery::GetOperation() const {
    return impl_->operation_;
}

double CNSpatialQuery::GetBufferDistance() const {
    return impl_->buffer_distance_;
}

int CNSpatialQuery::GetSRID() const {
    return impl_->srid_;
}

std::string CNSpatialQuery::ToWKT() const {
    if (!impl_->geometry_) return std::string();
    
    const char* op_name = "";
    switch (impl_->operation_) {
        case SpatialOperation::kIntersects: op_name = "INTERSECTS"; break;
        case SpatialOperation::kContains: op_name = "CONTAINS"; break;
        case SpatialOperation::kWithin: op_name = "WITHIN"; break;
        case SpatialOperation::kOverlaps: op_name = "OVERLAPS"; break;
        case SpatialOperation::kTouches: op_name = "TOUCHES"; break;
        case SpatialOperation::kCrosses: op_name = "CROSSES"; break;
        case SpatialOperation::kDisjoint: op_name = "DISJOINT"; break;
        case SpatialOperation::kEquals: op_name = "EQUALS"; break;
    }
    
    return std::string(op_name) + "(" + impl_->geometry_->AsText() + ")";
}

CNSpatialQuery CNSpatialQuery::Intersects(GeometryPtr geometry) {
    CNSpatialQuery query;
    query.SetGeometry(std::move(geometry)).SetOperation(SpatialOperation::kIntersects);
    return std::move(query);
}

CNSpatialQuery CNSpatialQuery::Contains(GeometryPtr geometry) {
    CNSpatialQuery query;
    query.SetGeometry(std::move(geometry)).SetOperation(SpatialOperation::kContains);
    return std::move(query);
}

CNSpatialQuery CNSpatialQuery::Within(GeometryPtr geometry) {
    CNSpatialQuery query;
    query.SetGeometry(std::move(geometry)).SetOperation(SpatialOperation::kWithin);
    return std::move(query);
}

CNSpatialQuery CNSpatialQuery::Overlaps(GeometryPtr geometry) {
    CNSpatialQuery query;
    query.SetGeometry(std::move(geometry)).SetOperation(SpatialOperation::kOverlaps);
    return std::move(query);
}

CNSpatialQuery CNSpatialQuery::WithinDistance(GeometryPtr geometry, double distance) {
    CNSpatialQuery query;
    query.SetGeometry(std::move(geometry)).SetOperation(SpatialOperation::kIntersects).SetBufferDistance(distance);
    return std::move(query);
}

} // namespace ogc
