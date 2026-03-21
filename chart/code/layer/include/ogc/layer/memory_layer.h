#pragma once

#include "ogc/layer/export.h"
#include "ogc/layer/layer.h"
#include "ogc/layer/layer_type.h"

#include "ogc/geometry.h"
#include "ogc/spatial_index.h"

#include <memory>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>
#include <set>

namespace ogc {

class CNFieldDefn;

class OGC_LAYER_API CNMemoryLayer : public CNLayer {
public:
    CNMemoryLayer(
        const std::string& name,
        GeomType geom_type,
        void* srs = nullptr);

    CNMemoryLayer(
        const std::string& name,
        std::shared_ptr<CNFeatureDefn> feature_defn);

    ~CNMemoryLayer() override;

    CNMemoryLayer(const CNMemoryLayer&) = delete;
    CNMemoryLayer& operator=(const CNMemoryLayer&) = delete;

    CNMemoryLayer(CNMemoryLayer&&) noexcept;
    CNMemoryLayer& operator=(CNMemoryLayer&&) noexcept;

    const std::string& GetName() const override {
        return name_;
    }

    CNLayerType GetLayerType() const override {
        return CNLayerType::kMemory;
    }

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
    int64_t CreateFeatureBatch(
        const std::vector<CNFeature*>& features) override;

    CNStatus CreateField(
        const CNFieldDefn* field_defn,
        bool approx_ok = false) override;
    CNStatus DeleteField(int field_index) override;
    CNStatus ReorderFields(const std::vector<int>& new_order) override;
    CNStatus AlterFieldDefn(
        int field_index,
        const CNFieldDefn* new_defn,
        int flags) override;

    void SetSpatialFilterRect(
        double min_x, double min_y,
        double max_x, double max_y) override;
    void SetSpatialFilter(const CNGeometry* geometry) override;
    const CNGeometry* GetSpatialFilter() const override;

    CNStatus SetAttributeFilter(const std::string& query) override;

    CNStatus StartTransaction() override;
    CNStatus CommitTransaction() override;
    CNStatus RollbackTransaction() override;

    bool TestCapability(CNLayerCapability capability) const override;

    std::unique_ptr<CNLayer> Clone() const override;

    void Reserve(int64_t expected_count);

    void ShrinkToFit();

    void Clear();

    int64_t GetMemoryUsage() const;

    void SetAutoFIDGeneration(bool auto_gen);

    void SetFIDReuse(bool reuse);

    void BuildSpatialIndex(SpatialIndexType index_type = SpatialIndexType::kRTree);

    ISpatialIndex<int64_t>* GetSpatialIndex();

    std::vector<int64_t> SpatialQuery(
        const CNGeometry* geometry,
        CNSpatialRelation relation = CNSpatialRelation::kIntersects);

private:
    int64_t GenerateFID();
    void UpdateExtent(const CNFeature* feature);
    void InvalidateExtent();
    void ApplySpatialFilter();
    bool PassesFilters(const CNFeature* feature) const;
    CNFeature* GetFeatureRef(int64_t fid);

    std::string name_;
    std::shared_ptr<CNFeatureDefn> feature_defn_;
    void* spatial_ref_;

    std::vector<std::unique_ptr<CNFeature>> features_;
    std::unordered_map<int64_t, size_t> fid_index_;

    int64_t next_fid_ = 1;
    bool auto_fid_generation_ = true;
    bool fid_reuse_ = true;
    std::set<int64_t> deleted_fids_;

    size_t read_cursor_ = 0;
    std::vector<size_t> filtered_indices_;

    std::unique_ptr<CNGeometry> spatial_filter_;
    std::string attribute_filter_;
    Envelope filter_extent_;

    std::unique_ptr<ISpatialIndex<int64_t>> spatial_index_;
    bool index_dirty_ = true;

    struct TransactionSnapshot {
        std::vector<std::unique_ptr<CNFeature>> features;
        std::unordered_map<int64_t, size_t> fid_index;
        int64_t next_fid;
        std::set<int64_t> deleted_fids;
    };
    std::stack<TransactionSnapshot> transaction_stack_;
    bool in_transaction_ = false;

    mutable Envelope cached_extent_;
    mutable bool extent_valid_ = false;
};

using CNMemoryLayerPtr = std::unique_ptr<CNMemoryLayer>;
using CNMemoryLayerRef = std::shared_ptr<CNMemoryLayer>;

} // namespace ogc
