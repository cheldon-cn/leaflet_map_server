#ifndef OGC_SYMBOLOGY_FILTER_H
#define OGC_SYMBOLOGY_FILTER_H

#include "ogc/symbology/export.h"
#include "ogc/geometry.h"
#include "ogc/feature/feature.h"
#include <memory>
#include <string>
#include <vector>

namespace ogc {
namespace symbology {

class Filter;
typedef std::shared_ptr<Filter> FilterPtr;

enum class FilterType {
    kNone,
    kComparison,
    kLogical,
    kSpatial,
    kId
};

class OGC_SYMBOLOGY_API Filter {
public:
    virtual ~Filter() = default;
    
    virtual FilterType GetType() const = 0;
    
    virtual bool Evaluate(const CNFeature* feature) const = 0;
    
    virtual bool Evaluate(const Geometry* geometry) const = 0;
    
    virtual std::string ToString() const = 0;
    
    virtual FilterPtr Clone() const = 0;
    
    static FilterPtr CreateAll();
    static FilterPtr CreateNone();
    
    bool IsAll() const;
    bool IsNone() const;
};

class OGC_SYMBOLOGY_API NullFilter : public Filter {
public:
    FilterType GetType() const override { return FilterType::kNone; }
    
    bool Evaluate(const CNFeature* feature) const override {
        (void)feature;
        return true;
    }
    
    bool Evaluate(const Geometry* geometry) const override {
        (void)geometry;
        return true;
    }
    
    std::string ToString() const override { return "ALL"; }
    
    FilterPtr Clone() const override {
        return std::make_shared<NullFilter>(*this);
    }
};

class OGC_SYMBOLOGY_API NoneFilter : public Filter {
public:
    FilterType GetType() const override { return FilterType::kNone; }
    
    bool Evaluate(const CNFeature* feature) const override {
        (void)feature;
        return false;
    }
    
    bool Evaluate(const Geometry* geometry) const override {
        (void)geometry;
        return false;
    }
    
    std::string ToString() const override { return "NONE"; }
    
    FilterPtr Clone() const override {
        return std::make_shared<NoneFilter>(*this);
    }
};

}
}

#endif
