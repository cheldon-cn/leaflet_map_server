#pragma once

#include "ogc/layer/export.h"
#include "ogc/layer/layer.h"
#include "ogc/layer/layer_type.h"

#include <memory>
#include <string>

namespace ogc {

class OGC_LAYER_API CNVectorLayer : public CNLayer {
public:
    CNVectorLayer() = default;
    ~CNVectorLayer() override = default;

    CNLayerType GetLayerType() const override {
        return CNLayerType::kVector;
    }

    virtual std::string GetDataSourcePath() const = 0;

    virtual std::string GetFormatName() const = 0;

    virtual bool IsReadOnly() const = 0;

    virtual std::string GetEncoding() const {
        return "UTF-8";
    }

    virtual int GetGeomFieldCount() const {
        return 1;
    }

    virtual const void* GetGeomFieldDefn(int index) const;

    virtual CNStatus SetCoordinateTransform(const void* target_srs);

    virtual void ClearCoordinateTransform();

protected:
    void* coord_transform_ = nullptr;
};

using CNVectorLayerPtr = std::unique_ptr<CNVectorLayer>;
using CNVectorLayerRef = std::shared_ptr<CNVectorLayer>;

} // namespace ogc
