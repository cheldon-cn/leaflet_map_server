#include "ogc/layer/geopackage_layer.h"
#include "ogc/layer/geometry_compat.h"

#include "ogc/feature/feature.h"
#include "ogc/feature/field_defn.h"
#include "ogc/geom/geometry.h"

#include <fstream>
#include <sstream>
#include <unordered_map>

namespace ogc {

class CNGeoPackageLayer::Impl {
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
    void* db_handle_ = nullptr;
};

CNGeoPackageLayer::CNGeoPackageLayer()
    : impl_(new Impl()) {
}

CNGeoPackageLayer::~CNGeoPackageLayer() = default;

std::unique_ptr<CNVectorLayer> CNGeoPackageLayer::Open(
    const std::string& path,
    const std::string& layer_name,
    bool update) {
    (void)path;
    (void)layer_name;
    (void)update;
    return nullptr;
}

std::unique_ptr<CNVectorLayer> CNGeoPackageLayer::Create(
    const std::string& path,
    const std::string& layer_name,
    GeomType geom_type) {
    (void)path;
    (void)layer_name;
    (void)geom_type;
    return nullptr;
}

const std::string& CNGeoPackageLayer::GetName() const {
    return impl_->name_;
}

std::string CNGeoPackageLayer::GetDataSourcePath() const {
    return impl_->path_;
}

std::string CNGeoPackageLayer::GetFormatName() const {
    return "GeoPackage";
}

bool CNGeoPackageLayer::IsReadOnly() const {
    return impl_->is_read_only_;
}

CNFeatureDefn* CNGeoPackageLayer::GetFeatureDefn() {
    return impl_->feature_defn_.get();
}

const CNFeatureDefn* CNGeoPackageLayer::GetFeatureDefn() const {
    return impl_->feature_defn_.get();
}

GeomType CNGeoPackageLayer::GetGeomType() const {
    if (impl_->feature_defn_ && impl_->feature_defn_->GetGeomFieldCount() > 0) {
        return impl_->feature_defn_->GetGeomFieldDefn(0)->GetGeomType();
    }
    return GeomType::kUnknown;
}

void* CNGeoPackageLayer::GetSpatialRef() {
    return impl_->spatial_ref_;
}

const void* CNGeoPackageLayer::GetSpatialRef() const {
    return impl_->spatial_ref_;
}

CNStatus CNGeoPackageLayer::GetExtent(Envelope& extent, bool force) const {
    (void)force;
    extent = impl_->extent_;
    return CNStatus::kSuccess;
}

int64_t CNGeoPackageLayer::GetFeatureCount(bool force) const {
    (void)force;
    return static_cast<int64_t>(impl_->features_.size());
}

void CNGeoPackageLayer::ResetReading() {
    impl_->read_cursor_ = 0;
}

std::unique_ptr<CNFeature> CNGeoPackageLayer::GetNextFeature() {
    CNFeature* feature = GetNextFeatureRef();
    if (!feature) {
        return nullptr;
    }
    return std::unique_ptr<CNFeature>(feature->Clone());
}

CNFeature* CNGeoPackageLayer::GetNextFeatureRef() {
    if (impl_->read_cursor_ >= impl_->features_.size()) {
        return nullptr;
    }
    return impl_->features_[impl_->read_cursor_++].get();
}

std::unique_ptr<CNFeature> CNGeoPackageLayer::GetFeature(int64_t fid) {
    auto it = impl_->fid_index_.find(fid);
    if (it == impl_->fid_index_.end()) {
        return nullptr;
    }
    return std::unique_ptr<CNFeature>(impl_->features_[it->second]->Clone());
}

CNStatus CNGeoPackageLayer::SetFeature(const CNFeature* feature) {
    (void)feature;
    return CNStatus::kNotSupported;
}

CNStatus CNGeoPackageLayer::CreateFeature(CNFeature* feature) {
    (void)feature;
    return CNStatus::kNotSupported;
}

CNStatus CNGeoPackageLayer::DeleteFeature(int64_t fid) {
    (void)fid;
    return CNStatus::kNotSupported;
}

CNStatus CNGeoPackageLayer::CreateField(const CNFieldDefn* field_defn, bool approx_ok) {
    (void)field_defn;
    (void)approx_ok;
    return CNStatus::kNotSupported;
}

CNStatus CNGeoPackageLayer::DeleteField(int field_index) {
    (void)field_index;
    return CNStatus::kNotSupported;
}

void CNGeoPackageLayer::SetSpatialFilter(const CNGeometry* geometry) {
    if (geometry) {
        impl_->spatial_filter_.reset(geometry->Clone().release());
    } else {
        impl_->spatial_filter_.reset();
    }
}

const CNGeometry* CNGeoPackageLayer::GetSpatialFilter() const {
    return impl_->spatial_filter_.get();
}

CNStatus CNGeoPackageLayer::SetAttributeFilter(const std::string& query) {
    impl_->attribute_filter_ = query;
    return CNStatus::kSuccess;
}

bool CNGeoPackageLayer::TestCapability(CNLayerCapability capability) const {
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

std::unique_ptr<CNLayer> CNGeoPackageLayer::Clone() const {
    return nullptr;
}

} // namespace ogc
