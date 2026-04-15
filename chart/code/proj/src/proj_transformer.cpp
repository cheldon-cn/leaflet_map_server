#include "ogc/proj/proj_transformer.h"
#include "ogc/geom/point.h"
#include "ogc/geom/linestring.h"
#include "ogc/geom/linearring.h"
#include "ogc/geom/polygon.h"
#include "ogc/geom/multipoint.h"
#include "ogc/geom/multilinestring.h"
#include "ogc/geom/multipolygon.h"
#include "ogc/geom/geometrycollection.h"
#include "ogc/geom/coordinate.h"
#include <sstream>
#include <algorithm>
#include <cmath>

#ifdef OGC_USE_PROJ
#include <proj.h>
#endif

namespace ogc {
namespace proj {

struct ProjTransformer::Impl {
    void* projContext;
    void* projTransform;
    void* projInverseTransform;
    std::string sourceCRS;
    std::string targetCRS;
    std::string name;
    std::string description;
    bool valid;
    mutable std::mutex mutex;
    
    Impl() : projContext(nullptr), projTransform(nullptr), projInverseTransform(nullptr), valid(false) {}
};

ProjTransformer::ProjTransformer()
    : impl_(new Impl())
{
}

ProjTransformer::ProjTransformer(const std::string& sourceCRS, const std::string& targetCRS)
    : impl_(new Impl())
{
    Initialize(sourceCRS, targetCRS);
}

ProjTransformer::~ProjTransformer()
{
    DestroyTransform();
}

bool ProjTransformer::IsValid() const
{
    return impl_->valid;
}

ogc::Coordinate ProjTransformer::Transform(const ogc::Coordinate& coord) const
{
    ogc::Coordinate result = coord;
    Transform(result.x, result.y);
    return result;
}

ogc::Coordinate ProjTransformer::TransformInverse(const ogc::Coordinate& coord) const
{
    ogc::Coordinate result = coord;
    TransformInverse(result.x, result.y);
    return result;
}

void ProjTransformer::Transform(double& x, double& y) const
{
    if (!impl_->valid) {
        return;
    }
    
#ifdef OGC_USE_PROJ
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    PJ_COORD input;
    PJ_COORD output;
    
    input.xy.x = x;
    input.xy.y = y;
    
    output = proj_trans(static_cast<PJ*>(impl_->projTransform), PJ_FWD, input);
    
    x = output.xy.x;
    y = output.xy.y;
#else
    (void)x;
    (void)y;
#endif
}

void ProjTransformer::TransformInverse(double& x, double& y) const
{
    if (!impl_->valid) {
        return;
    }
    
#ifdef OGC_USE_PROJ
    std::lock_guard<std::mutex> lock(impl_->mutex);
    
    PJ_COORD input;
    PJ_COORD output;
    
    input.xy.x = x;
    input.xy.y = y;
    
    output = proj_trans(static_cast<PJ*>(impl_->projTransform), PJ_INV, input);
    
    x = output.xy.x;
    y = output.xy.y;
#else
    (void)x;
    (void)y;
#endif
}

void ProjTransformer::TransformArray(double* x, double* y, size_t count) const
{
    if (!impl_->valid || !x || !y || count == 0) {
        return;
    }
    
    for (size_t i = 0; i < count; ++i) {
        Transform(x[i], y[i]);
    }
}

void ProjTransformer::TransformArrayInverse(double* x, double* y, size_t count) const
{
    if (!impl_->valid || !x || !y || count == 0) {
        return;
    }
    
    for (size_t i = 0; i < count; ++i) {
        TransformInverse(x[i], y[i]);
    }
}

ogc::Envelope ProjTransformer::Transform(const ogc::Envelope& env) const
{
    if (!impl_->valid) {
        return env;
    }
    
    double minX = env.GetMinX();
    double minY = env.GetMinY();
    double maxX = env.GetMaxX();
    double maxY = env.GetMaxY();
    
    Transform(minX, minY);
    Transform(maxX, maxY);
    
    double newMinX = std::min(minX, maxX);
    double newMinY = std::min(minY, maxY);
    double newMaxX = std::max(minX, maxX);
    double newMaxY = std::max(minY, maxY);
    
    double stepX = (env.GetMaxX() - env.GetMinX()) / 10.0;
    double stepY = (env.GetMaxY() - env.GetMinY()) / 10.0;
    
    for (int i = 0; i <= 10; ++i) {
        for (int j = 0; j <= 10; ++j) {
            double x = env.GetMinX() + i * stepX;
            double y = env.GetMinY() + j * stepY;
            Transform(x, y);
            newMinX = std::min(newMinX, x);
            newMinY = std::min(newMinY, y);
            newMaxX = std::max(newMaxX, x);
            newMaxY = std::max(newMaxY, y);
        }
    }
    
    return ogc::Envelope(newMinX, newMinY, newMaxX, newMaxY);
}

ogc::Envelope ProjTransformer::TransformInverse(const ogc::Envelope& env) const
{
    if (!impl_->valid) {
        return env;
    }
    
    double minX = env.GetMinX();
    double minY = env.GetMinY();
    double maxX = env.GetMaxX();
    double maxY = env.GetMaxY();
    
    TransformInverse(minX, minY);
    TransformInverse(maxX, maxY);
    
    double newMinX = std::min(minX, maxX);
    double newMinY = std::min(minY, maxY);
    double newMaxX = std::max(minX, maxX);
    double newMaxY = std::max(minY, maxY);
    
    double stepX = (env.GetMaxX() - env.GetMinX()) / 10.0;
    double stepY = (env.GetMaxY() - env.GetMinY()) / 10.0;
    
    for (int i = 0; i <= 10; ++i) {
        for (int j = 0; j <= 10; ++j) {
            double x = env.GetMinX() + i * stepX;
            double y = env.GetMinY() + j * stepY;
            TransformInverse(x, y);
            newMinX = std::min(newMinX, x);
            newMinY = std::min(newMinY, y);
            newMaxX = std::max(newMaxX, x);
            newMaxY = std::max(newMaxY, y);
        }
    }
    
    return ogc::Envelope(newMinX, newMinY, newMaxX, newMaxY);
}

ogc::GeometryPtr ProjTransformer::Transform(const ogc::Geometry* geometry) const
{
    return TransformGeometry(geometry, true);
}

ogc::GeometryPtr ProjTransformer::TransformInverse(const ogc::Geometry* geometry) const
{
    return TransformGeometry(geometry, false);
}

std::string ProjTransformer::GetSourceCRS() const
{
    return impl_->sourceCRS;
}

std::string ProjTransformer::GetTargetCRS() const
{
    return impl_->targetCRS;
}

std::string ProjTransformer::GetName() const
{
    return impl_->name;
}

std::string ProjTransformer::GetDescription() const
{
    return impl_->description;
}

CoordinateTransformerPtr ProjTransformer::Clone() const
{
    return Create(impl_->sourceCRS, impl_->targetCRS);
}

bool ProjTransformer::Initialize(const std::string& sourceCRS, const std::string& targetCRS)
{
    DestroyTransform();
    
    impl_->sourceCRS = sourceCRS;
    impl_->targetCRS = targetCRS;
    impl_->valid = false;
    
    std::stringstream ss;
    ss << "ProjTransform[" << sourceCRS << " -> " << targetCRS << "]";
    impl_->name = ss.str();
    
#ifdef OGC_USE_PROJ
    impl_->projContext = proj_context_create();
    if (!impl_->projContext) {
        return false;
    }
    
    std::string normalizedSource = NormalizeCRS(sourceCRS);
    std::string normalizedTarget = NormalizeCRS(targetCRS);
    
    impl_->projTransform = proj_create_crs_to_crs(
        static_cast<PJ_CONTEXT*>(impl_->projContext),
        normalizedSource.c_str(),
        normalizedTarget.c_str(),
        nullptr);
    
    if (!impl_->projTransform) {
        proj_context_destroy(static_cast<PJ_CONTEXT*>(impl_->projContext));
        impl_->projContext = nullptr;
        return false;
    }
    
    impl_->valid = true;
    return true;
#else
    return false;
#endif
}

void ProjTransformer::SetDescription(const std::string& description)
{
    impl_->description = description;
}

ProjTransformerPtr ProjTransformer::Create(const std::string& sourceCRS, const std::string& targetCRS)
{
    return std::make_shared<ProjTransformer>(sourceCRS, targetCRS);
}

ProjTransformerPtr ProjTransformer::CreateFromEpsg(int sourceEpsg, int targetEpsg)
{
    std::stringstream sourceSS, targetSS;
    sourceSS << "EPSG:" << sourceEpsg;
    targetSS << "EPSG:" << targetEpsg;
    return Create(sourceSS.str(), targetSS.str());
}

ProjTransformerPtr ProjTransformer::CreateFromWkt(const std::string& sourceWkt, const std::string& targetWkt)
{
    return Create(sourceWkt, targetWkt);
}

ProjTransformerPtr ProjTransformer::CreateFromProjString(const std::string& sourceProj, const std::string& targetProj)
{
    return Create(sourceProj, targetProj);
}

bool ProjTransformer::IsProjAvailable()
{
#ifdef OGC_USE_PROJ
    return true;
#else
    return false;
#endif
}

std::string ProjTransformer::GetProjVersion()
{
#ifdef OGC_USE_PROJ
    std::stringstream ss;
    ss << PROJ_VERSION_MAJOR << "." << PROJ_VERSION_MINOR << "." << PROJ_VERSION_PATCH;
    return ss.str();
#else
    return "PROJ library not available";
#endif
}

void ProjTransformer::DestroyTransform()
{
#ifdef OGC_USE_PROJ
    if (impl_->projTransform) {
        proj_destroy(static_cast<PJ*>(impl_->projTransform));
        impl_->projTransform = nullptr;
    }
    if (impl_->projContext) {
        proj_context_destroy(static_cast<PJ_CONTEXT*>(impl_->projContext));
        impl_->projContext = nullptr;
    }
#endif
    impl_->valid = false;
}

std::string ProjTransformer::NormalizeCRS(const std::string& crs) const
{
    std::string normalized = crs;
    
    std::transform(normalized.begin(), normalized.end(), normalized.begin(), ::toupper);
    
    if (normalized.find("EPSG:") == 0) {
        return crs;
    }
    
    if (normalized.find("WGS84") != std::string::npos || 
        normalized.find("WGS 84") != std::string::npos) {
        return "EPSG:4326";
    }
    
    if (normalized.find("WEBMERCATOR") != std::string::npos ||
        normalized.find("WEB MERCATOR") != std::string::npos ||
        normalized.find("MERCATOR") != std::string::npos) {
        return "EPSG:3857";
    }
    
    return crs;
}

bool ProjTransformer::IsEpsgCode(const std::string& crs) const
{
    std::string upper = crs;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    return upper.find("EPSG:") == 0;
}

int ProjTransformer::ParseEpsgCode(const std::string& crs) const
{
    std::string upper = crs;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    
    size_t pos = upper.find("EPSG:");
    if (pos == std::string::npos) {
        return -1;
    }
    
    std::string codeStr = crs.substr(pos + 5);
    return std::atoi(codeStr.c_str());
}

ogc::GeometryPtr ProjTransformer::TransformGeometry(const ogc::Geometry* geometry, bool forward) const
{
    if (!geometry) {
        return nullptr;
    }
    
    switch (geometry->GetGeometryType()) {
        case ogc::GeomType::kPoint:
            return TransformPoint(geometry, forward);
        case ogc::GeomType::kLineString:
            return TransformLineString(geometry, forward);
        case ogc::GeomType::kPolygon:
            return TransformPolygon(geometry, forward);
        case ogc::GeomType::kMultiPoint:
            return TransformMultiPoint(geometry, forward);
        case ogc::GeomType::kMultiLineString:
            return TransformMultiLineString(geometry, forward);
        case ogc::GeomType::kMultiPolygon:
            return TransformMultiPolygon(geometry, forward);
        case ogc::GeomType::kGeometryCollection:
            return TransformGeometryCollection(geometry, forward);
        default:
            return nullptr;
    }
}

ogc::GeometryPtr ProjTransformer::TransformPoint(const ogc::Geometry* geometry, bool forward) const
{
    const ogc::Point* point = dynamic_cast<const ogc::Point*>(geometry);
    if (!point) {
        return nullptr;
    }
    
    ogc::Coordinate coord = point->GetCoordinate();
    if (forward) {
        Transform(coord.x, coord.y);
    } else {
        TransformInverse(coord.x, coord.y);
    }
    
    return ogc::Point::Create(coord);
}

ogc::GeometryPtr ProjTransformer::TransformLineString(const ogc::Geometry* geometry, bool forward) const
{
    const ogc::LineString* lineString = dynamic_cast<const ogc::LineString*>(geometry);
    if (!lineString) {
        return nullptr;
    }
    
    std::vector<ogc::Coordinate> coords;
    coords.reserve(lineString->GetNumPoints());
    
    for (size_t i = 0; i < lineString->GetNumPoints(); ++i) {
        ogc::Coordinate coord = lineString->GetCoordinateN(i);
        if (forward) {
            Transform(coord.x, coord.y);
        } else {
            TransformInverse(coord.x, coord.y);
        }
        coords.push_back(coord);
    }
    
    return ogc::LineString::Create(coords);
}

ogc::GeometryPtr ProjTransformer::TransformPolygon(const ogc::Geometry* geometry, bool forward) const
{
    const ogc::Polygon* polygon = dynamic_cast<const ogc::Polygon*>(geometry);
    if (!polygon) {
        return nullptr;
    }
    
    std::vector<ogc::Coordinate> shellCoords;
    const ogc::LinearRing* shell = polygon->GetExteriorRing();
    if (shell) {
        shellCoords.reserve(shell->GetNumPoints());
        for (size_t i = 0; i < shell->GetNumPoints(); ++i) {
            ogc::Coordinate coord = shell->GetCoordinateN(i);
            if (forward) {
                Transform(coord.x, coord.y);
            } else {
                TransformInverse(coord.x, coord.y);
            }
            shellCoords.push_back(coord);
        }
    }
    
    ogc::LinearRingPtr exteriorRing = ogc::LinearRing::Create(shellCoords);
    ogc::PolygonPtr result = ogc::Polygon::Create(std::move(exteriorRing));
    
    for (size_t i = 0; i < polygon->GetNumInteriorRings(); ++i) {
        const ogc::LinearRing* hole = polygon->GetInteriorRingN(i);
        if (hole) {
            std::vector<ogc::Coordinate> holeCoordList;
            holeCoordList.reserve(hole->GetNumPoints());
            for (size_t j = 0; j < hole->GetNumPoints(); ++j) {
                ogc::Coordinate coord = hole->GetCoordinateN(j);
                if (forward) {
                    Transform(coord.x, coord.y);
                } else {
                    TransformInverse(coord.x, coord.y);
                }
                holeCoordList.push_back(coord);
            }
            ogc::LinearRingPtr interiorRing = ogc::LinearRing::Create(holeCoordList);
            result->AddInteriorRing(std::move(interiorRing));
        }
    }
    
    return result;
}

ogc::GeometryPtr ProjTransformer::TransformMultiPoint(const ogc::Geometry* geometry, bool forward) const
{
    const ogc::MultiPoint* multiPoint = dynamic_cast<const ogc::MultiPoint*>(geometry);
    if (!multiPoint) {
        return nullptr;
    }
    
    ogc::CoordinateList coords;
    coords.reserve(multiPoint->GetNumGeometries());
    
    for (size_t i = 0; i < multiPoint->GetNumGeometries(); ++i) {
        const ogc::Point* point = multiPoint->GetPointN(i);
        if (point) {
            ogc::Coordinate coord = point->GetCoordinate();
            if (forward) {
                Transform(coord.x, coord.y);
            } else {
                TransformInverse(coord.x, coord.y);
            }
            coords.push_back(coord);
        }
    }
    
    return ogc::MultiPoint::Create(coords);
}

ogc::GeometryPtr ProjTransformer::TransformMultiLineString(const ogc::Geometry* geometry, bool forward) const
{
    const ogc::MultiLineString* multiLineString = dynamic_cast<const ogc::MultiLineString*>(geometry);
    if (!multiLineString) {
        return nullptr;
    }
    
    std::vector<ogc::LineStringPtr> lineStrings;
    lineStrings.reserve(multiLineString->GetNumGeometries());
    
    for (size_t i = 0; i < multiLineString->GetNumGeometries(); ++i) {
        const ogc::LineString* lineString = multiLineString->GetLineStringN(i);
        if (lineString) {
            ogc::GeometryPtr transformed = TransformLineString(lineString, forward);
            ogc::LineStringPtr transformedLine(static_cast<ogc::LineString*>(transformed.release()));
            if (transformedLine) {
                lineStrings.push_back(std::move(transformedLine));
            }
        }
    }
    
    return ogc::MultiLineString::Create(std::move(lineStrings));
}

ogc::GeometryPtr ProjTransformer::TransformMultiPolygon(const ogc::Geometry* geometry, bool forward) const
{
    const ogc::MultiPolygon* multiPolygon = dynamic_cast<const ogc::MultiPolygon*>(geometry);
    if (!multiPolygon) {
        return nullptr;
    }
    
    std::vector<ogc::PolygonPtr> polygons;
    polygons.reserve(multiPolygon->GetNumGeometries());
    
    for (size_t i = 0; i < multiPolygon->GetNumGeometries(); ++i) {
        const ogc::Polygon* polygon = multiPolygon->GetPolygonN(i);
        if (polygon) {
            ogc::GeometryPtr transformed = TransformPolygon(polygon, forward);
            ogc::PolygonPtr transformedPolygon(static_cast<ogc::Polygon*>(transformed.release()));
            if (transformedPolygon) {
                polygons.push_back(std::move(transformedPolygon));
            }
        }
    }
    
    return ogc::MultiPolygon::Create(std::move(polygons));
}

ogc::GeometryPtr ProjTransformer::TransformGeometryCollection(const ogc::Geometry* geometry, bool forward) const
{
    const ogc::GeometryCollection* collection = dynamic_cast<const ogc::GeometryCollection*>(geometry);
    if (!collection) {
        return nullptr;
    }
    
    std::vector<ogc::GeometryPtr> geometries;
    geometries.reserve(collection->GetNumGeometries());
    
    for (size_t i = 0; i < collection->GetNumGeometries(); ++i) {
        const ogc::Geometry* geom = collection->GetGeometryN(i);
        ogc::GeometryPtr transformed = TransformGeometry(geom, forward);
        if (transformed) {
            geometries.push_back(std::move(transformed));
        }
    }
    
    return ogc::GeometryCollection::Create(std::move(geometries));
}

}
}
