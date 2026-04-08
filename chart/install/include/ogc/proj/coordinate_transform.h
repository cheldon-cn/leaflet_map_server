#ifndef OGC_PROJ_COORDINATE_TRANSFORM_H
#define OGC_PROJ_COORDINATE_TRANSFORM_H

#include <ogc/proj/transform_matrix.h>
#include <ogc/envelope.h>
#include <ogc/coordinate.h>
#include <ogc/geometry.h>
#include <memory>
#include <vector>

namespace ogc {
namespace proj {

class CoordinateTransform;
typedef std::unique_ptr<CoordinateTransform> CoordinateTransformPtr;

class OGC_PROJ_API CoordinateTransform {
public:
    CoordinateTransform();
    CoordinateTransform(const TransformMatrix& matrix);
    CoordinateTransform(double offsetX, double offsetY, double scaleX, double scaleY);
    CoordinateTransform(double offsetX, double offsetY, double scale);
    
    ~CoordinateTransform() = default;
    
    void SetMatrix(const TransformMatrix& matrix);
    TransformMatrix GetMatrix() const;
    
    void SetOffset(double offsetX, double offsetY);
    void GetOffset(double& offsetX, double& offsetY) const;
    
    void SetScale(double scaleX, double scaleY);
    void SetScale(double scale);
    void GetScale(double& scaleX, double& scaleY) const;
    
    void SetWorldToScreen(double worldMinX, double worldMinY, double worldMaxX, double worldMaxY,
                          double screenMinX, double screenMinY, double screenMaxX, double screenMaxY);
    
    void Transform(double& x, double& y) const;
    void TransformInverse(double& x, double& y) const;
    
    ogc::Coordinate Transform(const ogc::Coordinate& coord) const;
    ogc::Coordinate TransformInverse(const ogc::Coordinate& coord) const;
    
    void TransformArray(double* x, double* y, size_t count) const;
    void TransformArrayInverse(double* x, double* y, size_t count) const;
    
    ogc::Envelope Transform(const ogc::Envelope& env) const;
    ogc::Envelope TransformInverse(const ogc::Envelope& env) const;
    
    ogc::GeometryPtr Transform(const ogc::Geometry* geometry) const;
    ogc::GeometryPtr TransformInverse(const ogc::Geometry* geometry) const;
    
    void Reset();
    
    bool IsIdentity() const;
    
    CoordinateTransformPtr Clone() const;
    
    void Multiply(const CoordinateTransform& other);
    void PreMultiply(const CoordinateTransform& other);
    
    static CoordinateTransformPtr Create();
    static CoordinateTransformPtr Create(const TransformMatrix& matrix);
    static CoordinateTransformPtr Create(double offsetX, double offsetY, double scaleX, double scaleY);
    static CoordinateTransformPtr CreateWorldToScreen(double worldMinX, double worldMinY, 
                                                       double worldMaxX, double worldMaxY,
                                                       double screenMinX, double screenMinY,
                                                       double screenMaxX, double screenMaxY);
    
private:
    TransformMatrix m_matrix;
    TransformMatrix m_inverseMatrix;
    bool m_inverseValid;
    
    void UpdateInverse();
    ogc::GeometryPtr TransformGeometry(const ogc::Geometry* geometry, bool forward) const;
};

}
}

#endif
