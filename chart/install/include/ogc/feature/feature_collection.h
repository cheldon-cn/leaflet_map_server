#pragma once

#include "export.h"
#include "ogc/feature/feature.h"
#include "ogc/feature/feature_defn.h"
#include <memory>
#include <string>
#include <vector>

namespace ogc {

class OGC_FEATURE_API CNFeatureCollection {
public:
    CNFeatureCollection();
    ~CNFeatureCollection();

    CNFeatureCollection(const CNFeatureCollection& other);
    CNFeatureCollection& operator=(const CNFeatureCollection& other);

    CNFeatureCollection(CNFeatureCollection&& other) noexcept;
    CNFeatureCollection& operator=(CNFeatureCollection&& other) noexcept;

    void SetFeatureDefinition(CNFeatureDefn* defn);
    CNFeatureDefn* GetFeatureDefinition() const;

    size_t GetFeatureCount() const;
    CNFeature* GetFeature(size_t index) const;
    void AddFeature(CNFeature* feature);
    void InsertFeature(size_t index, CNFeature* feature);
    std::unique_ptr<CNFeature> RemoveFeature(size_t index);
    void Clear();

    CNFeature* operator[](size_t index) const { return GetFeature(index); }

    void SetName(const std::string& name);
    std::string GetName() const;

    void SetDescription(const std::string& desc);
    std::string GetDescription() const;

    std::unique_ptr<Envelope> GetEnvelope() const;

    CNFeatureCollection* Clone() const;
    CNFeatureCollection* FilterByFID(int64_t fid) const;
    CNFeatureCollection* FilterByField(const std::string& field_name, const CNFieldValue& value) const;

    void Swap(CNFeatureCollection& other);

    std::string ToJSON() const;
    void FromJSON(const std::string& json);

    std::string ToXML() const;
    void FromXML(const std::string& xml);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

using FeatureCollection = CNFeatureCollection;

}
