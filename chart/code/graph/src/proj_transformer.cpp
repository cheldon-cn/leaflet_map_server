#include "ogc/draw/proj_transformer.h"
#include "ogc/point.h"
#include "ogc/linestring.h"
#include "ogc/linearring.h"
#include "ogc/polygon.h"
#include "ogc/multipoint.h"
#include "ogc/multilinestring.h"
#include "ogc/multipolygon.h"
#include "ogc/geometrycollection.h"
#include "ogc/coordinate.h"
#include <sstream>
#include <algorithm>
#include <cmath>

#ifdef OGC_USE_PROJ
#include <proj.h>
#endif

namespace ogc {
namespace draw {

ProjTransformer::ProjTransformer()
    : m_projContext(nullptr)
    , m_projTransform(nullptr)
    , m_projInverseTransform(nullptr)
    , m_valid(false)
{
}

ProjTransformer::ProjTransformer(const std::string& sourceCRS, const std::string& targetCRS)
    : m_projContext(nullptr)
    , m_projTransform(nullptr)
    , m_projInverseTransform(nullptr)
    , m_valid(false)
{
    Initialize(sourceCRS, targetCRS);
}

ProjTransformer::~ProjTransformer()
{
    DestroyTransform();
}

bool ProjTransformer::IsValid() const
{
    return m_valid;
}

Coordinate ProjTransformer::Transform(const Coordinate& coord) const
{
    Coordinate result = coord;
    Transform(result.x, result.y);
    return result;
}

Coordinate ProjTransformer::TransformInverse(const Coordinate& coord) const
{
    Coordinate result = coord;
    TransformInverse(result.x, result.y);
    return result;
}

void ProjTransformer::Transform(double& x, double& y) const
{
    if (!m_valid) {
        return;
    }
    
#ifdef OGC_USE_PROJ
    std::lock_guard<std::mutex> lock(m_mutex);
    
    PJ_COORD input;
    PJ_COORD output;
    
    input.xy.x = x;
    input.xy.y = y;
    
    output = proj_trans(static_cast<PJ*>(m_projTransform), PJ_FWD, input);
    
    x = output.xy.x;
    y = output.xy.y;
#else
    (void)x;
    (void)y;
#endif
}

void ProjTransformer::TransformInverse(double& x, double& y) const
{
    if (!m_valid) {
        return;
    }
    
#ifdef OGC_USE_PROJ
    std::lock_guard<std::mutex> lock(m_mutex);
    
    PJ_COORD input;
    PJ_COORD output;
    
    input.xy.x = x;
    input.xy.y = y;
    
    output = proj_trans(static_cast<PJ*>(m_projTransform), PJ_INV, input);
    
    x = output.xy.x;
    y = output.xy.y;
#else
    (void)x;
    (void)y;
#endif
}

void ProjTransformer::TransformArray(double* x, double* y, size_t count) const
{
    if (!m_valid || !x || !y || count == 0) {
        return;
    }
    
    for (size_t i = 0; i < count; ++i) {
        Transform(x[i], y[i]);
    }
}

void ProjTransformer::TransformArrayInverse(double* x, double* y, size_t count) const
{
    if (!m_valid || !x || !y || count == 0) {
        return;
    }
    
    for (size_t i = 0; i < count; ++i) {
        TransformInverse(x[i], y[i]);
    }
}

Envelope ProjTransformer::Transform(const Envelope& env) const
{
    if (!m_valid) {
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
    
    return Envelope(newMinX, newMinY, newMaxX, newMaxY);
}

Envelope ProjTransformer::TransformInverse(const Envelope& env) const
{
    if (!m_valid) {
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
    
    return Envelope(newMinX, newMinY, newMaxX, newMaxY);
}

GeometryPtr ProjTransformer::Transform(const Geometry* geometry) const
{
    return TransformGeometry(geometry, true);
}

GeometryPtr ProjTransformer::TransformInverse(const Geometry* geometry) const
{
    return TransformGeometry(geometry, false);
}

std::string ProjTransformer::GetSourceCRS() const
{
    return m_sourceCRS;
}

std::string ProjTransformer::GetTargetCRS() const
{
    return m_targetCRS;
}

std::string ProjTransformer::GetName() const
{
    return m_name;
}

std::string ProjTransformer::GetDescription() const
{
    return m_description;
}

CoordinateTransformerPtr ProjTransformer::Clone() const
{
    return Create(m_sourceCRS, m_targetCRS);
}

bool ProjTransformer::Initialize(const std::string& sourceCRS, const std::string& targetCRS)
{
    DestroyTransform();
    
    m_sourceCRS = sourceCRS;
    m_targetCRS = targetCRS;
    m_valid = false;
    
    std::stringstream ss;
    ss << "ProjTransform[" << sourceCRS << " -> " << targetCRS << "]";
    m_name = ss.str();
    
#ifdef OGC_USE_PROJ
    m_projContext = proj_context_create();
    if (!m_projContext) {
        return false;
    }
    
    std::string normalizedSource = NormalizeCRS(sourceCRS);
    std::string normalizedTarget = NormalizeCRS(targetCRS);
    
    m_projTransform = proj_create_crs_to_crs(
        static_cast<PJ_CONTEXT*>(m_projContext),
        normalizedSource.c_str(),
        normalizedTarget.c_str(),
        nullptr);
    
    if (!m_projTransform) {
        proj_context_destroy(static_cast<PJ_CONTEXT*>(m_projContext));
        m_projContext = nullptr;
        return false;
    }
    
    m_valid = true;
    return true;
#else
    return false;
#endif
}

void ProjTransformer::SetDescription(const std::string& description)
{
    m_description = description;
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
    if (m_projTransform) {
        proj_destroy(static_cast<PJ*>(m_projTransform));
        m_projTransform = nullptr;
    }
    if (m_projContext) {
        proj_context_destroy(static_cast<PJ_CONTEXT*>(m_projContext));
        m_projContext = nullptr;
    }
#endif
    m_valid = false;
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

GeometryPtr ProjTransformer::TransformGeometry(const Geometry* geometry, bool forward) const
{
    if (!geometry) {
        return nullptr;
    }
    
    switch (geometry->GetGeometryType()) {
        case GeomType::kPoint:
            return TransformPoint(geometry, forward);
        case GeomType::kLineString:
            return TransformLineString(geometry, forward);
        case GeomType::kPolygon:
            return TransformPolygon(geometry, forward);
        case GeomType::kMultiPoint:
            return TransformMultiPoint(geometry, forward);
        case GeomType::kMultiLineString:
            return TransformMultiLineString(geometry, forward);
        case GeomType::kMultiPolygon:
            return TransformMultiPolygon(geometry, forward);
        case GeomType::kGeometryCollection:
            return TransformGeometryCollection(geometry, forward);
        default:
            return nullptr;
    }
}

GeometryPtr ProjTransformer::TransformPoint(const Geometry* geometry, bool forward) const
{
    const Point* point = dynamic_cast<const Point*>(geometry);
    if (!point) {
        return nullptr;
    }
    
    Coordinate coord = point->GetCoordinate();
    if (forward) {
        Transform(coord.x, coord.y);
    } else {
        TransformInverse(coord.x, coord.y);
    }
    
    return Point::Create(coord);
}

GeometryPtr ProjTransformer::TransformLineString(const Geometry* geometry, bool forward) const
{
    const LineString* lineString = dynamic_cast<const LineString*>(geometry);
    if (!lineString) {
        return nullptr;
    }
    
    std::vector<Coordinate> coords;
    coords.reserve(lineString->GetNumPoints());
    
    for (size_t i = 0; i < lineString->GetNumPoints(); ++i) {
        Coordinate coord = lineString->GetCoordinateN(i);
        if (forward) {
            Transform(coord.x, coord.y);
        } else {
            TransformInverse(coord.x, coord.y);
        }
        coords.push_back(coord);
    }
    
    return LineString::Create(coords);
}

GeometryPtr ProjTransformer::TransformPolygon(const Geometry* geometry, bool forward) const
{
    const Polygon* polygon = dynamic_cast<const Polygon*>(geometry);
    if (!polygon) {
        return nullptr;
    }
    
    std::vector<Coordinate> shellCoords;
    const LinearRing* shell = polygon->GetExteriorRing();
    if (shell) {
        shellCoords.reserve(shell->GetNumPoints());
        for (size_t i = 0; i < shell->GetNumPoints(); ++i) {
            Coordinate coord = shell->GetCoordinateN(i);
            if (forward) {
                Transform(coord.x, coord.y);
            } else {
                TransformInverse(coord.x, coord.y);
            }
            shellCoords.push_back(coord);
        }
    }
    
    LinearRingPtr exteriorRing = LinearRing::Create(shellCoords);
    PolygonPtr result = Polygon::Create(std::move(exteriorRing));
    
    for (size_t i = 0; i < polygon->GetNumInteriorRings(); ++i) {
        const LinearRing* hole = polygon->GetInteriorRingN(i);
        if (hole) {
            std::vector<Coordinate> holeCoordList;
            holeCoordList.reserve(hole->GetNumPoints());
            for (size_t j = 0; j < hole->GetNumPoints(); ++j) {
                Coordinate coord = hole->GetCoordinateN(j);
                if (forward) {
                    Transform(coord.x, coord.y);
                } else {
                    TransformInverse(coord.x, coord.y);
                }
                holeCoordList.push_back(coord);
            }
            LinearRingPtr interiorRing = LinearRing::Create(holeCoordList);
            result->AddInteriorRing(std::move(interiorRing));
        }
    }
    
    return result;
}

GeometryPtr ProjTransformer::TransformMultiPoint(const Geometry* geometry, bool forward) const
{
    const MultiPoint* multiPoint = dynamic_cast<const MultiPoint*>(geometry);
    if (!multiPoint) {
        return nullptr;
    }
    
    CoordinateList coords;
    coords.reserve(multiPoint->GetNumGeometries());
    
    for (size_t i = 0; i < multiPoint->GetNumGeometries(); ++i) {
        const Point* point = multiPoint->GetPointN(i);
        if (point) {
            Coordinate coord = point->GetCoordinate();
            if (forward) {
                Transform(coord.x, coord.y);
            } else {
                TransformInverse(coord.x, coord.y);
            }
            coords.push_back(coord);
        }
    }
    
    return MultiPoint::Create(coords);
}

GeometryPtr ProjTransformer::TransformMultiLineString(const Geometry* geometry, bool forward) const
{
    const MultiLineString* multiLineString = dynamic_cast<const MultiLineString*>(geometry);
    if (!multiLineString) {
        return nullptr;
    }
    
    std::vector<LineStringPtr> lineStrings;
    lineStrings.reserve(multiLineString->GetNumGeometries());
    
    for (size_t i = 0; i < multiLineString->GetNumGeometries(); ++i) {
        const LineString* lineString = multiLineString->GetLineStringN(i);
        if (lineString) {
            GeometryPtr transformed = TransformLineString(lineString, forward);
            LineStringPtr transformedLine(static_cast<LineString*>(transformed.release()));
            if (transformedLine) {
                lineStrings.push_back(std::move(transformedLine));
            }
        }
    }
    
    return MultiLineString::Create(std::move(lineStrings));
}

GeometryPtr ProjTransformer::TransformMultiPolygon(const Geometry* geometry, bool forward) const
{
    const MultiPolygon* multiPolygon = dynamic_cast<const MultiPolygon*>(geometry);
    if (!multiPolygon) {
        return nullptr;
    }
    
    std::vector<PolygonPtr> polygons;
    polygons.reserve(multiPolygon->GetNumGeometries());
    
    for (size_t i = 0; i < multiPolygon->GetNumGeometries(); ++i) {
        const Polygon* polygon = multiPolygon->GetPolygonN(i);
        if (polygon) {
            GeometryPtr transformed = TransformPolygon(polygon, forward);
            PolygonPtr transformedPolygon(static_cast<Polygon*>(transformed.release()));
            if (transformedPolygon) {
                polygons.push_back(std::move(transformedPolygon));
            }
        }
    }
    
    return MultiPolygon::Create(std::move(polygons));
}

GeometryPtr ProjTransformer::TransformGeometryCollection(const Geometry* geometry, bool forward) const
{
    const GeometryCollection* collection = dynamic_cast<const GeometryCollection*>(geometry);
    if (!collection) {
        return nullptr;
    }
    
    std::vector<GeometryPtr> geometries;
    geometries.reserve(collection->GetNumGeometries());
    
    for (size_t i = 0; i < collection->GetNumGeometries(); ++i) {
        const Geometry* geom = collection->GetGeometryN(i);
        GeometryPtr transformed = TransformGeometry(geom, forward);
        if (transformed) {
            geometries.push_back(std::move(transformed));
        }
    }
    
    return GeometryCollection::Create(std::move(geometries));
}

}
}
