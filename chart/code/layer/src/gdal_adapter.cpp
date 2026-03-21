#include "ogc/layer/gdal_adapter.h"
#include "ogc/feature/feature.h"
#include "ogc/geometry.h"

namespace ogc {

std::unique_ptr<CNLayer> CNGDALAdapter::WrapLayer(void* layer) {
    (void)layer;
    return nullptr;
}

void* CNGDALAdapter::UnwrapLayer(CNLayer* layer) {
    (void)layer;
    return nullptr;
}

std::unique_ptr<CNDataSource> CNGDALAdapter::WrapDataset(void* dataset) {
    (void)dataset;
    return nullptr;
}

void* CNGDALAdapter::UnwrapDataset(CNDataSource* datasource) {
    (void)datasource;
    return nullptr;
}

std::unique_ptr<CNFeature> CNGDALAdapter::ConvertFeature(
    const void* feature, void* defn) {
    (void)feature;
    (void)defn;
    return nullptr;
}

void* CNGDALAdapter::ConvertFeature(
    const CNFeature* feature, void* defn) {
    (void)feature;
    (void)defn;
    return nullptr;
}

std::unique_ptr<CNGeometry> CNGDALAdapter::ConvertGeometry(
    const void* geometry) {
    (void)geometry;
    return nullptr;
}

void* CNGDALAdapter::ConvertGeometry(const CNGeometry* geometry) {
    (void)geometry;
    return nullptr;
}

bool CNGDALAdapter::IsGDALAvailable() {
    return false;
}

} // namespace ogc
