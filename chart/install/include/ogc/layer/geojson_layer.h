#pragma once

#include "ogc/layer/export.h"
#include "ogc/layer/vector_layer.h"

#include <memory>
#include <string>

namespace ogc {

class OGC_LAYER_API CNGeoJSONLayer : public CNVectorLayer {
public:
    static std::unique_ptr<CNVectorLayer> Open(
        const std::string& path,
        bool update = false);

    static std::unique_ptr<CNVectorLayer> Create(
        const std::string& path);

    ~CNGeoJSONLayer() override;

    const std::string& GetName() const override;
    std::string GetDataSourcePath() const override;
    std::string GetFormatName() const override;
    bool IsReadOnly() const override;

    CNFeatureDefn* GetFeatureDefn() override;
    const CNFeatureDefn* GetFeatureDefn() const override;

    GeomType GetGeomType() const override;

    void* GetSpatialRef() override;
    const void* GetSpatialRef() const override;

    CNStatus GetExtent(Envelope& extent, bool force = true) const override;

    int64_t GetFeatureCount(bool force = true) const override;

    void ResetReading() override;
    std::unique_ptr<CNFeature> GetNextFeature() override;
    CNFeature* GetNextFeatureRef() override;

    std::unique_ptr<CNFeature> GetFeature(int64_t fid) override;
    CNStatus SetFeature(const CNFeature* feature) override;

    CNStatus CreateFeature(CNFeature* feature) override;
    CNStatus DeleteFeature(int64_t fid) override;

    CNStatus CreateField(const CNFieldDefn* field_defn, bool approx_ok = false) override;

    void SetSpatialFilter(const CNGeometry* geometry) override;
    const CNGeometry* GetSpatialFilter() const override;

    CNStatus SetAttributeFilter(const std::string& query) override;

    bool TestCapability(CNLayerCapability capability) const override;

    std::unique_ptr<CNLayer> Clone() const override;

private:
    CNGeoJSONLayer();
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace ogc
