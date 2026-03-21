#include "ogc/feature/feature_collection.h"
#include "ogc/geometry.h"
#include "ogc/envelope.h"
#include <algorithm>
#include <sstream>

namespace ogc {

struct CNFeatureCollection::Impl {
    CNFeatureDefn* definition_;
    std::vector<CNFeature*> features_;
    std::string name_;
    std::string description_;

    Impl() : definition_(nullptr) {
        if (definition_) {
            definition_->AddReference();
        }
    }

    ~Impl() {
        for (auto feature : features_) {
            delete feature;
        }
        if (definition_) {
            definition_->ReleaseReference();
        }
    }

    Impl(const Impl& other)
        : definition_(other.definition_), name_(other.name_), description_(other.description_) {
        if (definition_) {
            definition_->AddReference();
        }
        features_.reserve(other.features_.size());
        for (const auto& feature : other.features_) {
            if (feature) {
                features_.push_back(feature->Clone());
            } else {
                features_.push_back(nullptr);
            }
        }
    }
};

CNFeatureCollection::CNFeatureCollection()
    : impl_(new Impl()) {
}

CNFeatureCollection::~CNFeatureCollection() {
}

CNFeatureCollection::CNFeatureCollection(const CNFeatureCollection& other)
    : impl_(new Impl(*other.impl_)) {
}

CNFeatureCollection& CNFeatureCollection::operator=(const CNFeatureCollection& other) {
    if (this != &other) {
        impl_.reset(new Impl(*other.impl_));
    }
    return *this;
}

CNFeatureCollection::CNFeatureCollection(CNFeatureCollection&& other) noexcept
    : impl_(std::move(other.impl_)) {
}

CNFeatureCollection& CNFeatureCollection::operator=(CNFeatureCollection&& other) noexcept {
    if (this != &other) {
        impl_ = std::move(other.impl_);
    }
    return *this;
}

void CNFeatureCollection::SetFeatureDefinition(CNFeatureDefn* defn) {
    if (impl_->definition_) {
        impl_->definition_->ReleaseReference();
    }
    impl_->definition_ = defn;
    if (impl_->definition_) {
        impl_->definition_->AddReference();
    }
}

CNFeatureDefn* CNFeatureCollection::GetFeatureDefinition() const {
    return impl_->definition_;
}

size_t CNFeatureCollection::GetFeatureCount() const {
    return impl_->features_.size();
}

CNFeature* CNFeatureCollection::GetFeature(size_t index) const {
    if (index >= impl_->features_.size()) {
        return nullptr;
    }
    return impl_->features_[index];
}

void CNFeatureCollection::AddFeature(CNFeature* feature) {
    impl_->features_.push_back(feature);
}

void CNFeatureCollection::InsertFeature(size_t index, CNFeature* feature) {
    if (index > impl_->features_.size()) {
        index = impl_->features_.size();
    }
    impl_->features_.insert(impl_->features_.begin() + index, feature);
}

std::unique_ptr<CNFeature> CNFeatureCollection::RemoveFeature(size_t index) {
    if (index >= impl_->features_.size()) {
        return std::unique_ptr<CNFeature>();
    }
    CNFeature* feature = impl_->features_[index];
    impl_->features_.erase(impl_->features_.begin() + index);
    return std::unique_ptr<CNFeature>(feature);
}

void CNFeatureCollection::Clear() {
    for (auto feature : impl_->features_) {
        delete feature;
    }
    impl_->features_.clear();
}

void CNFeatureCollection::SetName(const std::string& name) {
    impl_->name_ = name;
}

std::string CNFeatureCollection::GetName() const {
    return impl_->name_;
}

void CNFeatureCollection::SetDescription(const std::string& desc) {
    impl_->description_ = desc;
}

std::string CNFeatureCollection::GetDescription() const {
    return impl_->description_;
}

std::unique_ptr<Envelope> CNFeatureCollection::GetEnvelope() const {
    Envelope result;
    for (const auto& feature : impl_->features_) {
        if (feature) {
            GeometryPtr geom = feature->GetGeometry();
            if (geom) {
                const Envelope& env = geom->GetEnvelope();
                result.ExpandToInclude(env);
            }
        }
    }
    return std::unique_ptr<Envelope>(new Envelope(result));
}

CNFeatureCollection* CNFeatureCollection::Clone() const {
    CNFeatureCollection* collection = new CNFeatureCollection();
    collection->impl_->name_ = impl_->name_;
    collection->impl_->description_ = impl_->description_;
    if (impl_->definition_) {
        collection->SetFeatureDefinition(impl_->definition_);
    }
    for (const auto& feature : impl_->features_) {
        if (feature) {
            collection->AddFeature(feature->Clone());
        } else {
            collection->AddFeature(nullptr);
        }
    }
    return collection;
}

CNFeatureCollection* CNFeatureCollection::FilterByFID(int64_t fid) const {
    CNFeatureCollection* result = new CNFeatureCollection();
    if (impl_->definition_) {
        result->SetFeatureDefinition(impl_->definition_);
    }
    for (const auto& feature : impl_->features_) {
        if (feature && feature->GetFID() == fid) {
            result->AddFeature(feature->Clone());
        }
    }
    return result;
}

CNFeatureCollection* CNFeatureCollection::FilterByField(const std::string& field_name, const CNFieldValue& value) const {
    CNFeatureCollection* result = new CNFeatureCollection();
    if (impl_->definition_) {
        result->SetFeatureDefinition(impl_->definition_);
    }
    CNFeatureDefn* defn = impl_->definition_;
    if (!defn) {
        return result;
    }
    size_t field_index = defn->GetFieldIndex(field_name.c_str());
    if (field_index == static_cast<size_t>(-1)) {
        return result;
    }
    std::string value_str = value.GetString();
    for (const auto& feature : impl_->features_) {
        if (feature) {
            std::string field_value = feature->GetFieldAsString(field_index);
            if (field_value == value_str) {
                result->AddFeature(feature->Clone());
            }
        }
    }
    return result;
}

std::string CNFeatureCollection::ToJSON() const {
    std::ostringstream oss;
    oss << "{\"type\":\"FeatureCollection\",\"features\":[";
    for (size_t i = 0; i < impl_->features_.size(); ++i) {
        if (i > 0) oss << ",";
        const CNFeature* feature = impl_->features_[i];
        if (feature) {
            oss << "{\"type\":\"Feature\",\"geometry\":";
            GeometryPtr geom = feature->GetGeometry();
            if (geom) {
                std::string wkt = geom->AsText();
                oss << "\"" << wkt << "\"";
            } else {
                oss << "null";
            }
            oss << ",\"properties\":{";
            CNFeatureDefn* defn = impl_->definition_;
            if (defn) {
                size_t field_count = defn->GetFieldCount();
                for (size_t j = 0; j < field_count; ++j) {
                    if (j > 0) oss << ",";
                    CNFieldDefn* field_defn = defn->GetFieldDefn(j);
                    if (field_defn) {
                        oss << "\"" << field_defn->GetName() << "\":";
                        std::string field_value = feature->GetFieldAsString(j);
                        oss << "\"" << field_value << "\"";
                    }
                }
            }
            oss << "}}";
        }
    }
    oss << "]}";
    return oss.str();
}

void CNFeatureCollection::FromJSON(const std::string& json) {
    Clear();
}

std::string CNFeatureCollection::ToXML() const {
    std::ostringstream oss;
    oss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    oss << "<FeatureCollection";
    if (!impl_->name_.empty()) {
        oss << " name=\"" << impl_->name_ << "\"";
    }
    if (!impl_->description_.empty()) {
        oss << " description=\"" << impl_->description_ << "\"";
    }
    oss << ">\n";
    for (const auto& feature : impl_->features_) {
        if (feature) {
            oss << "  <Feature fid=\"" << feature->GetFID() << "\">\n";
            oss << "    <Geometry>";
            GeometryPtr geom = feature->GetGeometry();
            if (geom) {
                oss << geom->AsText();
            }
            oss << "</Geometry>\n";
            oss << "  </Feature>\n";
        }
    }
    oss << "</FeatureCollection>\n";
    return oss.str();
}

void CNFeatureCollection::FromXML(const std::string& xml) {
    Clear();
}

void CNFeatureCollection::Swap(CNFeatureCollection& other) {
    impl_.swap(other.impl_);
}

}
