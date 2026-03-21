#include "ogc/feature/feature_defn.h"
#include <cstring>
#include <algorithm>

namespace ogc {

CNFeatureDefn* CNFeatureDefn::Create(const char* name) {
    return new CNFeatureDefn(name);
}

struct CNFeatureDefn::Impl {
    std::string name_;
    std::vector<CNFieldDefnPtr> fields_;
    std::vector<CNGeomFieldDefnPtr> geom_fields_;
    std::vector<size_t> field_occurrences_;
    mutable std::atomic<int> ref_count_;
    
    Impl() : ref_count_(1) {}
    
    explicit Impl(const char* name) : name_(name ? name : ""), ref_count_(1) {}
};

CNFeatureDefn::CNFeatureDefn()
    : impl_(new Impl()) {
}

CNFeatureDefn::CNFeatureDefn(const char* name)
    : impl_(new Impl(name)) {
}

CNFeatureDefn::~CNFeatureDefn() {
}

const char* CNFeatureDefn::GetName() const {
    return impl_->name_.c_str();
}

void CNFeatureDefn::SetName(const char* name) {
    impl_->name_ = name ? name : "";
}

size_t CNFeatureDefn::GetFieldCount() const {
    size_t count = 0;
    for (size_t occ : impl_->field_occurrences_) {
        count += occ;
    }
    return count;
}

CNFieldDefn* CNFeatureDefn::GetFieldDefn(size_t index) const {
    size_t accumulated = 0;
    for (size_t i = 0; i < impl_->fields_.size(); ++i) {
        size_t occ = impl_->field_occurrences_[i];
        if (index < accumulated + occ) {
            return impl_->fields_[i];
        }
        accumulated += occ;
    }
    return nullptr;
}

int CNFeatureDefn::GetFieldIndex(const char* name) const {
    if (!name) return -1;
    return GetFieldIndex(name, std::strlen(name));
}

int CNFeatureDefn::GetFieldIndex(const char* name, size_t name_length) const {
    if (!name || name_length == 0) return -1;
    for (size_t i = 0; i < impl_->fields_.size(); ++i) {
        const char* field_name = impl_->fields_[i]->GetName();
        if (field_name && std::strncmp(field_name, name, name_length) == 0 && 
            std::strlen(field_name) == name_length) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

void CNFeatureDefn::AddFieldDefn(CNFieldDefn* field, size_t occurrence) {
    if (!field) return;
    if (occurrence == 0) return;
    impl_->fields_.push_back(field);
    impl_->field_occurrences_.push_back(occurrence);
}

void CNFeatureDefn::DeleteFieldDefn(size_t index) {
    if (index >= impl_->fields_.size()) return;
    impl_->fields_.erase(impl_->fields_.begin() + index);
    impl_->field_occurrences_.erase(impl_->field_occurrences_.begin() + index);
}

void CNFeatureDefn::ClearFieldDefns() {
    impl_->fields_.clear();
    impl_->field_occurrences_.clear();
}

size_t CNFeatureDefn::GetGeomFieldCount() const {
    return impl_->geom_fields_.size();
}

CNGeomFieldDefn* CNFeatureDefn::GetGeomFieldDefn(size_t index) const {
    if (index >= impl_->geom_fields_.size()) return nullptr;
    return impl_->geom_fields_[index];
}

int CNFeatureDefn::GetGeomFieldIndex(const char* name) const {
    if (!name) return -1;
    for (size_t i = 0; i < impl_->geom_fields_.size(); ++i) {
        const char* field_name = impl_->geom_fields_[i]->GetName();
        if (field_name && std::strcmp(field_name, name) == 0) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

void CNFeatureDefn::AddGeomFieldDefn(CNGeomFieldDefn* field) {
    if (!field) return;
    impl_->geom_fields_.push_back(field);
}

void CNFeatureDefn::DeleteGeomFieldDefn(size_t index) {
    if (index >= impl_->geom_fields_.size()) return;
    impl_->geom_fields_.erase(impl_->geom_fields_.begin() + index);
}

void CNFeatureDefn::ClearGeomFieldDefns() {
    impl_->geom_fields_.clear();
}

int CNFeatureDefn::GetReferenceCount() const {
    return impl_->ref_count_.load();
}

void CNFeatureDefn::AddReference() const {
    impl_->ref_count_.fetch_add(1);
}

void CNFeatureDefn::ReleaseReference() const {
    if (impl_->ref_count_.fetch_sub(1) == 1) {
        delete this;
    }
}

CNFeatureDefn* CNFeatureDefn::Clone() const {
    CNFeatureDefn* clone = new CNFeatureDefn(impl_->name_.c_str());
    for (size_t i = 0; i < impl_->fields_.size(); ++i) {
        clone->AddFieldDefn(impl_->fields_[i]->Clone(), impl_->field_occurrences_[i]);
    }
    for (size_t i = 0; i < impl_->geom_fields_.size(); ++i) {
        clone->AddGeomFieldDefn(impl_->geom_fields_[i]->Clone());
    }
    return clone;
}

bool CNFeatureDefn::IsValid() const {
    if (impl_->name_.empty()) return false;
    return true;
}

std::string CNFeatureDefn::ToJSON() const {
    std::string json = "{\"name\": \"";
    json += impl_->name_;
    json += "\", \"fields\": [";
    
    for (size_t i = 0; i < impl_->fields_.size(); ++i) {
        if (i > 0) json += ", ";
        json += "{\"name\": \"";
        json += impl_->fields_[i]->GetName();
        json += "\", \"type\": \"";
        json += GetFieldTypeName(impl_->fields_[i]->GetType());
        json += "\", \"nullable\": ";
        json += impl_->fields_[i]->IsNullable() ? "true" : "false";
        json += "}";
    }
    
    json += "], \"geomFields\": [";
    
    for (size_t i = 0; i < impl_->geom_fields_.size(); ++i) {
        if (i > 0) json += ", ";
        json += "{\"name\": \"";
        json += impl_->geom_fields_[i]->GetName();
        json += "\", \"type\": \"Geometry\"";
        json += ", \"nullable\": ";
        json += impl_->geom_fields_[i]->IsNullable() ? "true" : "false";
        json += "}";
    }
    
    json += "]}";
    return json;
}

bool CNFeatureDefn::FromJSON(const std::string& json) {
    if (json.empty()) return false;
    
    size_t name_pos = json.find("\"name\"");
    if (name_pos != std::string::npos) {
        size_t colon_pos = json.find(":", name_pos);
        size_t quote_start = json.find("\"", colon_pos);
        size_t quote_end = json.find("\"", quote_start + 1);
        if (quote_start != std::string::npos && quote_end != std::string::npos) {
            SetName(json.substr(quote_start + 1, quote_end - quote_start - 1).c_str());
        }
    }
    
    return !impl_->name_.empty();
}

std::string CNFeatureDefn::ToXML() const {
    std::string xml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xml += "<FeatureDefn name=\"";
    xml += impl_->name_;
    xml += "\">\n";
    
    for (size_t i = 0; i < impl_->fields_.size(); ++i) {
        xml += "  <FieldDefn name=\"";
        xml += impl_->fields_[i]->GetName();
        xml += "\" type=\"";
        xml += GetFieldTypeName(impl_->fields_[i]->GetType());
        xml += "\" nullable=\"";
        xml += impl_->fields_[i]->IsNullable() ? "true" : "false";
        xml += "\"/>\n";
    }
    
    for (size_t i = 0; i < impl_->geom_fields_.size(); ++i) {
        xml += "  <GeomFieldDefn name=\"";
        xml += impl_->geom_fields_[i]->GetName();
        xml += "\" nullable=\"";
        xml += impl_->geom_fields_[i]->IsNullable() ? "true" : "false";
        xml += "\"/>\n";
    }
    
    xml += "</FeatureDefn>";
    return xml;
}

bool CNFeatureDefn::FromXML(const std::string& xml) {
    if (xml.empty()) return false;
    
    size_t name_pos = xml.find("name=\"");
    if (name_pos != std::string::npos) {
        size_t quote_start = name_pos + 6;
        size_t quote_end = xml.find("\"", quote_start);
        if (quote_end != std::string::npos) {
            SetName(xml.substr(quote_start, quote_end - quote_start).c_str());
        }
    }
    
    return !impl_->name_.empty();
}

} // namespace ogc
