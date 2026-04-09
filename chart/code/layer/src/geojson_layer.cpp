#include "ogc/layer/geojson_layer.h"
#include "ogc/layer/geometry_compat.h"

#include "ogc/feature/feature.h"
#include "ogc/feature/field_defn.h"
#include "ogc/geom/geometry.h"

#include <fstream>
#include <sstream>
#include <unordered_map>

namespace ogc {

class CNGeoJSONLayer::Impl {
public:
    std::string path_;
    std::string name_;
    bool is_read_only_ = true;
    std::shared_ptr<CNFeatureDefn> feature_defn_;
    void* spatial_ref_ = nullptr;
    Envelope extent_;
    bool extent_loaded_ = false;
    std::vector<std::unique_ptr<CNFeature>> features_;
    std::unordered_map<int64_t, size_t> fid_index_;
    size_t read_cursor_ = 0;
    std::unique_ptr<CNGeometry> spatial_filter_;
    std::string attribute_filter_;
    int64_t next_fid_ = 1;
};

CNGeoJSONLayer::CNGeoJSONLayer()
    : impl_(new Impl()) {
}

CNGeoJSONLayer::~CNGeoJSONLayer() = default;

std::unique_ptr<CNVectorLayer> CNGeoJSONLayer::Open(
    const std::string& path, bool update) {
    (void)path;
    (void)update;
    return nullptr;
}

std::unique_ptr<CNVectorLayer> CNGeoJSONLayer::Create(
    const std::string& path) {
    (void)path;
    return nullptr;
}

const std::string& CNGeoJSONLayer::GetName() const {
    return impl_->name_;
}

std::string CNGeoJSONLayer::GetDataSourcePath() const {
    return impl_->path_;
}

std::string CNGeoJSONLayer::GetFormatName() const {
    return "GeoJSON";
}

bool CNGeoJSONLayer::IsReadOnly() const {
    return impl_->is_read_only_;
}

CNFeatureDefn* CNGeoJSONLayer::GetFeatureDefn() {
    return impl_->feature_defn_.get();
}

const CNFeatureDefn* CNGeoJSONLayer::GetFeatureDefn() const {
    return impl_->feature_defn_.get();
}

GeomType CNGeoJSONLayer::GetGeomType() const {
    if (impl_->feature_defn_ && impl_->feature_defn_->GetGeomFieldCount() > 0) {
        return impl_->feature_defn_->GetGeomFieldDefn(0)->GetGeomType();
    }
    return GeomType::kUnknown;
}

void* CNGeoJSONLayer::GetSpatialRef() {
    return impl_->spatial_ref_;
}

const void* CNGeoJSONLayer::GetSpatialRef() const {
    return impl_->spatial_ref_;
}

CNStatus CNGeoJSONLayer::GetExtent(Envelope& extent, bool force) const {
    (void)force;
    extent = impl_->extent_;
    return CNStatus::kSuccess;
}

int64_t CNGeoJSONLayer::GetFeatureCount(bool force) const {
    (void)force;
    return static_cast<int64_t>(impl_->features_.size());
}

void CNGeoJSONLayer::ResetReading() {
    impl_->read_cursor_ = 0;
}

std::unique_ptr<CNFeature> CNGeoJSONLayer::GetNextFeature() {
    CNFeature* feature = GetNextFeatureRef();
    if (!feature) {
        return nullptr;
    }
    return std::unique_ptr<CNFeature>(feature->Clone());
}

CNFeature* CNGeoJSONLayer::GetNextFeatureRef() {
    if (impl_->read_cursor_ >= impl_->features_.size()) {
        return nullptr;
    }
    return impl_->features_[impl_->read_cursor_++].get();
}

std::unique_ptr<CNFeature> CNGeoJSONLayer::GetFeature(int64_t fid) {
    auto it = impl_->fid_index_.find(fid);
    if (it == impl_->fid_index_.end()) {
        return nullptr;
    }
    return std::unique_ptr<CNFeature>(impl_->features_[it->second]->Clone());
}

CNStatus CNGeoJSONLayer::SetFeature(const CNFeature* feature) {
    (void)feature;
    return CNStatus::kNotSupported;
}

CNStatus CNGeoJSONLayer::CreateFeature(CNFeature* feature) {
    (void)feature;
    return CNStatus::kNotSupported;
}

CNStatus CNGeoJSONLayer::DeleteFeature(int64_t fid) {
    (void)fid;
    return CNStatus::kNotSupported;
}

CNStatus CNGeoJSONLayer::CreateField(const CNFieldDefn* field_defn, bool approx_ok) {
    (void)field_defn;
    (void)approx_ok;
    return CNStatus::kNotSupported;
}

void CNGeoJSONLayer::SetSpatialFilter(const CNGeometry* geometry) {
    if (geometry) {
        impl_->spatial_filter_.reset(geometry->Clone().release());
    } else {
        impl_->spatial_filter_.reset();
    }
}

const CNGeometry* CNGeoJSONLayer::GetSpatialFilter() const {
    return impl_->spatial_filter_.get();
}

CNStatus CNGeoJSONLayer::SetAttributeFilter(const std::string& query) {
    impl_->attribute_filter_ = query;
    return CNStatus::kSuccess;
}

bool CNGeoJSONLayer::TestCapability(CNLayerCapability capability) const {
    switch (capability) {
        case CNLayerCapability::kRandomRead:
        case CNLayerCapability::kFastFeatureCount:
        case CNLayerCapability::kFastGetExtent:
            return true;
        case CNLayerCapability::kSequentialWrite:
        case CNLayerCapability::kRandomWrite:
        case CNLayerCapability::kCreateFeature:
        case CNLayerCapability::kDeleteFeature:
        case CNLayerCapability::kTransactions:
            return !impl_->is_read_only_;
        default:
            return false;
    }
}

std::unique_ptr<CNLayer> CNGeoJSONLayer::Clone() const {
    return nullptr;
}

} // namespace ogc
