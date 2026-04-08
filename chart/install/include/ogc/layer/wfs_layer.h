#pragma once

#include "ogc/layer/export.h"
#include "ogc/layer/layer.h"
#include "ogc/layer/layer_type.h"

#include <memory>
#include <string>
#include <vector>

namespace ogc {

struct CNWFSConnectionParams {
    std::string url;
    std::string version = "2.0.2";
    std::string user;
    std::string password;
    int timeout_ms = 30000;
    int max_features = 1000;
    bool verify_ssl = true;
};

class OGC_LAYER_API CNWFSLayer : public CNLayer {
public:
    static std::unique_ptr<CNWFSLayer> Connect(
        const CNWFSConnectionParams& params,
        const std::string& type_name);

    static std::string GetCapabilities(const std::string& url);

    static std::vector<std::string> ListLayers(const std::string& url);

    ~CNWFSLayer() override;

    const std::string& GetName() const override;
    CNLayerType GetLayerType() const override;

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

    void SetSpatialFilterRect(double min_x, double min_y,
                               double max_x, double max_y) override;
    void SetSpatialFilter(const CNGeometry* geometry) override;
    const CNGeometry* GetSpatialFilter() const override;

    CNStatus SetAttributeFilter(const std::string& query) override;

    bool TestCapability(CNLayerCapability capability) const override;

    std::string GetServiceURL() const;
    std::string GetTypeName() const;

    void SetBBOXCRS(const std::string& crs);
    void SetSortBy(const std::string& property, bool ascending = true);
    void SetOutputFormat(const std::string& format);

    CNStatus PreloadAll();

private:
    CNWFSLayer();
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace ogc
