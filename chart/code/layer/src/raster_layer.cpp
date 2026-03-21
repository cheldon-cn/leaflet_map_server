#include "ogc/layer/raster_layer.h"

namespace ogc {

CNStatus CNRasterLayer::SetGeoTransform(const double* transform) {
    if (!transform) {
        return CNStatus::kNullPointer;
    }
    geo_transform_.assign(transform, transform + 6);
    return CNStatus::kSuccess;
}

void* CNRasterLayer::GetSpatialRef() {
    return spatial_ref_;
}

const void* CNRasterLayer::GetSpatialRef() const {
    return spatial_ref_;
}

} // namespace ogc
