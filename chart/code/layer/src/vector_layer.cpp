#include "ogc/layer/vector_layer.h"

namespace ogc {

const void* CNVectorLayer::GetGeomFieldDefn(int index) const {
    (void)index;
    if (GetFeatureDefn() && GetFeatureDefn()->GetGeomFieldCount() > 0) {
        return GetFeatureDefn()->GetGeomFieldDefn(0);
    }
    return nullptr;
}

CNStatus CNVectorLayer::SetCoordinateTransform(const void* target_srs) {
    (void)target_srs;
    return CNStatus::kNotSupported;
}

void CNVectorLayer::ClearCoordinateTransform() {
    coord_transform_ = nullptr;
}

} // namespace ogc
