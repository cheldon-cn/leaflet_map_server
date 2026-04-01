#ifndef OGC_DRAW_TRANSFORM_MATRIX_H
#define OGC_DRAW_TRANSFORM_MATRIX_H

#include "ogc/draw/export.h"
#include "ogc/draw/geometry_types.h"
#include <cmath>
#include <cstring>

namespace ogc {
namespace draw {

class OGC_DRAW_API TransformMatrix {
public:
    double m[3][3];

    TransformMatrix() {
        LoadIdentity();
    }

    TransformMatrix(double m00, double m01, double m02,
                    double m10, double m11, double m12) {
        std::memset(m, 0, sizeof(m));
        m[0][0] = m00; m[0][1] = m01; m[0][2] = m02;
        m[1][0] = m10; m[1][1] = m11; m[1][2] = m12;
        m[2][2] = 1.0;
    }

    void LoadIdentity() {
        std::memset(m, 0, sizeof(m));
        m[0][0] = 1.0;
        m[1][1] = 1.0;
        m[2][2] = 1.0;
    }

    void Reset() {
        LoadIdentity();
    }

    static TransformMatrix Identity() {
        TransformMatrix mat;
        return mat;
    }

    static TransformMatrix Translate(double dx, double dy) {
        TransformMatrix mat;
        mat.m[0][2] = dx;
        mat.m[1][2] = dy;
        return mat;
    }

    static TransformMatrix CreateTranslation(double tx, double ty) {
        return Translate(tx, ty);
    }

    static TransformMatrix Rotate(double angleRadians) {
        TransformMatrix mat;
        double c = std::cos(angleRadians);
        double s = std::sin(angleRadians);
        mat.m[0][0] = c;
        mat.m[0][1] = -s;
        mat.m[1][0] = s;
        mat.m[1][1] = c;
        return mat;
    }

    static TransformMatrix Rotate(double angleRadians, double cx, double cy) {
        double c = std::cos(angleRadians);
        double s = std::sin(angleRadians);
        return TransformMatrix(c, -s, cx - c * cx + s * cy,
                               s, c, cy - s * cx - c * cy);
    }

    static TransformMatrix RotateDegrees(double angleDegrees) {
        return Rotate(angleDegrees * 3.14159265358979323846 / 180.0);
    }

    static TransformMatrix CreateRotation(double angle) {
        return Rotate(angle);
    }

    static TransformMatrix CreateRotation(double angle, double cx, double cy) {
        return Rotate(angle, cx, cy);
    }

    static TransformMatrix Scale(double sx, double sy) {
        TransformMatrix mat;
        mat.m[0][0] = sx;
        mat.m[1][1] = sy;
        return mat;
    }

    static TransformMatrix ScaleUniform(double s) {
        return Scale(s, s);
    }

    static TransformMatrix CreateScale(double sx, double sy) {
        return Scale(sx, sy);
    }

    static TransformMatrix Shear(double shx, double shy) {
        TransformMatrix mat;
        mat.m[0][1] = shx;
        mat.m[1][0] = shy;
        return mat;
    }

    static TransformMatrix CreateShear(double shx, double shy) {
        return Shear(shx, shy);
    }

    double Get(int row, int col) const {
        if (row < 0 || row > 2 || col < 0 || col > 2) {
            return 0.0;
        }
        return m[row][col];
    }

    void Set(int row, int col, double value) {
        if (row >= 0 && row <= 2 && col >= 0 && col <= 2) {
            m[row][col] = value;
        }
    }

    double GetM00() const { return m[0][0]; }
    double GetM01() const { return m[0][1]; }
    double GetM02() const { return m[0][2]; }
    double GetM10() const { return m[1][0]; }
    double GetM11() const { return m[1][1]; }
    double GetM12() const { return m[1][2]; }

    void SetM00(double v) { m[0][0] = v; }
    void SetM01(double v) { m[0][1] = v; }
    void SetM02(double v) { m[0][2] = v; }
    void SetM10(double v) { m[1][0] = v; }
    void SetM11(double v) { m[1][1] = v; }
    void SetM12(double v) { m[1][2] = v; }

    TransformMatrix operator*(const TransformMatrix& other) const {
        TransformMatrix result;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                result.m[i][j] = 0;
                for (int k = 0; k < 3; ++k) {
                    result.m[i][j] += m[i][k] * other.m[k][j];
                }
            }
        }
        return result;
    }

    TransformMatrix Multiply(const TransformMatrix& other) const {
        return *this * other;
    }

    TransformMatrix& operator*=(const TransformMatrix& other) {
        *this = *this * other;
        return *this;
    }

    TransformMatrix Inverse() const {
        double det = Determinant();
        if (std::abs(det) < 1e-15) {
            return Identity();
        }

        TransformMatrix inv;
        inv.m[0][0] = m[1][1] * m[2][2] - m[1][2] * m[2][1];
        inv.m[0][1] = m[0][2] * m[2][1] - m[0][1] * m[2][2];
        inv.m[0][2] = m[0][1] * m[1][2] - m[0][2] * m[1][1];

        inv.m[1][0] = m[1][2] * m[2][0] - m[1][0] * m[2][2];
        inv.m[1][1] = m[0][0] * m[2][2] - m[0][2] * m[2][0];
        inv.m[1][2] = m[0][2] * m[1][0] - m[0][0] * m[1][2];

        inv.m[2][0] = m[1][0] * m[2][1] - m[1][1] * m[2][0];
        inv.m[2][1] = m[0][1] * m[2][0] - m[0][0] * m[2][1];
        inv.m[2][2] = m[0][0] * m[1][1] - m[0][1] * m[1][0];

        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                inv.m[i][j] /= det;
            }
        }

        return inv;
    }

    double Determinant() const {
        return m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -
               m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
               m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
    }

    void TransformPoint(double x, double y, double& outX, double& outY) const {
        double w = m[2][0] * x + m[2][1] * y + m[2][2];
        outX = (m[0][0] * x + m[0][1] * y + m[0][2]) / w;
        outY = (m[1][0] * x + m[1][1] * y + m[1][2]) / w;
    }

    void Transform(double x, double y, double& outX, double& outY) const {
        TransformPoint(x, y, outX, outY);
    }

    Point TransformPoint(const Point& pt) const {
        double x, y;
        TransformPoint(pt.x, pt.y, x, y);
        return Point(x, y);
    }

    Rect TransformRect(const Rect& rect) const {
        Point corners[4];
        corners[0] = TransformPoint(Point(rect.x, rect.y));
        corners[1] = TransformPoint(Point(rect.x + rect.w, rect.y));
        corners[2] = TransformPoint(Point(rect.x + rect.w, rect.y + rect.h));
        corners[3] = TransformPoint(Point(rect.x, rect.y + rect.h));

        double minX = corners[0].x;
        double maxX = corners[0].x;
        double minY = corners[0].y;
        double maxY = corners[0].y;

        for (int i = 1; i < 4; ++i) {
            minX = std::min(minX, corners[i].x);
            maxX = std::max(maxX, corners[i].x);
            minY = std::min(minY, corners[i].y);
            maxY = std::max(maxY, corners[i].y);
        }

        return Rect(minX, minY, maxX - minX, maxY - minY);
    }

    void ApplyTranslate(double tx, double ty) {
        m[0][2] += (m[0][0] * tx + m[0][1] * ty);
        m[1][2] += (m[1][0] * tx + m[1][1] * ty);
    }

    void ApplyScale(double sx, double sy) {
        m[0][0] *= sx;
        m[0][1] *= sx;
        m[0][2] *= sx;
        m[1][0] *= sy;
        m[1][1] *= sy;
        m[1][2] *= sy;
    }

    void ApplyRotate(double angle) {
        double c = std::cos(angle);
        double s = std::sin(angle);
        double m00 = m[0][0], m01 = m[0][1];
        double m10 = m[1][0], m11 = m[1][1];
        
        m[0][0] = m00 * c + m10 * s;
        m[0][1] = m01 * c + m11 * s;
        m[1][0] = m10 * c - m00 * s;
        m[1][1] = m11 * c - m01 * s;
    }

    void ApplyRotate(double angle, double cx, double cy) {
        ApplyTranslate(cx, cy);
        ApplyRotate(angle);
        ApplyTranslate(-cx, -cy);
    }

    void ApplyShear(double shx, double shy) {
        double m00 = m[0][0], m01 = m[0][1], m02 = m[0][2];
        double m10 = m[1][0], m11 = m[1][1], m12 = m[1][2];
        
        m[0][0] = m00 + m10 * shx;
        m[0][1] = m01 + m11 * shx;
        m[0][2] = m02 + m12 * shx;
        m[1][0] = m10 + m00 * shy;
        m[1][1] = m11 + m01 * shy;
        m[1][2] = m12 + m02 * shy;
    }

    bool IsIdentity() const {
        return std::abs(m[0][0] - 1.0) < 1e-10 &&
               std::abs(m[0][1]) < 1e-10 &&
               std::abs(m[0][2]) < 1e-10 &&
               std::abs(m[1][0]) < 1e-10 &&
               std::abs(m[1][1] - 1.0) < 1e-10 &&
               std::abs(m[1][2]) < 1e-10 &&
               std::abs(m[2][0]) < 1e-10 &&
               std::abs(m[2][1]) < 1e-10 &&
               std::abs(m[2][2] - 1.0) < 1e-10;
    }

    bool IsInvertible() const {
        return std::abs(Determinant()) >= 1e-15;
    }

    bool IsTranslationOnly() const {
        return std::abs(m[0][0] - 1.0) < 1e-10 &&
               std::abs(m[0][1]) < 1e-10 &&
               std::abs(m[1][0]) < 1e-10 &&
               std::abs(m[1][1] - 1.0) < 1e-10 &&
               std::abs(m[2][0]) < 1e-10 &&
               std::abs(m[2][1]) < 1e-10 &&
               std::abs(m[2][2] - 1.0) < 1e-10;
    }

    bool IsTranslation() const {
        return IsTranslationOnly();
    }

    bool IsUniformScale() const {
        return std::abs(m[0][0] - m[1][1]) < 1e-10 &&
               std::abs(m[0][1]) < 1e-10 &&
               std::abs(m[1][0]) < 1e-10 &&
               std::abs(m[2][0]) < 1e-10 &&
               std::abs(m[2][1]) < 1e-10 &&
               std::abs(m[2][2] - 1.0) < 1e-10;
    }

    double GetScaleX() const {
        return std::sqrt(m[0][0] * m[0][0] + m[1][0] * m[1][0]);
    }

    double GetScaleY() const {
        return std::sqrt(m[0][1] * m[0][1] + m[1][1] * m[1][1]);
    }

    double GetRotation() const {
        return std::atan2(m[1][0], m[0][0]);
    }

    double GetTranslationX() const {
        return m[0][2];
    }

    double GetTranslationY() const {
        return m[1][2];
    }

    void PreTranslate(double dx, double dy) {
        *this = Translate(dx, dy) * (*this);
    }

    void PostTranslate(double dx, double dy) {
        *this = (*this) * Translate(dx, dy);
    }

    void PreRotate(double angleRadians) {
        *this = Rotate(angleRadians) * (*this);
    }

    void PostRotate(double angleRadians) {
        *this = (*this) * Rotate(angleRadians);
    }

    void PreScale(double sx, double sy) {
        *this = Scale(sx, sy) * (*this);
    }

    void PostScale(double sx, double sy) {
        *this = (*this) * Scale(sx, sy);
    }

    bool Equals(const TransformMatrix& other, double epsilon = 1e-10) const {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (std::abs(m[i][j] - other.m[i][j]) > epsilon) {
                    return false;
                }
            }
        }
        return true;
    }

    bool operator==(const TransformMatrix& other) const {
        return Equals(other);
    }

    bool operator!=(const TransformMatrix& other) const {
        return !Equals(other);
    }
};

}  
}  

#endif  
