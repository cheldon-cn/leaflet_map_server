#include "ogc/feature/geom_field_defn.h"
#include <cstring>

namespace ogc {

CNGeomFieldDefn::CNGeomFieldDefn() {}

CNGeomFieldDefn::~CNGeomFieldDefn() {}

class CNGeomFieldDefnImpl : public CNGeomFieldDefn {
public:
    CNGeomFieldDefnImpl()
        : name_(), geom_type_(GeomType::kUnknown), spatial_ref_(nullptr),
          nullable_(true), is_2d_(true), measured_(false),
          xmin_(0), xmax_(0), ymin_(0), ymax_(0) {
    }
    
    explicit CNGeomFieldDefnImpl(const char* name)
        : name_(name ? name : ""), geom_type_(GeomType::kUnknown), spatial_ref_(nullptr),
          nullable_(true), is_2d_(true), measured_(false),
          xmin_(0), xmax_(0), ymin_(0), ymax_(0) {
    }
    
    const char* GetName() const override {
        return name_.c_str();
    }
    
    void SetName(const char* name) override {
        name_ = name ? name : "";
    }
    
    GeomType GetGeomType() const override {
        return geom_type_;
    }
    
    void SetGeomType(GeomType type) override {
        geom_type_ = type;
    }
    
    void* GetSpatialRef() const override {
        return spatial_ref_;
    }
    
    void SetSpatialRef(void* sr) override {
        spatial_ref_ = sr;
    }
    
    bool IsNullable() const override {
        return nullable_;
    }
    
    void SetNullable(bool nullable) override {
        nullable_ = nullable;
    }
    
    bool Is2D() const override {
        return is_2d_;
    }
    
    void Set2D(bool is_2d) override {
        is_2d_ = is_2d;
    }
    
    bool IsMeasured() const override {
        return measured_;
    }
    
    void SetMeasured(bool measured) override {
        measured_ = measured;
    }
    
    double GetXMin() const override {
        return xmin_;
    }
    
    double GetXMax() const override {
        return xmax_;
    }
    
    double GetYMin() const override {
        return ymin_;
    }
    
    double GetYMax() const override {
        return ymax_;
    }
    
    void SetExtent(double xmin, double xmax, double ymin, double ymax) override {
        xmin_ = xmin;
        xmax_ = xmax;
        ymin_ = ymin;
        ymax_ = ymax;
    }
    
    CNGeomFieldDefn* Clone() const override {
        CNGeomFieldDefnImpl* clone = new CNGeomFieldDefnImpl(name_.c_str());
        clone->SetGeomType(geom_type_);
        clone->SetSpatialRef(spatial_ref_);
        clone->SetNullable(nullable_);
        clone->Set2D(is_2d_);
        clone->SetMeasured(measured_);
        clone->SetExtent(xmin_, xmax_, ymin_, ymax_);
        return clone;
    }
    
private:
    std::string name_;
    GeomType geom_type_;
    void* spatial_ref_;
    bool nullable_;
    bool is_2d_;
    bool measured_;
    double xmin_;
    double xmax_;
    double ymin_;
    double ymax_;
};

CNGeomFieldDefn* CreateCNGeomFieldDefn() {
    return new CNGeomFieldDefnImpl();
}

CNGeomFieldDefn* CreateCNGeomFieldDefn(const char* name) {
    return new CNGeomFieldDefnImpl(name);
}

} // namespace ogc
