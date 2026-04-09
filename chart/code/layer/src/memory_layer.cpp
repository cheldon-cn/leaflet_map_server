#include "ogc/layer/memory_layer.h"

#include "ogc/feature/feature.h"
#include "ogc/feature/field_defn.h"
#include "ogc/geom/geometry.h"
#include "ogc/geom/envelope.h"
#include "ogc/geom/factory.h"

#include <algorithm>

namespace ogc {

CNMemoryLayer::CNMemoryLayer(
    const std::string& name,
    GeomType geom_type,
    void* srs)
    : name_(name)
    , spatial_ref_(srs) {
    feature_defn_ = std::shared_ptr<CNFeatureDefn>(CNFeatureDefn::Create(name.c_str()));
    auto geom_field = CreateCNGeomFieldDefn("geom");
    geom_field->SetGeomType(geom_type);
    feature_defn_->AddGeomFieldDefn(geom_field);
}

CNMemoryLayer::CNMemoryLayer(
    const std::string& name,
    std::shared_ptr<CNFeatureDefn> feature_defn)
    : name_(name)
    , feature_defn_(feature_defn) {
}

CNMemoryLayer::~CNMemoryLayer() = default;

CNFeatureDefn* CNMemoryLayer::GetFeatureDefn() {
    return feature_defn_.get();
}

const CNFeatureDefn* CNMemoryLayer::GetFeatureDefn() const {
    return feature_defn_.get();
}

GeomType CNMemoryLayer::GetGeomType() const {
    if (feature_defn_->GetGeomFieldCount() > 0) {
        return feature_defn_->GetGeomFieldDefn(0)->GetGeomType();
    }
    return GeomType::kUnknown;
}

void* CNMemoryLayer::GetSpatialRef() {
    return spatial_ref_;
}

const void* CNMemoryLayer::GetSpatialRef() const {
    return spatial_ref_;
}

CNStatus CNMemoryLayer::GetExtent(Envelope& extent, bool force) const {
    (void)force;
    if (extent_valid_) {
        extent = cached_extent_;
        return CNStatus::kSuccess;
    }

    if (features_.empty()) {
        extent = Envelope();
        return CNStatus::kSuccess;
    }

    bool first = true;
    for (const auto& feature : features_) {
        if (feature->GetGeometry()) {
            const Envelope& feature_env = feature->GetGeometry()->GetEnvelope();
            if (first) {
                extent = feature_env;
                first = false;
            } else {
                extent.ExpandToInclude(feature_env);
            }
        }
    }

    cached_extent_ = extent;
    extent_valid_ = true;
    return CNStatus::kSuccess;
}

int64_t CNMemoryLayer::GetFeatureCount(bool force) const {
    (void)force;
    int64_t count = 0;
    for (const auto& f : features_) {
        if (f) {
            ++count;
        }
    }
    return count;
}

void CNMemoryLayer::ResetReading() {
    read_cursor_ = 0;
    ApplySpatialFilter();
}

std::unique_ptr<CNFeature> CNMemoryLayer::GetNextFeature() {
    CNFeature* feature = GetNextFeatureRef();
    if (!feature) {
        return nullptr;
    }
    return std::unique_ptr<CNFeature>(feature->Clone());
}

CNFeature* CNMemoryLayer::GetNextFeatureRef() {
    if (filtered_indices_.empty()) {
        if (read_cursor_ >= features_.size()) {
            return nullptr;
        }
        CNFeature* feature = features_[read_cursor_].get();
        read_cursor_++;
        if (!PassesFilters(feature)) {
            return GetNextFeatureRef();
        }
        return feature;
    } else {
        if (read_cursor_ >= filtered_indices_.size()) {
            return nullptr;
        }
        size_t index = filtered_indices_[read_cursor_];
        read_cursor_++;
        return features_[index].get();
    }
}

std::unique_ptr<CNFeature> CNMemoryLayer::GetFeature(int64_t fid) {
    CNFeature* feature = GetFeatureRef(fid);
    if (!feature) {
        return nullptr;
    }
    return std::unique_ptr<CNFeature>(feature->Clone());
}

CNFeature* CNMemoryLayer::GetFeatureRef(int64_t fid) {
    auto it = fid_index_.find(fid);
    if (it != fid_index_.end()) {
        return features_[it->second].get();
    }
    return nullptr;
}

CNStatus CNMemoryLayer::SetFeature(const CNFeature* feature) {
    if (!feature) {
        return CNStatus::kNullPointer;
    }

    int64_t fid = feature->GetFID();
    auto it = fid_index_.find(fid);
    if (it == fid_index_.end()) {
        return CNStatus::kFeatureNotFound;
    }

    features_[it->second].reset(feature->Clone());
    InvalidateExtent();
    index_dirty_ = true;

    return CNStatus::kSuccess;
}

CNStatus CNMemoryLayer::CreateFeature(CNFeature* feature) {
    if (!feature) {
        return CNStatus::kNullPointer;
    }

    int64_t fid = feature->GetFID();
    if (fid_index_.find(fid) != fid_index_.end()) {
        if (auto_fid_generation_) {
            fid = GenerateFID();
            feature->SetFID(fid);
        } else {
            return CNStatus::kInvalidFID;
        }
    }

    size_t index = features_.size();
    features_.push_back(std::unique_ptr<CNFeature>(feature->Clone()));
    fid_index_[fid] = index;

    UpdateExtent(feature);
    index_dirty_ = true;

    return CNStatus::kSuccess;
}

CNStatus CNMemoryLayer::DeleteFeature(int64_t fid) {
    auto it = fid_index_.find(fid);
    if (it == fid_index_.end()) {
        return CNStatus::kFeatureNotFound;
    }

    size_t index = it->second;
    features_[index].reset();

    if (fid_reuse_) {
        deleted_fids_.insert(fid);
    }

    fid_index_.erase(it);
    InvalidateExtent();
    index_dirty_ = true;

    return CNStatus::kSuccess;
}

int64_t CNMemoryLayer::CreateFeatureBatch(
    const std::vector<CNFeature*>& features) {
    int64_t count = 0;
    for (auto* f : features) {
        if (CreateFeature(f) == CNStatus::kSuccess) {
            count++;
        }
    }
    return count;
}

CNStatus CNMemoryLayer::CreateField(
    const CNFieldDefn* field_defn,
    bool approx_ok) {
    (void)approx_ok;
    if (!field_defn) {
        return CNStatus::kNullPointer;
    }
    feature_defn_->AddFieldDefn(const_cast<CNFieldDefn*>(field_defn));
    return CNStatus::kSuccess;
}

CNStatus CNMemoryLayer::DeleteField(int field_index) {
    if (field_index < 0 ||
        static_cast<size_t>(field_index) >= feature_defn_->GetFieldCount()) {
        return CNStatus::kOutOfRange;
    }
    feature_defn_->DeleteFieldDefn(field_index);
    return CNStatus::kSuccess;
}

CNStatus CNMemoryLayer::ReorderFields(const std::vector<int>& new_order) {
    (void)new_order;
    return CNStatus::kNotSupported;
}

CNStatus CNMemoryLayer::AlterFieldDefn(
    int field_index,
    const CNFieldDefn* new_defn,
    int flags) {
    (void)field_index;
    (void)new_defn;
    (void)flags;
    return CNStatus::kNotSupported;
}

void CNMemoryLayer::SetSpatialFilterRect(
    double min_x, double min_y,
    double max_x, double max_y) {
    filter_extent_ = Envelope(min_x, min_y, max_x, max_y);
    ApplySpatialFilter();
}

void CNMemoryLayer::SetSpatialFilter(const CNGeometry* geometry) {
    if (geometry) {
        spatial_filter_ = geometry->Clone();
        filter_extent_ = geometry->GetEnvelope();
    } else {
        spatial_filter_.reset();
        filter_extent_ = Envelope();
        filter_extent_.SetNull();
    }
    ApplySpatialFilter();
}

const CNGeometry* CNMemoryLayer::GetSpatialFilter() const {
    return spatial_filter_.get();
}

CNStatus CNMemoryLayer::SetAttributeFilter(const std::string& query) {
    attribute_filter_ = query;
    ApplySpatialFilter();
    return CNStatus::kSuccess;
}

CNStatus CNMemoryLayer::StartTransaction() {
    if (in_transaction_) {
        return CNStatus::kTransactionActive;
    }

    TransactionSnapshot snapshot;
    snapshot.features.reserve(features_.size());
    for (const auto& f : features_) {
        if (f) {
            snapshot.features.push_back(std::unique_ptr<CNFeature>(f->Clone()));
        }
    }
    snapshot.fid_index = fid_index_;
    snapshot.next_fid = next_fid_;
    snapshot.deleted_fids = deleted_fids_;

    transaction_stack_.push(std::move(snapshot));
    in_transaction_ = true;

    return CNStatus::kSuccess;
}

CNStatus CNMemoryLayer::CommitTransaction() {
    if (!in_transaction_) {
        return CNStatus::kNoTransaction;
    }

    transaction_stack_.pop();
    in_transaction_ = false;

    return CNStatus::kSuccess;
}

CNStatus CNMemoryLayer::RollbackTransaction() {
    if (!in_transaction_) {
        return CNStatus::kNoTransaction;
    }

    if (transaction_stack_.empty()) {
        return CNStatus::kNoTransaction;
    }

    TransactionSnapshot& snapshot = transaction_stack_.top();
    features_ = std::move(snapshot.features);
    fid_index_ = std::move(snapshot.fid_index);
    next_fid_ = snapshot.next_fid;
    deleted_fids_ = std::move(snapshot.deleted_fids);

    transaction_stack_.pop();
    in_transaction_ = false;

    InvalidateExtent();
    index_dirty_ = true;

    return CNStatus::kSuccess;
}

bool CNMemoryLayer::TestCapability(CNLayerCapability capability) const {
    switch (capability) {
        case CNLayerCapability::kRandomRead:
        case CNLayerCapability::kFastSpatialFilter:
        case CNLayerCapability::kFastFeatureCount:
        case CNLayerCapability::kFastGetExtent:
        case CNLayerCapability::kSequentialWrite:
        case CNLayerCapability::kRandomWrite:
        case CNLayerCapability::kCreateFeature:
        case CNLayerCapability::kDeleteFeature:
        case CNLayerCapability::kCreateField:
        case CNLayerCapability::kDeleteField:
        case CNLayerCapability::kTransactions:
            return true;
        case CNLayerCapability::kReorderFields:
        case CNLayerCapability::kAlterFieldDefn:
        case CNLayerCapability::kStringsAsUTF8:
        case CNLayerCapability::kIgnoreFields:
        case CNLayerCapability::kCurveGeometries:
            return false;
        default:
            return false;
    }
}

std::unique_ptr<CNLayer> CNMemoryLayer::Clone() const {
    auto layer = std::unique_ptr<CNMemoryLayer>(
        new CNMemoryLayer(name_, feature_defn_));
    layer->features_.reserve(features_.size());
    for (const auto& f : features_) {
        if (f) {
            layer->features_.push_back(std::unique_ptr<CNFeature>(f->Clone()));
        }
    }
    layer->fid_index_ = fid_index_;
    layer->next_fid_ = next_fid_;
    layer->deleted_fids_ = deleted_fids_;
    layer->spatial_ref_ = spatial_ref_;
    return std::unique_ptr<CNLayer>(layer.release());
}

void CNMemoryLayer::Reserve(int64_t expected_count) {
    features_.reserve(static_cast<size_t>(expected_count));
}

void CNMemoryLayer::ShrinkToFit() {
    std::vector<std::unique_ptr<CNFeature>> new_features;
    new_features.reserve(features_.size());
    std::unordered_map<int64_t, size_t> new_index;

    for (size_t i = 0; i < features_.size(); ++i) {
        if (features_[i]) {
            new_features.push_back(std::move(features_[i]));
            for (const auto& pair : fid_index_) {
                if (pair.second == i) {
                    new_index[pair.first] = new_features.size() - 1;
                    break;
                }
            }
        }
    }

    features_ = std::move(new_features);
    fid_index_ = std::move(new_index);
}

void CNMemoryLayer::Clear() {
    features_.clear();
    fid_index_.clear();
    deleted_fids_.clear();
    next_fid_ = 1;
    read_cursor_ = 0;
    filtered_indices_.clear();
    InvalidateExtent();
    index_dirty_ = true;
}

int64_t CNMemoryLayer::GetMemoryUsage() const {
    return 0;
}

void CNMemoryLayer::SetAutoFIDGeneration(bool auto_gen) {
    auto_fid_generation_ = auto_gen;
}

void CNMemoryLayer::SetFIDReuse(bool reuse) {
    fid_reuse_ = reuse;
}

void CNMemoryLayer::BuildSpatialIndex(SpatialIndexType index_type) {
    (void)index_type;
    index_dirty_ = false;
}

ISpatialIndex<int64_t>* CNMemoryLayer::GetSpatialIndex() {
    return spatial_index_.get();
}

std::vector<int64_t> CNMemoryLayer::SpatialQuery(
    const CNGeometry* geometry,
    CNSpatialRelation relation) {
    (void)geometry;
    (void)relation;
    std::vector<int64_t> results;
    if (!geometry) {
        for (const auto& pair : fid_index_) {
            results.push_back(pair.first);
        }
        return results;
    }

    const Envelope& query_env = geometry->GetEnvelope();

    for (const auto& pair : fid_index_) {
        CNFeature* f = features_[pair.second].get();
        if (f && f->GetGeometry()) {
            const Envelope& feat_env = f->GetGeometry()->GetEnvelope();
            if (feat_env.Intersects(query_env)) {
                results.push_back(pair.first);
            }
        }
    }

    return results;
}

int64_t CNMemoryLayer::GenerateFID() {
    if (fid_reuse_ && !deleted_fids_.empty()) {
        int64_t reused_fid = *deleted_fids_.begin();
        deleted_fids_.erase(deleted_fids_.begin());
        return reused_fid;
    }

    while (fid_index_.find(next_fid_) != fid_index_.end()) {
        next_fid_++;
    }
    return next_fid_++;
}

void CNMemoryLayer::UpdateExtent(const CNFeature* feature) {
    if (feature && feature->GetGeometry()) {
        const Envelope& feature_env = feature->GetGeometry()->GetEnvelope();
        if (!extent_valid_) {
            cached_extent_ = feature_env;
            extent_valid_ = true;
        } else {
            cached_extent_.ExpandToInclude(feature_env);
        }
    }
}

void CNMemoryLayer::InvalidateExtent() {
    extent_valid_ = false;
}

void CNMemoryLayer::ApplySpatialFilter() {
    if (!spatial_filter_ && attribute_filter_.empty() && filter_extent_.IsNull()) {
        filtered_indices_.clear();
        return;
    }

    filtered_indices_.clear();
    for (size_t i = 0; i < features_.size(); ++i) {
        if (features_[i] && PassesFilters(features_[i].get())) {
            filtered_indices_.push_back(i);
        }
    }
}

bool CNMemoryLayer::PassesFilters(const CNFeature* feature) const {
    if (!feature) {
        return false;
    }

    if (!filter_extent_.IsNull() && feature->GetGeometry()) {
        const Envelope& feat_env = feature->GetGeometry()->GetEnvelope();
        if (!feat_env.Intersects(filter_extent_)) {
            return false;
        }
    }

    return true;
}

} // namespace ogc
