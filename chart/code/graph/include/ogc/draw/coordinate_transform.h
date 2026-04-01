#ifndef OGC_DRAW_COORDINATE_TRANSFORM_H
#define OGC_DRAW_COORDINATE_TRANSFORM_H

#include <ogc/draw/transform_matrix.h>
#include "ogc/envelope.h"
#include "ogc/coordinate.h"
#include "ogc/geometry.h"
#include <memory>
#include <vector>

namespace ogc {
namespace draw {

class CoordinateTransform;
typedef std::unique_ptr<CoordinateTransform> CoordinateTransformPtr;

class OGC_GRAPH_API CoordinateTransform {
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
    
    Coordinate Transform(const Coordinate& coord) const;
    Coordinate TransformInverse(const Coordinate& coord) const;
    
    void TransformArray(double* x, double* y, size_t count) const;
    void TransformArrayInverse(double* x, double* y, size_t count) const;
    
    Envelope Transform(const Envelope& env) const;
    Envelope TransformInverse(const Envelope& env) const;
    
    GeometryPtr Transform(const Geometry* geometry) const;
    GeometryPtr TransformInverse(const Geometry* geometry) const;
    
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
    GeometryPtr TransformGeometry(const Geometry* geometry, bool forward) const;
};

}
}

#endif
