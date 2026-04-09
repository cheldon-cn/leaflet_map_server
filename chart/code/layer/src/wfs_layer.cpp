#include "ogc/layer/wfs_layer.h"
#include "ogc/layer/geometry_compat.h"

#include "ogc/feature/feature.h"
#include "ogc/feature/field_defn.h"
#include "ogc/geom/geometry.h"

#include <fstream>
#include <sstream>

namespace ogc {

class CNWFSLayer::Impl {
public:
    CNWFSConnectionParams params_;
    std::string type_name_;
    std::shared_ptr<CNFeatureDefn> feature_defn_;
    void* spatial_ref_ = nullptr;
    Envelope extent_;
    bool extent_loaded_ = false;
    bool preloaded_ = false;
    std::vector<std::unique_ptr<CNFeature>> features_;
    size_t read_cursor_ = 0;
    std::unique_ptr<CNGeometry> spatial_filter_;
    Envelope filter_extent_;
    std::string attribute_filter_;
    std::string bbox_crs_;
    std::string sort_by_property_;
    bool sort_ascending_ = true;
    std::string output_format_;
};

CNWFSLayer::CNWFSLayer()
    : impl_(new Impl()) {
}

CNWFSLayer::~CNWFSLayer() = default;

std::unique_ptr<CNWFSLayer> CNWFSLayer::Connect(
    const CNWFSConnectionParams& params,
    const std::string& type_name) {
    (void)params;
    (void)type_name;
    return nullptr;
}

std::string CNWFSLayer::GetCapabilities(const std::string& url) {
    (void)url;
    return "";
}

std::vector<std::string> CNWFSLayer::ListLayers(const std::string& url) {
    (void)url;
    return {};
}

const std::string& CNWFSLayer::GetName() const {
    return impl_->type_name_;
}

CNLayerType CNWFSLayer::GetLayerType() const {
    return CNLayerType::kWFS;
}

CNFeatureDefn* CNWFSLayer::GetFeatureDefn() {
    return impl_->feature_defn_.get();
}

const CNFeatureDefn* CNWFSLayer::GetFeatureDefn() const {
    return impl_->feature_defn_.get();
}

GeomType CNWFSLayer::GetGeomType() const {
    if (impl_->feature_defn_ && impl_->feature_defn_->GetGeomFieldCount() > 0) {
        return impl_->feature_defn_->GetGeomFieldDefn(0)->GetGeomType();
    }
    return GeomType::kUnknown;
}

void* CNWFSLayer::GetSpatialRef() {
    return impl_->spatial_ref_;
}

const void* CNWFSLayer::GetSpatialRef() const {
    return impl_->spatial_ref_;
}

CNStatus CNWFSLayer::GetExtent(Envelope& extent, bool force) const {
    (void)force;
    extent = impl_->extent_;
    return CNStatus::kSuccess;
}

int64_t CNWFSLayer::GetFeatureCount(bool force) const {
    (void)force;
    return static_cast<int64_t>(impl_->features_.size());
}

void CNWFSLayer::ResetReading() {
    impl_->read_cursor_ = 0;
}

std::unique_ptr<CNFeature> CNWFSLayer::GetNextFeature() {
    CNFeature* feature = GetNextFeatureRef();
    if (!feature) {
        return nullptr;
    }
    return std::unique_ptr<CNFeature>(feature->Clone());
}

CNFeature* CNWFSLayer::GetNextFeatureRef() {
    if (impl_->read_cursor_ >= impl_->features_.size()) {
        return nullptr;
    }
    return impl_->features_[impl_->read_cursor_++].get();
}

std::unique_ptr<CNFeature> CNWFSLayer::GetFeature(int64_t fid) {
    for (auto& f : impl_->features_) {
        if (f->GetFID() == fid) {
            return std::unique_ptr<CNFeature>(f->Clone());
        }
    }
    return nullptr;
}

void CNWFSLayer::SetSpatialFilterRect(double min_x, double min_y,
                                       double max_x, double max_y) {
    impl_->filter_extent_ = Envelope(min_x, max_x, min_y, max_y);
}

void CNWFSLayer::SetSpatialFilter(const CNGeometry* geometry) {
    if (geometry) {
        impl_->spatial_filter_.reset(geometry->Clone().release());
    } else {
        impl_->spatial_filter_.reset();
    }
}

const CNGeometry* CNWFSLayer::GetSpatialFilter() const {
    return impl_->spatial_filter_.get();
}

CNStatus CNWFSLayer::SetAttributeFilter(const std::string& query) {
    impl_->attribute_filter_ = query;
    return CNStatus::kSuccess;
}

bool CNWFSLayer::TestCapability(CNLayerCapability capability) const {
    switch (capability) {
        case CNLayerCapability::kRandomRead:
        case CNLayerCapability::kFastFeatureCount:
        case CNLayerCapability::kFastGetExtent:
        case CNLayerCapability::kSequentialRead:
            return true;
        default:
            return false;
    }
}

std::string CNWFSLayer::GetServiceURL() const {
    return impl_->params_.url;
}

std::string CNWFSLayer::GetTypeName() const {
    return impl_->type_name_;
}

void CNWFSLayer::SetBBOXCRS(const std::string& crs) {
    impl_->bbox_crs_ = crs;
}

void CNWFSLayer::SetSortBy(const std::string& property, bool ascending) {
    impl_->sort_by_property_ = property;
    impl_->sort_ascending_ = ascending;
}

void CNWFSLayer::SetOutputFormat(const std::string& format) {
    impl_->output_format_ = format;
}

CNStatus CNWFSLayer::PreloadAll() {
    impl_->preloaded_ = true;
    return CNStatus::kSuccess;
}

} // namespace ogc
