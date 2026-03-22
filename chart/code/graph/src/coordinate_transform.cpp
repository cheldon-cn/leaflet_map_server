#include "ogc/draw/coordinate_transform.h"
#include "ogc/point.h"
#include "ogc/linestring.h"
#include "ogc/polygon.h"
#include "ogc/multipoint.h"
#include "ogc/multilinestring.h"
#include "ogc/multipolygon.h"
#include "ogc/geometrycollection.h"

namespace ogc {
namespace draw {

CoordinateTransform::CoordinateTransform()
    : m_inverseValid(true)
{
}

CoordinateTransform::CoordinateTransform(const TransformMatrix& matrix)
    : m_matrix(matrix)
    , m_inverseValid(false)
{
}

CoordinateTransform::CoordinateTransform(double offsetX, double offsetY, double scaleX, double scaleY)
    : m_inverseValid(false)
{
    m_matrix = TransformMatrix::CreateScale(scaleX, scaleY);
    TransformMatrix translate = TransformMatrix::CreateTranslation(offsetX, offsetY);
    m_matrix = translate * m_matrix;
}

CoordinateTransform::CoordinateTransform(double offsetX, double offsetY, double scale)
    : CoordinateTransform(offsetX, offsetY, scale, scale)
{
}

void CoordinateTransform::SetMatrix(const TransformMatrix& matrix)
{
    m_matrix = matrix;
    m_inverseValid = false;
}

TransformMatrix CoordinateTransform::GetMatrix() const
{
    return m_matrix;
}

void CoordinateTransform::SetOffset(double offsetX, double offsetY)
{
    double scaleX = m_matrix.GetScaleX();
    double scaleY = m_matrix.GetScaleY();
    double rotation = m_matrix.GetRotation();
    
    m_matrix = TransformMatrix::CreateTranslation(offsetX, offsetY);
    m_matrix.Scale(scaleX, scaleY);
    if (std::abs(rotation) > 1e-10) {
        m_matrix.Rotate(rotation);
    }
    m_inverseValid = false;
}

void CoordinateTransform::GetOffset(double& offsetX, double& offsetY) const
{
    offsetX = m_matrix.GetTranslationX();
    offsetY = m_matrix.GetTranslationY();
}

void CoordinateTransform::SetScale(double scaleX, double scaleY)
{
    double offsetX = m_matrix.GetTranslationX();
    double offsetY = m_matrix.GetTranslationY();
    double rotation = m_matrix.GetRotation();
    
    m_matrix = TransformMatrix::CreateTranslation(offsetX, offsetY);
    m_matrix.Scale(scaleX, scaleY);
    if (std::abs(rotation) > 1e-10) {
        m_matrix.Rotate(rotation);
    }
    m_inverseValid = false;
}

void CoordinateTransform::SetScale(double scale)
{
    SetScale(scale, scale);
}

void CoordinateTransform::GetScale(double& scaleX, double& scaleY) const
{
    scaleX = m_matrix.GetScaleX();
    scaleY = m_matrix.GetScaleY();
}

void CoordinateTransform::SetWorldToScreen(double worldMinX, double worldMinY, double worldMaxX, double worldMaxY,
                                           double screenMinX, double screenMinY, double screenMaxX, double screenMaxY)
{
    double worldWidth = worldMaxX - worldMinX;
    double worldHeight = worldMaxY - worldMinY;
    double screenWidth = screenMaxX - screenMinX;
    double screenHeight = screenMaxY - screenMinY;
    
    if (worldWidth <= 0 || worldHeight <= 0 || screenWidth <= 0 || screenHeight <= 0) {
        m_matrix = TransformMatrix();
        m_inverseValid = true;
        return;
    }
    
    double scaleX = screenWidth / worldWidth;
    double scaleY = screenHeight / worldHeight;
    
    double offsetX = screenMinX - worldMinX * scaleX;
    double offsetY = screenMinY - worldMinY * scaleY;
    
    m_matrix = TransformMatrix::CreateTranslation(offsetX, offsetY);
    TransformMatrix scaleMatrix = TransformMatrix::CreateScale(scaleX, scaleY);
    m_matrix = m_matrix * scaleMatrix;
    
    m_inverseValid = false;
}

void CoordinateTransform::Transform(double& x, double& y) const
{
    double outX, outY;
    m_matrix.Transform(x, y, outX, outY);
    x = outX;
    y = outY;
}

void CoordinateTransform::TransformInverse(double& x, double& y) const
{
    if (!m_inverseValid) {
        const_cast<CoordinateTransform*>(this)->UpdateInverse();
    }
    double outX, outY;
    m_inverseMatrix.Transform(x, y, outX, outY);
    x = outX;
    y = outY;
}

Coordinate CoordinateTransform::Transform(const Coordinate& coord) const
{
    Coordinate result = coord;
    Transform(result.x, result.y);
    return result;
}

Coordinate CoordinateTransform::TransformInverse(const Coordinate& coord) const
{
    Coordinate result = coord;
    TransformInverse(result.x, result.y);
    return result;
}

void CoordinateTransform::TransformArray(double* x, double* y, size_t count) const
{
    if (!x || !y || count == 0) {
        return;
    }
    
    for (size_t i = 0; i < count; ++i) {
        Transform(x[i], y[i]);
    }
}

void CoordinateTransform::TransformArrayInverse(double* x, double* y, size_t count) const
{
    if (!x || !y || count == 0) {
        return;
    }
    
    for (size_t i = 0; i < count; ++i) {
        TransformInverse(x[i], y[i]);
    }
}

Envelope CoordinateTransform::Transform(const Envelope& env) const
{
    double minX = env.GetMinX();
    double minY = env.GetMinY();
    double maxX = env.GetMaxX();
    double maxY = env.GetMaxY();
    
    Transform(minX, minY);
    Transform(maxX, maxY);
    
    return Envelope(std::min(minX, maxX), std::min(minY, maxY),
                    std::max(minX, maxX), std::max(minY, maxY));
}

Envelope CoordinateTransform::TransformInverse(const Envelope& env) const
{
    double minX = env.GetMinX();
    double minY = env.GetMinY();
    double maxX = env.GetMaxX();
    double maxY = env.GetMaxY();
    
    TransformInverse(minX, minY);
    TransformInverse(maxX, maxY);
    
    return Envelope(std::min(minX, maxX), std::min(minY, maxY),
                    std::max(minX, maxX), std::max(minY, maxY));
}

GeometryPtr CoordinateTransform::Transform(const Geometry* geometry) const
{
    return TransformGeometry(geometry, true);
}

GeometryPtr CoordinateTransform::TransformInverse(const Geometry* geometry) const
{
    return TransformGeometry(geometry, false);
}

void CoordinateTransform::Reset()
{
    m_matrix = TransformMatrix();
    m_inverseMatrix = TransformMatrix();
    m_inverseValid = true;
}

bool CoordinateTransform::IsIdentity() const
{
    return m_matrix.IsIdentity();
}

CoordinateTransformPtr CoordinateTransform::Clone() const
{
    return CoordinateTransformPtr(new CoordinateTransform(m_matrix));
}

void CoordinateTransform::Multiply(const CoordinateTransform& other)
{
    m_matrix = m_matrix * other.m_matrix;
    m_inverseValid = false;
}

void CoordinateTransform::PreMultiply(const CoordinateTransform& other)
{
    m_matrix = other.m_matrix * m_matrix;
    m_inverseValid = false;
}

CoordinateTransformPtr CoordinateTransform::Create()
{
    return CoordinateTransformPtr(new CoordinateTransform());
}

CoordinateTransformPtr CoordinateTransform::Create(const TransformMatrix& matrix)
{
    return CoordinateTransformPtr(new CoordinateTransform(matrix));
}

CoordinateTransformPtr CoordinateTransform::Create(double offsetX, double offsetY, double scaleX, double scaleY)
{
    return CoordinateTransformPtr(new CoordinateTransform(offsetX, offsetY, scaleX, scaleY));
}

CoordinateTransformPtr CoordinateTransform::CreateWorldToScreen(double worldMinX, double worldMinY,
                                                                 double worldMaxX, double worldMaxY,
                                                                 double screenMinX, double screenMinY,
                                                                 double screenMaxX, double screenMaxY)
{
    CoordinateTransformPtr transform(new CoordinateTransform());
    transform->SetWorldToScreen(worldMinX, worldMinY, worldMaxX, worldMaxY,
                                screenMinX, screenMinY, screenMaxX, screenMaxY);
    return transform;
}

void CoordinateTransform::UpdateInverse()
{
    m_inverseMatrix = m_matrix.Inverse();
    m_inverseValid = true;
}

GeometryPtr CoordinateTransform::TransformGeometry(const Geometry* geometry, bool forward) const
{
    if (!geometry) {
        return nullptr;
    }
    
    switch (geometry->GetGeometryType()) {
        case GeomType::kPoint: {
            const Point* point = dynamic_cast<const Point*>(geometry);
            if (!point) return nullptr;
            Coordinate coord = point->GetCoordinate();
            if (forward) {
                Transform(coord.x, coord.y);
            } else {
                TransformInverse(coord.x, coord.y);
            }
            return Point::Create(coord);
        }
        case GeomType::kLineString: {
            const LineString* lineString = dynamic_cast<const LineString*>(geometry);
            if (!lineString) return nullptr;
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
        case GeomType::kPolygon: {
            const Polygon* polygon = dynamic_cast<const Polygon*>(geometry);
            if (!polygon) return nullptr;
            
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
                    std::vector<Coordinate> holeCoords;
                    holeCoords.reserve(hole->GetNumPoints());
                    for (size_t j = 0; j < hole->GetNumPoints(); ++j) {
                        Coordinate coord = hole->GetCoordinateN(j);
                        if (forward) {
                            Transform(coord.x, coord.y);
                        } else {
                            TransformInverse(coord.x, coord.y);
                        }
                        holeCoords.push_back(coord);
                    }
                    LinearRingPtr interiorRing = LinearRing::Create(holeCoords);
                    result->AddInteriorRing(std::move(interiorRing));
                }
            }
            
            return result;
        }
        case GeomType::kMultiPoint: {
            const MultiPoint* multiPoint = dynamic_cast<const MultiPoint*>(geometry);
            if (!multiPoint) return nullptr;
            
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
        case GeomType::kMultiLineString: {
            const MultiLineString* multiLineString = dynamic_cast<const MultiLineString*>(geometry);
            if (!multiLineString) return nullptr;
            
            std::vector<LineStringPtr> lineStrings;
            lineStrings.reserve(multiLineString->GetNumGeometries());
            for (size_t i = 0; i < multiLineString->GetNumGeometries(); ++i) {
                const LineString* lineString = multiLineString->GetLineStringN(i);
                if (lineString) {
                    GeometryPtr transformed = TransformGeometry(lineString, forward);
                    LineStringPtr transformedLine(static_cast<LineString*>(transformed.release()));
                    if (transformedLine) {
                        lineStrings.push_back(std::move(transformedLine));
                    }
                }
            }
            return MultiLineString::Create(std::move(lineStrings));
        }
        case GeomType::kMultiPolygon: {
            const MultiPolygon* multiPolygon = dynamic_cast<const MultiPolygon*>(geometry);
            if (!multiPolygon) return nullptr;
            
            std::vector<PolygonPtr> polygons;
            polygons.reserve(multiPolygon->GetNumGeometries());
            for (size_t i = 0; i < multiPolygon->GetNumGeometries(); ++i) {
                const Polygon* polygon = multiPolygon->GetPolygonN(i);
                if (polygon) {
                    GeometryPtr transformed = TransformGeometry(polygon, forward);
                    PolygonPtr transformedPolygon(static_cast<Polygon*>(transformed.release()));
                    if (transformedPolygon) {
                        polygons.push_back(std::move(transformedPolygon));
                    }
                }
            }
            return MultiPolygon::Create(std::move(polygons));
        }
        case GeomType::kGeometryCollection: {
            const GeometryCollection* collection = dynamic_cast<const GeometryCollection*>(geometry);
            if (!collection) return nullptr;
            
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
        default:
            return nullptr;
    }
}

}
}
