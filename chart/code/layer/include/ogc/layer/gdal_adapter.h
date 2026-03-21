#pragma once

#include "ogc/layer/export.h"
#include "ogc/layer/layer.h"
#include "ogc/layer/datasource.h"

#include <memory>
#include <string>

namespace ogc {

class OGC_LAYER_API CNGDALAdapter {
public:
    static std::unique_ptr<CNLayer> WrapLayer(void* layer);

    static void* UnwrapLayer(CNLayer* layer);

    static std::unique_ptr<CNDataSource> WrapDataset(void* dataset);

    static void* UnwrapDataset(CNDataSource* datasource);

    static std::unique_ptr<CNFeature> ConvertFeature(
        const void* feature,
        void* defn = nullptr);

    static void* ConvertFeature(
        const CNFeature* feature,
        void* defn = nullptr);

    static std::unique_ptr<CNGeometry> ConvertGeometry(
        const void* geometry);

    static void* ConvertGeometry(
        const CNGeometry* geometry);

    static bool IsGDALAvailable();
};

} // namespace ogc
