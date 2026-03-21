#pragma once

#include "ogc/layer/export.h"
#include "ogc/layer/layer.h"
#include "ogc/layer/layer_type.h"

#include <memory>
#include <string>
#include <vector>

namespace ogc {

enum class CNDataType {
    kByte,
    kUInt16,
    kInt16,
    kUInt32,
    kInt32,
    kFloat32,
    kFloat64,
    kComplexInt16,
    kComplexInt32,
    kComplexFloat32,
    kComplexFloat64
};

class OGC_LAYER_API CNRasterBand {
public:
    virtual ~CNRasterBand() = default;

    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;
    virtual CNDataType GetDataType() const = 0;
    virtual int GetNoDataValue(double& value) const = 0;
    virtual double GetNoDataValue() const = 0;
    virtual CNStatus ReadRaster(int x_offset, int y_offset, int width, int height,
                                  void* buffer, CNDataType buffer_type) = 0;
    virtual CNStatus WriteRaster(int x_offset, int y_offset, int width, int height,
                                   const void* buffer, CNDataType buffer_type) = 0;
    virtual double GetMinimum() const = 0;
    virtual double GetMaximum() const = 0;
    virtual double GetOffset() const = 0;
    virtual double GetScale() const = 0;
    virtual std::string GetDescription() const = 0;
};

class OGC_LAYER_API CNRasterLayer : public CNLayer {
public:
    CNRasterLayer() = default;
    ~CNRasterLayer() override = default;

    CNLayerType GetLayerType() const override {
        return CNLayerType::kRaster;
    }

    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;
    virtual int GetBandCount() const = 0;
    virtual double GetPixelWidth() const = 0;
    virtual double GetPixelHeight() const = 0;
    virtual CNDataType GetDataType() const = 0;

    virtual void GetGeoTransform(double* transform) const = 0;
    virtual CNStatus SetGeoTransform(const double* transform);

    virtual CNRasterBand* GetBand(int band_index) = 0;

    virtual CNStatus ReadRaster(int x_offset, int y_offset, int width, int height,
                                 int band_count, CNDataType buffer_type, void* buffer) = 0;

    virtual CNStatus WriteRaster(int x_offset, int y_offset, int width, int height,
                                  int band_count, CNDataType buffer_type, const void* buffer) = 0;

    virtual void* GetSpatialRef() override;
    virtual const void* GetSpatialRef() const override;

protected:
    void* spatial_ref_ = nullptr;
    std::vector<double> geo_transform_;
};

} // namespace ogc
