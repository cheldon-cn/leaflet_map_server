#pragma once

#include "ogc/layer/export.h"
#include "ogc/layer/layer_type.h"

#include "ogc/feature/feature_defn.h"
#include "ogc/layer/geometry_compat.h"
#include "ogc/envelope.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <atomic>

namespace ogc {

class CNFeature;
class CNFieldDefn;

class OGC_LAYER_API CNLayer {
public:
    CNLayer() = default;

    virtual ~CNLayer() = default;

    CNLayer(const CNLayer&) = delete;
    CNLayer& operator=(const CNLayer&) = delete;

    CNLayer(CNLayer&&) = default;
    CNLayer& operator=(CNLayer&&) = default;

    virtual const std::string& GetName() const = 0;

    virtual CNLayerType GetLayerType() const = 0;

    virtual CNFeatureDefn* GetFeatureDefn() = 0;

    virtual const CNFeatureDefn* GetFeatureDefn() const = 0;

    virtual GeomType GetGeomType() const = 0;

    virtual void* GetSpatialRef() = 0;

    virtual const void* GetSpatialRef() const = 0;

    virtual CNStatus GetExtent(Envelope& extent, bool force = true) const = 0;

    virtual int64_t GetFeatureCount(bool force = true) const = 0;

    virtual void ResetReading() = 0;

    virtual std::unique_ptr<CNFeature> GetNextFeature() = 0;

    virtual CNFeature* GetNextFeatureRef() {
        return nullptr;
    }

    virtual std::unique_ptr<CNFeature> GetFeature(int64_t fid) = 0;

    virtual CNStatus SetFeature(const CNFeature* feature) {
        return CNStatus::kNotSupported;
    }

    virtual CNStatus CreateFeature(CNFeature* feature) {
        return CNStatus::kNotSupported;
    }

    virtual CNStatus DeleteFeature(int64_t fid) {
        return CNStatus::kNotSupported;
    }

    virtual int64_t CreateFeatureBatch(
        const std::vector<CNFeature*>& features) {
        int64_t count = 0;
        for (auto* f : features) {
            if (CreateFeature(f) == CNStatus::kSuccess) {
                count++;
            }
        }
        return count;
    }

    virtual CNStatus CreateField(
        const CNFieldDefn* field_defn,
        bool approx_ok = false) {
        return CNStatus::kNotSupported;
    }

    virtual CNStatus DeleteField(int field_index) {
        return CNStatus::kNotSupported;
    }

    virtual CNStatus ReorderFields(const std::vector<int>& new_order) {
        return CNStatus::kNotSupported;
    }

    virtual CNStatus AlterFieldDefn(
        int field_index,
        const CNFieldDefn* new_defn,
        int flags) {
        return CNStatus::kNotSupported;
    }

    virtual void SetSpatialFilterRect(
        double min_x, double min_y,
        double max_x, double max_y) {
        SetSpatialFilter(nullptr);
    }

    virtual void SetSpatialFilter(const CNGeometry* geometry) = 0;

    virtual const CNGeometry* GetSpatialFilter() const = 0;

    virtual CNStatus SetAttributeFilter(const std::string& query) = 0;

    virtual void ClearAttributeFilter() {
        SetAttributeFilter("");
    }

    virtual CNStatus StartTransaction() {
        return CNStatus::kNotSupported;
    }

    virtual CNStatus CommitTransaction() {
        return CNStatus::kNotSupported;
    }

    virtual CNStatus RollbackTransaction() {
        return CNStatus::kNotSupported;
    }

    virtual bool TestCapability(CNLayerCapability capability) const = 0;

    virtual std::vector<CNLayerCapability> GetCapabilities() const {
        std::vector<CNLayerCapability> caps;
        for (int i = 0; i < static_cast<int>(CNLayerCapability::kCurveGeometries); ++i) {
            if (TestCapability(static_cast<CNLayerCapability>(i))) {
                caps.push_back(static_cast<CNLayerCapability>(i));
            }
        }
        return caps;
    }

    virtual std::string GetMetadata(const std::string& key) const {
        (void)key;
        return "";
    }

    virtual CNStatus SetMetadata(
        const std::string& key,
        const std::string& value) {
        (void)key;
        (void)value;
        return CNStatus::kNotSupported;
    }

    virtual CNStatus SyncToDisk() {
        return CNStatus::kSuccess;
    }

    virtual std::unique_ptr<CNLayer> Clone() const = 0;

protected:
    virtual void OnFeatureDefnModified() {}

    virtual void OnFeatureModified() {}
};

using CNLayerPtr = std::unique_ptr<CNLayer>;
using CNLayerRef = std::shared_ptr<CNLayer>;

} // namespace ogc
