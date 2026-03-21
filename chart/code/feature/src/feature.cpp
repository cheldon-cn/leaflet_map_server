#include "ogc/feature/feature.h"
#include <cstring>
#include <algorithm>

namespace ogc {

struct CNFeature::Impl {
    CNFeatureDefn* definition_;
    int64_t fid_;
    std::vector<CNFieldValue> fields_;
    std::vector<GeometryPtr> geometries_;
    
    Impl() : definition_(nullptr), fid_(0) {}
    
    explicit Impl(CNFeatureDefn* def) : definition_(def), fid_(0) {
        if (definition_) {
            definition_->AddReference();
            size_t field_count = definition_->GetFieldCount();
            fields_.resize(field_count);
            size_t geom_count = definition_->GetGeomFieldCount();
            geometries_.resize(geom_count);
        }
    }
};

CNFeature::CNFeature()
    : impl_(new Impl()) {
}

CNFeature::CNFeature(CNFeatureDefn* definition)
    : impl_(new Impl(definition)) {
}

CNFeature::~CNFeature() {
    if (impl_->definition_) {
        impl_->definition_->ReleaseReference();
    }
}

CNFeature::CNFeature(const CNFeature& other)
    : impl_(new Impl(other.impl_->definition_)) {
    impl_->fid_ = other.impl_->fid_;
    impl_->fields_ = other.impl_->fields_;
    impl_->geometries_.reserve(other.impl_->geometries_.size());
    for (const auto& geom : other.impl_->geometries_) {
        if (geom) {
            impl_->geometries_.push_back(geom->Clone());
        } else {
            impl_->geometries_.push_back(GeometryPtr());
        }
    }
    if (impl_->definition_) {
        impl_->definition_->AddReference();
    }
}

CNFeature::CNFeature(CNFeature&& other) noexcept
    : impl_(std::move(other.impl_)) {
    other.impl_.reset(new Impl());
}

CNFeature& CNFeature::operator=(const CNFeature& other) {
    if (this != &other) {
        if (impl_->definition_) {
            impl_->definition_->ReleaseReference();
        }
        impl_.reset(new Impl(other.impl_->definition_));
        impl_->fid_ = other.impl_->fid_;
        impl_->fields_ = other.impl_->fields_;
        impl_->geometries_.clear();
        impl_->geometries_.reserve(other.impl_->geometries_.size());
        for (const auto& geom : other.impl_->geometries_) {
            if (geom) {
                impl_->geometries_.push_back(geom->Clone());
            } else {
                impl_->geometries_.push_back(GeometryPtr());
            }
        }
        if (impl_->definition_) {
            impl_->definition_->AddReference();
        }
    }
    return *this;
}

CNFeature& CNFeature::operator=(CNFeature&& other) noexcept {
    if (this != &other) {
        if (impl_->definition_) {
            impl_->definition_->ReleaseReference();
        }
        impl_ = std::move(other.impl_);
        other.impl_.reset(new Impl());
    }
    return *this;
}

CNFeatureDefn* CNFeature::GetFeatureDefn() const {
    return impl_->definition_;
}

void CNFeature::SetFeatureDefn(CNFeatureDefn* definition) {
    if (impl_->definition_) {
        impl_->definition_->ReleaseReference();
    }
    impl_->definition_ = definition;
    if (definition) {
        definition->AddReference();
        EnsureFieldsInitialized();
        EnsureGeomFieldsInitialized();
    }
}

int64_t CNFeature::GetFID() const {
    return impl_->fid_;
}

void CNFeature::SetFID(int64_t fid) {
    impl_->fid_ = fid;
}

size_t CNFeature::GetFieldCount() const {
    return impl_->fields_.size();
}

CNFieldValue& CNFeature::GetField(size_t index) {
    if (index >= impl_->fields_.size()) {
        static CNFieldValue empty;
        return empty;
    }
    return impl_->fields_[index];
}

const CNFieldValue& CNFeature::GetField(size_t index) const {
    if (index >= impl_->fields_.size()) {
        static const CNFieldValue empty;
        return empty;
    }
    return impl_->fields_[index];
}

bool CNFeature::IsFieldSet(size_t index) const {
    if (index >= impl_->fields_.size()) return false;
    return impl_->fields_[index].IsSet();
}

bool CNFeature::IsFieldNull(size_t index) const {
    if (index >= impl_->fields_.size()) return false;
    return impl_->fields_[index].IsNull();
}

void CNFeature::SetField(size_t index, const CNFieldValue& value) {
    if (index >= impl_->fields_.size()) return;
    impl_->fields_[index] = value;
}

void CNFeature::SetField(size_t index, CNFieldValue&& value) {
    if (index >= impl_->fields_.size()) return;
    impl_->fields_[index] = std::move(value);
}

void CNFeature::SetFieldNull(size_t index) {
    if (index >= impl_->fields_.size()) return;
    impl_->fields_[index].SetNull();
}

void CNFeature::UnsetField(size_t index) {
    if (index >= impl_->fields_.size()) return;
    impl_->fields_[index].Unset();
}

int32_t CNFeature::GetFieldAsInteger(size_t index) const {
    if (index >= impl_->fields_.size()) return 0;
    return impl_->fields_[index].GetInteger();
}

int64_t CNFeature::GetFieldAsInteger64(size_t index) const {
    if (index >= impl_->fields_.size()) return 0;
    return impl_->fields_[index].GetInteger64();
}

double CNFeature::GetFieldAsReal(size_t index) const {
    if (index >= impl_->fields_.size()) return 0.0;
    return impl_->fields_[index].GetReal();
}

std::string CNFeature::GetFieldAsString(size_t index) const {
    if (index >= impl_->fields_.size()) return std::string();
    return impl_->fields_[index].GetString();
}

CNDateTime CNFeature::GetFieldAsDateTime(size_t index) const {
    if (index >= impl_->fields_.size()) return CNDateTime();
    return impl_->fields_[index].GetDateTime();
}

std::vector<uint8_t> CNFeature::GetFieldAsBinary(size_t index) const {
    if (index >= impl_->fields_.size()) return std::vector<uint8_t>();
    return impl_->fields_[index].GetBinary();
}

int32_t CNFeature::GetFieldAsInteger(const char* name) const {
    if (!impl_->definition_ || !name) return 0;
    int idx = impl_->definition_->GetFieldIndex(name);
    if (idx < 0) return 0;
    return GetFieldAsInteger(static_cast<size_t>(idx));
}

int64_t CNFeature::GetFieldAsInteger64(const char* name) const {
    if (!impl_->definition_ || !name) return 0;
    int idx = impl_->definition_->GetFieldIndex(name);
    if (idx < 0) return 0;
    return GetFieldAsInteger64(static_cast<size_t>(idx));
}

double CNFeature::GetFieldAsReal(const char* name) const {
    if (!impl_->definition_ || !name) return 0.0;
    int idx = impl_->definition_->GetFieldIndex(name);
    if (idx < 0) return 0.0;
    return GetFieldAsReal(static_cast<size_t>(idx));
}

std::string CNFeature::GetFieldAsString(const char* name) const {
    if (!impl_->definition_ || !name) return std::string();
    int idx = impl_->definition_->GetFieldIndex(name);
    if (idx < 0) return std::string();
    return GetFieldAsString(static_cast<size_t>(idx));
}

CNDateTime CNFeature::GetFieldAsDateTime(const char* name) const {
    if (!impl_->definition_ || !name) return CNDateTime();
    int idx = impl_->definition_->GetFieldIndex(name);
    if (idx < 0) return CNDateTime();
    return GetFieldAsDateTime(static_cast<size_t>(idx));
}

std::vector<uint8_t> CNFeature::GetFieldAsBinary(const char* name) const {
    if (!impl_->definition_ || !name) return std::vector<uint8_t>();
    int idx = impl_->definition_->GetFieldIndex(name);
    if (idx < 0) return std::vector<uint8_t>();
    return GetFieldAsBinary(static_cast<size_t>(idx));
}

void CNFeature::SetFieldInteger(size_t index, int32_t value) {
    if (index >= impl_->fields_.size()) return;
    impl_->fields_[index].SetInteger(value);
}

void CNFeature::SetFieldInteger64(size_t index, int64_t value) {
    if (index >= impl_->fields_.size()) return;
    impl_->fields_[index].SetInteger64(value);
}

void CNFeature::SetFieldReal(size_t index, double value) {
    if (index >= impl_->fields_.size()) return;
    impl_->fields_[index].SetReal(value);
}

void CNFeature::SetFieldString(size_t index, const char* value) {
    if (index >= impl_->fields_.size()) return;
    impl_->fields_[index].SetString(value);
}

void CNFeature::SetFieldString(size_t index, const std::string& value) {
    if (index >= impl_->fields_.size()) return;
    impl_->fields_[index].SetString(value);
}

void CNFeature::SetFieldDateTime(size_t index, const CNDateTime& value) {
    if (index >= impl_->fields_.size()) return;
    impl_->fields_[index].SetDateTime(value);
}

void CNFeature::SetFieldBinary(size_t index, const std::vector<uint8_t>& value) {
    if (index >= impl_->fields_.size()) return;
    impl_->fields_[index].SetBinary(value);
}

void CNFeature::SetFieldInteger(const char* name, int32_t value) {
    if (!impl_->definition_ || !name) return;
    int idx = impl_->definition_->GetFieldIndex(name);
    if (idx >= 0) SetFieldInteger(static_cast<size_t>(idx), value);
}

void CNFeature::SetFieldInteger64(const char* name, int64_t value) {
    if (!impl_->definition_ || !name) return;
    int idx = impl_->definition_->GetFieldIndex(name);
    if (idx >= 0) SetFieldInteger64(static_cast<size_t>(idx), value);
}

void CNFeature::SetFieldReal(const char* name, double value) {
    if (!impl_->definition_ || !name) return;
    int idx = impl_->definition_->GetFieldIndex(name);
    if (idx >= 0) SetFieldReal(static_cast<size_t>(idx), value);
}

void CNFeature::SetFieldString(const char* name, const char* value) {
    if (!impl_->definition_ || !name) return;
    int idx = impl_->definition_->GetFieldIndex(name);
    if (idx >= 0) SetFieldString(static_cast<size_t>(idx), value);
}

void CNFeature::SetFieldString(const char* name, const std::string& value) {
    if (!impl_->definition_ || !name) return;
    int idx = impl_->definition_->GetFieldIndex(name);
    if (idx >= 0) SetFieldString(static_cast<size_t>(idx), value);
}

void CNFeature::SetFieldDateTime(const char* name, const CNDateTime& value) {
    if (!impl_->definition_ || !name) return;
    int idx = impl_->definition_->GetFieldIndex(name);
    if (idx >= 0) SetFieldDateTime(static_cast<size_t>(idx), value);
}

void CNFeature::SetFieldBinary(const char* name, const std::vector<uint8_t>& value) {
    if (!impl_->definition_ || !name) return;
    int idx = impl_->definition_->GetFieldIndex(name);
    if (idx >= 0) SetFieldBinary(static_cast<size_t>(idx), value);
}

void CNFeature::SetFieldNull(const char* name) {
    if (!impl_->definition_ || !name) return;
    int idx = impl_->definition_->GetFieldIndex(name);
    if (idx >= 0) SetFieldNull(static_cast<size_t>(idx));
}

size_t CNFeature::GetGeomFieldCount() const {
    return impl_->geometries_.size();
}

GeometryPtr CNFeature::GetGeometry(size_t index) const {
    if (index >= impl_->geometries_.size()) return GeometryPtr();
    const GeometryPtr& geom = impl_->geometries_[index];
    if (!geom) return GeometryPtr();
    return geom->Clone();
}

void CNFeature::SetGeometry(GeometryPtr geometry, size_t index) {
    if (index >= impl_->geometries_.size()) {
        impl_->geometries_.resize(index + 1);
    }
    impl_->geometries_[index] = std::move(geometry);
}

GeometryPtr CNFeature::StealGeometry(size_t index) {
    if (index >= impl_->geometries_.size()) return GeometryPtr();
    GeometryPtr geom = std::move(impl_->geometries_[index]);
    impl_->geometries_[index].reset();
    return geom;
}

std::unique_ptr<Envelope> CNFeature::GetEnvelope() const {
    Envelope result;
    for (const auto& geom : impl_->geometries_) {
        if (geom) {
            const Envelope& geom_env = geom->GetEnvelope();
            result.ExpandToInclude(geom_env);
        }
    }
    return std::unique_ptr<Envelope>(new Envelope(result));
}

CNFeature* CNFeature::Clone() const {
    CNFeature* clone = new CNFeature();
    if (impl_->definition_) {
        clone->SetFeatureDefn(impl_->definition_);
    }
    clone->SetFID(impl_->fid_);
    for (size_t i = 0; i < impl_->fields_.size(); ++i) {
        clone->SetField(i, impl_->fields_[i]);
    }
    for (size_t i = 0; i < impl_->geometries_.size(); ++i) {
        if (impl_->geometries_[i]) {
            clone->SetGeometry(impl_->geometries_[i]->Clone(), i);
        }
    }
    return clone;
}

void CNFeature::Swap(CNFeature& other) noexcept {
    impl_.swap(other.impl_);
}

bool CNFeature::Equal(const CNFeature& other) const {
    if (impl_->fid_ != other.impl_->fid_) return false;
    
    if (impl_->fields_.size() != other.impl_->fields_.size()) return false;
    for (size_t i = 0; i < impl_->fields_.size(); ++i) {
        const CNFieldValue& this_field = impl_->fields_[i];
        const CNFieldValue& other_field = other.impl_->fields_[i];
        if (this_field.GetType() != other_field.GetType()) return false;
    }
    
    if (impl_->geometries_.size() != other.impl_->geometries_.size()) return false;
    for (size_t i = 0; i < impl_->geometries_.size(); ++i) {
        const GeometryPtr& this_geom = impl_->geometries_[i];
        const GeometryPtr& other_geom = other.impl_->geometries_[i];
        if (this_geom && other_geom) {
            if (!this_geom->Equals(other_geom.get())) return false;
        } else if (this_geom || other_geom) {
            return false;
        }
    }
    
    return true;
}

bool CNFeature::operator==(const CNFeature& other) const {
    return Equal(other);
}

bool CNFeature::operator!=(const CNFeature& other) const {
    return !Equal(other);
}

void CNFeature::EnsureFieldsInitialized() {
    if (!impl_->definition_) return;
    size_t field_count = impl_->definition_->GetFieldCount();
    if (impl_->fields_.size() < field_count) {
        impl_->fields_.resize(field_count);
    }
}

void CNFeature::EnsureGeomFieldsInitialized() {
    if (!impl_->definition_) return;
    size_t geom_count = impl_->definition_->GetGeomFieldCount();
    if (impl_->geometries_.size() < geom_count) {
        impl_->geometries_.resize(geom_count);
    }
}

} // namespace ogc
