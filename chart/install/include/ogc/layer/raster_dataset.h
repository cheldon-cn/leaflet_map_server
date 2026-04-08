#pragma once

#include "ogc/layer/raster_layer.h"

#include <memory>
#include <string>
#include <vector>

namespace ogc {

class OGC_LAYER_API CNRasterDataset {
public:
    virtual ~CNRasterDataset() = default;

    virtual const std::string& GetName() const = 0;
    virtual std::string GetPath() const = 0;
    virtual bool IsReadOnly() const = 0;

    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;
    virtual int GetBandCount() const = 0;

    virtual void* GetSpatialRef() = 0;
    virtual const void* GetSpatialRef() const = 0;

    virtual void GetGeoTransform(double* transform) const = 0;
    virtual CNStatus SetGeoTransform(const double* transform);

    virtual double GetPixelWidth() const;
    virtual double GetPixelHeight() const;

    virtual CNRasterBand* GetBand(int index) = 0;

    virtual CNStatus ReadRaster(
        int x_offset, int y_offset,
        int x_size, int y_size,
        void* buffer,
        int buffer_x_size, int buffer_y_size,
        CNDataType data_type,
        const int* band_list = nullptr,
        int band_count = 0) = 0;

    virtual CNStatus WriteRaster(
        int x_offset, int y_offset,
        int x_size, int y_size,
        const void* buffer,
        int buffer_x_size, int buffer_y_size,
        CNDataType data_type,
        const int* band_list = nullptr,
        int band_count = 0) = 0;

    static std::unique_ptr<CNRasterDataset> Open(
        const std::string& path,
        bool update = false);

    static std::unique_ptr<CNRasterDataset> Create(
        const std::string& path,
        int width, int height, int band_count,
        CNDataType data_type);
};

using CNRasterDatasetPtr = std::unique_ptr<CNRasterDataset>;

} // namespace ogc
