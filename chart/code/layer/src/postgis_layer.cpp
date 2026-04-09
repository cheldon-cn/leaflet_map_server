#include "ogc/layer/postgis_layer.h"
#include "ogc/layer/geometry_compat.h"

#include "ogc/feature/feature.h"
#include "ogc/feature/field_defn.h"
#include "ogc/geom/geometry.h"

#include <fstream>
#include <sstream>
#include <unordered_map>

namespace ogc {

class CNPostGISLayer::Impl {
public:
    CNPostGISConnectionParams params_;
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
    void* connection_ = nullptr;
    bool in_transaction_ = false;
};

CNPostGISLayer::CNPostGISLayer()
    : impl_(new Impl()) {
}

CNPostGISLayer::~CNPostGISLayer() = default;

std::unique_ptr<CNVectorLayer> CNPostGISLayer::Open(
    const CNPostGISConnectionParams& params,
    bool update) {
    (void)params;
    (void)update;
    return nullptr;
}

std::unique_ptr<CNVectorLayer> CNPostGISLayer::Create(
    const CNPostGISConnectionParams& params,
    GeomType geom_type) {
    (void)params;
    (void)geom_type;
    return nullptr;
}

const std::string& CNPostGISLayer::GetName() const {
    return impl_->name_;
}

std::string CNPostGISLayer::GetDataSourcePath() const {
    std::ostringstream oss;
    oss << impl_->params_.host << ":" << impl_->params_.port
        << "/" << impl_->params_.database;
    return oss.str();
}

std::string CNPostGISLayer::GetFormatName() const {
    return "PostGIS";
}

bool CNPostGISLayer::IsReadOnly() const {
    return impl_->is_read_only_;
}

CNFeatureDefn* CNPostGISLayer::GetFeatureDefn() {
    return impl_->feature_defn_.get();
}

const CNFeatureDefn* CNPostGISLayer::GetFeatureDefn() const {
    return impl_->feature_defn_.get();
}

GeomType CNPostGISLayer::GetGeomType() const {
    if (impl_->feature_defn_ && impl_->feature_defn_->GetGeomFieldCount() > 0) {
        return impl_->feature_defn_->GetGeomFieldDefn(0)->GetGeomType();
    }
    return GeomType::kUnknown;
}

void* CNPostGISLayer::GetSpatialRef() {
    return impl_->spatial_ref_;
}

const void* CNPostGISLayer::GetSpatialRef() const {
    return impl_->spatial_ref_;
}

CNStatus CNPostGISLayer::GetExtent(Envelope& extent, bool force) const {
    (void)force;
    extent = impl_->extent_;
    return CNStatus::kSuccess;
}

int64_t CNPostGISLayer::GetFeatureCount(bool force) const {
    (void)force;
    return static_cast<int64_t>(impl_->features_.size());
}

void CNPostGISLayer::ResetReading() {
    impl_->read_cursor_ = 0;
}

std::unique_ptr<CNFeature> CNPostGISLayer::GetNextFeature() {
    CNFeature* feature = GetNextFeatureRef();
    if (!feature) {
        return nullptr;
    }
    return std::unique_ptr<CNFeature>(feature->Clone());
}

CNFeature* CNPostGISLayer::GetNextFeatureRef() {
    if (impl_->read_cursor_ >= impl_->features_.size()) {
        return nullptr;
    }
    return impl_->features_[impl_->read_cursor_++].get();
}

std::unique_ptr<CNFeature> CNPostGISLayer::GetFeature(int64_t fid) {
    auto it = impl_->fid_index_.find(fid);
    if (it == impl_->fid_index_.end()) {
        return nullptr;
    }
    return std::unique_ptr<CNFeature>(impl_->features_[it->second]->Clone());
}

CNStatus CNPostGISLayer::SetFeature(const CNFeature* feature) {
    (void)feature;
    return CNStatus::kNotSupported;
}

CNStatus CNPostGISLayer::CreateFeature(CNFeature* feature) {
    (void)feature;
    return CNStatus::kNotSupported;
}

CNStatus CNPostGISLayer::DeleteFeature(int64_t fid) {
    (void)fid;
    return CNStatus::kNotSupported;
}

CNStatus CNPostGISLayer::CreateField(const CNFieldDefn* field_defn, bool approx_ok) {
    (void)field_defn;
    (void)approx_ok;
    return CNStatus::kNotSupported;
}

CNStatus CNPostGISLayer::DeleteField(int field_index) {
    (void)field_index;
    return CNStatus::kNotSupported;
}

void CNPostGISLayer::SetSpatialFilter(const CNGeometry* geometry) {
    if (geometry) {
        impl_->spatial_filter_.reset(geometry->Clone().release());
    } else {
        impl_->spatial_filter_.reset();
    }
}

const CNGeometry* CNPostGISLayer::GetSpatialFilter() const {
    return impl_->spatial_filter_.get();
}

CNStatus CNPostGISLayer::SetAttributeFilter(const std::string& query) {
    impl_->attribute_filter_ = query;
    return CNStatus::kSuccess;
}

CNStatus CNPostGISLayer::StartTransaction() {
    if (impl_->in_transaction_) {
        return CNStatus::kTransactionActive;
    }
    impl_->in_transaction_ = true;
    return CNStatus::kSuccess;
}

CNStatus CNPostGISLayer::CommitTransaction() {
    if (!impl_->in_transaction_) {
        return CNStatus::kTransactionNotActive;
    }
    impl_->in_transaction_ = false;
    return CNStatus::kSuccess;
}

CNStatus CNPostGISLayer::RollbackTransaction() {
    if (!impl_->in_transaction_) {
        return CNStatus::kTransactionNotActive;
    }
    impl_->in_transaction_ = false;
    return CNStatus::kSuccess;
}

bool CNPostGISLayer::TestCapability(CNLayerCapability capability) const {
    switch (capability) {
        case CNLayerCapability::kRandomRead:
        case CNLayerCapability::kFastFeatureCount:
        case CNLayerCapability::kFastGetExtent:
        case CNLayerCapability::kTransactions:
            return true;
        case CNLayerCapability::kSequentialWrite:
        case CNLayerCapability::kRandomWrite:
        case CNLayerCapability::kCreateFeature:
        case CNLayerCapability::kDeleteFeature:
            return !impl_->is_read_only_;
        default:
            return false;
    }
}

std::unique_ptr<CNLayer> CNPostGISLayer::Clone() const {
    return nullptr;
}

} // namespace ogc
