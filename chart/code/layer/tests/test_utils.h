#pragma once

#include "ogc/layer/memory_layer.h"
#include "ogc/feature/feature.h"
#include "ogc/feature/field_value.h"

#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace ogc {
namespace testing {

class MockLayer : public CNLayer {
public:
    MockLayer();
    explicit MockLayer(const std::string& name);

    void SetFeatureDefn(CNFeatureDefn* defn);
    void AddFeature(std::unique_ptr<CNFeature> feature);
    void ClearFeatures();

    const std::string& GetName() const override { return name_; }
    void SetName(const std::string& name) { name_ = name; }

    CNLayerType GetLayerType() const override { return layer_type_; }
    void SetLayerType(CNLayerType type) { layer_type_ = type; }

    CNFeatureDefn* GetFeatureDefn() override { return feature_defn_.get(); }
    const CNFeatureDefn* GetFeatureDefn() const override { return feature_defn_.get(); }

    GeomType GetGeomType() const override { return geom_type_; }
    void SetGeomType(GeomType type) { geom_type_ = type; }

    void* GetSpatialRef() override { return spatial_ref_; }
    const void* GetSpatialRef() const override { return spatial_ref_; }
    void SetSpatialRef(void* ref) { spatial_ref_ = ref; }

    CNStatus GetExtent(Envelope& extent, bool force = true) const override;
    int64_t GetFeatureCount(bool force = true) const override;

    void ResetReading() override;
    std::unique_ptr<CNFeature> GetNextFeature() override;
    CNFeature* GetNextFeatureRef() override;
    std::unique_ptr<CNFeature> GetFeature(int64_t fid) override;

    CNStatus SetFeature(const CNFeature* feature) override;
    CNStatus CreateFeature(CNFeature* feature) override;
    CNStatus DeleteFeature(int64_t fid) override;
    int64_t CreateFeatureBatch(const std::vector<CNFeature*>& features) override;

    CNStatus CreateField(const CNFieldDefn* field_defn, bool approx_ok = false) override;
    CNStatus DeleteField(int field_index) override;
    CNStatus ReorderFields(const std::vector<int>& new_order) override;
    CNStatus AlterFieldDefn(int field_index, const CNFieldDefn* new_defn, int flags) override;

    void SetSpatialFilterRect(double min_x, double min_y, double max_x, double max_y) override;
    void SetSpatialFilter(const CNGeometry* geometry) override;
    const CNGeometry* GetSpatialFilter() const override;
    CNStatus SetAttributeFilter(const std::string& query) override;

    CNStatus StartTransaction() override;
    CNStatus CommitTransaction() override;
    CNStatus RollbackTransaction() override;

    bool TestCapability(CNLayerCapability capability) const override;
    std::unique_ptr<CNLayer> Clone() const override;

    void SetCapability(CNLayerCapability cap, bool value);
    void SetExtent(const Envelope& extent);

private:
    std::string name_;
    CNLayerType layer_type_ = CNLayerType::kMemory;
    std::shared_ptr<CNFeatureDefn> feature_defn_;
    GeomType geom_type_ = GeomType::kUnknown;
    void* spatial_ref_ = nullptr;

    std::vector<std::unique_ptr<CNFeature>> features_;
    std::unordered_map<int64_t, size_t> fid_index_;
    int64_t next_fid_ = 1;
    size_t current_index_ = 0;

    std::unique_ptr<CNGeometry> spatial_filter_;
    std::string attribute_filter_;

    std::unordered_map<CNLayerCapability, bool> capabilities_;
    Envelope cached_extent_;
    bool extent_valid_ = false;

    bool in_transaction_ = false;
};

class TestDataGenerator {
public:
    static std::unique_ptr<CNFeature> CreatePointFeature(
        CNFeatureDefn* defn,
        int64_t fid,
        double x, double y,
        const std::string& name = "");

    static std::unique_ptr<CNFeature> CreateLineStringFeature(
        CNFeatureDefn* defn,
        int64_t fid,
        const std::vector<double>& coords,
        const std::string& name = "");

    static std::unique_ptr<CNFeature> CreatePolygonFeature(
        CNFeatureDefn* defn,
        int64_t fid,
        const std::vector<std::vector<double>>& rings,
        const std::string& name = "");

    static CNFeatureDefn* CreateSimpleFeatureDefn(const std::string& name = "test");
    static CNFeatureDefn* CreateComplexFeatureDefn(const std::string& name = "test");

    static std::vector<std::unique_ptr<CNFeature>> GenerateRandomPoints(
        CNFeatureDefn* defn,
        int count,
        double min_x, double max_x,
        double min_y, double max_y);

    static std::vector<std::unique_ptr<CNFeature>> GenerateGridPoints(
        CNFeatureDefn* defn,
        int rows, int cols,
        double min_x, double min_y,
        double cell_width, double cell_height);
};

class LayerTestUtils {
public:
    static bool CompareFeatures(const CNFeature* f1, const CNFeature* f2, bool compare_fid = true);
    static bool CompareGeometries(const CNGeometry* g1, const CNGeometry* g2);
    static bool CompareFieldValues(const CNFieldValue& v1, const CNFieldValue& v2);

    static void PrintFeature(const CNFeature* feature);
    static void PrintLayerSummary(const CNLayer* layer);

    static std::string GetTempFilePath(const std::string& prefix, const std::string& ext);
};

} // namespace testing
} // namespace ogc
