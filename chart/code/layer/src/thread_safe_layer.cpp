#include "ogc/layer/thread_safe_layer.h"

#include "ogc/feature/feature.h"
#include "ogc/feature/field_defn.h"

namespace ogc {

CNThreadSafeLayer::CNThreadSafeLayer(std::unique_ptr<CNLayer> layer)
    : layer_(std::move(layer)) {
}

const std::string& CNThreadSafeLayer::GetName() const {
    CNReadWriteLock::ReadGuard guard(lock_);
    return layer_->GetName();
}

CNLayerType CNThreadSafeLayer::GetLayerType() const {
    CNReadWriteLock::ReadGuard guard(lock_);
    return layer_->GetLayerType();
}

CNFeatureDefn* CNThreadSafeLayer::GetFeatureDefn() {
    CNReadWriteLock::ReadGuard guard(lock_);
    return layer_->GetFeatureDefn();
}

const CNFeatureDefn* CNThreadSafeLayer::GetFeatureDefn() const {
    CNReadWriteLock::ReadGuard guard(lock_);
    return layer_->GetFeatureDefn();
}

GeomType CNThreadSafeLayer::GetGeomType() const {
    CNReadWriteLock::ReadGuard guard(lock_);
    return layer_->GetGeomType();
}

void* CNThreadSafeLayer::GetSpatialRef() {
    CNReadWriteLock::ReadGuard guard(lock_);
    return layer_->GetSpatialRef();
}

const void* CNThreadSafeLayer::GetSpatialRef() const {
    CNReadWriteLock::ReadGuard guard(lock_);
    return layer_->GetSpatialRef();
}

CNStatus CNThreadSafeLayer::GetExtent(Envelope& extent, bool force) const {
    CNReadWriteLock::ReadGuard guard(lock_);
    return layer_->GetExtent(extent, force);
}

int64_t CNThreadSafeLayer::GetFeatureCount(bool force) const {
    CNReadWriteLock::ReadGuard guard(lock_);
    return layer_->GetFeatureCount(force);
}

void CNThreadSafeLayer::ResetReading() {
    CNReadWriteLock::WriteGuard guard(lock_);
    layer_->ResetReading();
}

std::unique_ptr<CNFeature> CNThreadSafeLayer::GetNextFeature() {
    CNReadWriteLock::ReadGuard guard(lock_);
    return layer_->GetNextFeature();
}

CNFeature* CNThreadSafeLayer::GetNextFeatureRef() {
    CNReadWriteLock::ReadGuard guard(lock_);
    return layer_->GetNextFeatureRef();
}

std::unique_ptr<CNFeature> CNThreadSafeLayer::GetFeature(int64_t fid) {
    CNReadWriteLock::ReadGuard guard(lock_);
    return layer_->GetFeature(fid);
}

CNStatus CNThreadSafeLayer::SetFeature(const CNFeature* feature) {
    CNReadWriteLock::WriteGuard guard(lock_);
    return layer_->SetFeature(feature);
}

CNStatus CNThreadSafeLayer::CreateFeature(CNFeature* feature) {
    CNReadWriteLock::WriteGuard guard(lock_);
    return layer_->CreateFeature(feature);
}

CNStatus CNThreadSafeLayer::DeleteFeature(int64_t fid) {
    CNReadWriteLock::WriteGuard guard(lock_);
    return layer_->DeleteFeature(fid);
}

int64_t CNThreadSafeLayer::CreateFeatureBatch(const std::vector<CNFeature*>& features) {
    CNReadWriteLock::WriteGuard guard(lock_);
    return layer_->CreateFeatureBatch(features);
}

CNStatus CNThreadSafeLayer::CreateField(const CNFieldDefn* field_defn, bool approx_ok) {
    CNReadWriteLock::WriteGuard guard(lock_);
    return layer_->CreateField(field_defn, approx_ok);
}

CNStatus CNThreadSafeLayer::DeleteField(int field_index) {
    CNReadWriteLock::WriteGuard guard(lock_);
    return layer_->DeleteField(field_index);
}

CNStatus CNThreadSafeLayer::ReorderFields(const std::vector<int>& new_order) {
    CNReadWriteLock::WriteGuard guard(lock_);
    return layer_->ReorderFields(new_order);
}

CNStatus CNThreadSafeLayer::AlterFieldDefn(int field_index, const CNFieldDefn* new_defn, int flags) {
    CNReadWriteLock::WriteGuard guard(lock_);
    return layer_->AlterFieldDefn(field_index, new_defn, flags);
}

void CNThreadSafeLayer::SetSpatialFilterRect(double min_x, double min_y, double max_x, double max_y) {
    CNReadWriteLock::WriteGuard guard(lock_);
    layer_->SetSpatialFilterRect(min_x, min_y, max_x, max_y);
}

void CNThreadSafeLayer::SetSpatialFilter(const CNGeometry* geometry) {
    CNReadWriteLock::WriteGuard guard(lock_);
    layer_->SetSpatialFilter(geometry);
}

const CNGeometry* CNThreadSafeLayer::GetSpatialFilter() const {
    CNReadWriteLock::ReadGuard guard(lock_);
    return layer_->GetSpatialFilter();
}

CNStatus CNThreadSafeLayer::SetAttributeFilter(const std::string& query) {
    CNReadWriteLock::WriteGuard guard(lock_);
    return layer_->SetAttributeFilter(query);
}

CNStatus CNThreadSafeLayer::StartTransaction() {
    CNReadWriteLock::WriteGuard guard(lock_);
    return layer_->StartTransaction();
}

CNStatus CNThreadSafeLayer::CommitTransaction() {
    CNReadWriteLock::WriteGuard guard(lock_);
    return layer_->CommitTransaction();
}

CNStatus CNThreadSafeLayer::RollbackTransaction() {
    CNReadWriteLock::WriteGuard guard(lock_);
    return layer_->RollbackTransaction();
}

bool CNThreadSafeLayer::TestCapability(CNLayerCapability capability) const {
    CNReadWriteLock::ReadGuard guard(lock_);
    return layer_->TestCapability(capability);
}

std::unique_ptr<CNLayer> CNThreadSafeLayer::Clone() const {
    CNReadWriteLock::ReadGuard guard(lock_);
    return layer_->Clone();
}

} // namespace ogc
