#include "ogc/feature/field_defn.h"
#include <cstring>
#include <cstdlib>

namespace ogc {

CNFieldDefn::CNFieldDefn() {}

CNFieldDefn::~CNFieldDefn() {}

class CNFieldDefnImpl : public CNFieldDefn {
public:
    CNFieldDefnImpl() 
        : name_(), type_(CNFieldType::kString), sub_type_(CNFieldSubType::kNone),
          width_(0), precision_(0), nullable_(true), unique_(false),
          default_value_(), alternative_name_(), comment_(), domain_() {
    }
    
    explicit CNFieldDefnImpl(const char* name) 
        : name_(name ? name : ""), type_(CNFieldType::kString), sub_type_(CNFieldSubType::kNone),
          width_(0), precision_(0), nullable_(true), unique_(false),
          default_value_(), alternative_name_(), comment_(), domain_() {
    }
    
    const char* GetName() const override {
        return name_.c_str();
    }
    
    void SetName(const char* name) override {
        name_ = name ? name : "";
    }
    
    CNFieldType GetType() const override {
        return type_;
    }
    
    void SetType(CNFieldType type) override {
        type_ = type;
    }
    
    CNFieldSubType GetSubType() const override {
        return sub_type_;
    }
    
    void SetSubType(CNFieldSubType sub_type) override {
        sub_type_ = sub_type;
    }
    
    int GetWidth() const override {
        return width_;
    }
    
    void SetWidth(int width) override {
        width_ = width;
    }
    
    int GetPrecision() const override {
        return precision_;
    }
    
    void SetPrecision(int precision) override {
        precision_ = precision;
    }
    
    bool IsNullable() const override {
        return nullable_;
    }
    
    void SetNullable(bool nullable) override {
        nullable_ = nullable;
    }
    
    bool IsUnique() const override {
        return unique_;
    }
    
    void SetUnique(bool unique) override {
        unique_ = unique;
    }
    
    const char* GetDefaultValue() const override {
        return default_value_.c_str();
    }
    
    void SetDefaultValue(const char* value) override {
        default_value_ = value ? value : "";
    }
    
    const char* GetAlternativeName() const override {
        return alternative_name_.c_str();
    }
    
    void SetAlternativeName(const char* name) override {
        alternative_name_ = name ? name : "";
    }
    
    const char* GetComment() const override {
        return comment_.c_str();
    }
    
    void SetComment(const char* comment) override {
        comment_ = comment ? comment : "";
    }
    
    const char* GetDomainName() const override {
        return domain_.c_str();
    }
    
    void SetDomainName(const char* domain) override {
        domain_ = domain ? domain : "";
    }
    
    CNFieldDefn* Clone() const override {
        CNFieldDefnImpl* clone = new CNFieldDefnImpl(name_.c_str());
        clone->SetType(type_);
        clone->SetSubType(sub_type_);
        clone->SetWidth(width_);
        clone->SetPrecision(precision_);
        clone->SetNullable(nullable_);
        clone->SetUnique(unique_);
        clone->SetDefaultValue(default_value_.c_str());
        clone->SetAlternativeName(alternative_name_.c_str());
        clone->SetComment(comment_.c_str());
        clone->SetDomainName(domain_.c_str());
        return clone;
    }
    
private:
    std::string name_;
    CNFieldType type_;
    CNFieldSubType sub_type_;
    int width_;
    int precision_;
    bool nullable_;
    bool unique_;
    std::string default_value_;
    std::string alternative_name_;
    std::string comment_;
    std::string domain_;
};

CNFieldDefn* CreateCNFieldDefn() {
    return new CNFieldDefnImpl();
}

CNFieldDefn* CreateCNFieldDefn(const char* name) {
    return new CNFieldDefnImpl(name);
}

} // namespace ogc
