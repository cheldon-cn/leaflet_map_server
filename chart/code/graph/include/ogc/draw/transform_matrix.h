#ifndef OGC_DRAW_TRANSFORM_MATRIX_H
#define OGC_DRAW_TRANSFORM_MATRIX_H

#include "ogc/draw/export.h"
#include <array>
#include <cmath>

namespace ogc {
namespace draw {

class OGC_GRAPH_API TransformMatrix {
public:
    TransformMatrix();
    TransformMatrix(double m00, double m01, double m02,
                    double m10, double m11, double m12);
    
    static TransformMatrix Identity();
    static TransformMatrix CreateTranslation(double tx, double ty);
    static TransformMatrix CreateScale(double sx, double sy);
    static TransformMatrix CreateRotation(double angle);
    static TransformMatrix CreateRotation(double angle, double cx, double cy);
    static TransformMatrix CreateShear(double shx, double shy);
    
    double Get(int row, int col) const;
    void Set(int row, int col, double value);
    
    double GetM00() const;
    double GetM01() const;
    double GetM02() const;
    double GetM10() const;
    double GetM11() const;
    double GetM12() const;
    
    void SetM00(double v);
    void SetM01(double v);
    void SetM02(double v);
    void SetM10(double v);
    void SetM11(double v);
    void SetM12(double v);
    
    TransformMatrix Multiply(const TransformMatrix& other) const;
    TransformMatrix operator*(const TransformMatrix& other) const;
    
    void Transform(double x, double y, double& outX, double& outY) const;
    
    void Translate(double tx, double ty);
    void Scale(double sx, double sy);
    void Rotate(double angle);
    void Rotate(double angle, double cx, double cy);
    void Shear(double shx, double shy);
    
    double Determinant() const;
    bool IsInvertible() const;
    TransformMatrix Inverse() const;
    
    bool IsIdentity() const;
    bool IsTranslation() const;
    bool IsUniformScale() const;
    
    double GetScaleX() const;
    double GetScaleY() const;
    double GetTranslationX() const;
    double GetTranslationY() const;
    double GetRotation() const;
    
    void Reset();
    
    bool Equals(const TransformMatrix& other, double epsilon = 1e-10) const;
    bool operator==(const TransformMatrix& other) const;
    bool operator!=(const TransformMatrix& other) const;

private:
    std::array<double, 6> m_matrix;
};

inline double TransformMatrix::GetM00() const { return m_matrix[0]; }
inline double TransformMatrix::GetM01() const { return m_matrix[1]; }
inline double TransformMatrix::GetM02() const { return m_matrix[2]; }
inline double TransformMatrix::GetM10() const { return m_matrix[3]; }
inline double TransformMatrix::GetM11() const { return m_matrix[4]; }
inline double TransformMatrix::GetM12() const { return m_matrix[5]; }

inline void TransformMatrix::SetM00(double v) { m_matrix[0] = v; }
inline void TransformMatrix::SetM01(double v) { m_matrix[1] = v; }
inline void TransformMatrix::SetM02(double v) { m_matrix[2] = v; }
inline void TransformMatrix::SetM10(double v) { m_matrix[3] = v; }
inline void TransformMatrix::SetM11(double v) { m_matrix[4] = v; }
inline void TransformMatrix::SetM12(double v) { m_matrix[5] = v; }

}  
}  

#endif  
