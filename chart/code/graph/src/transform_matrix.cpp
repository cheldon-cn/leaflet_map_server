#include "ogc/draw/transform_matrix.h"
#include <cmath>

namespace ogc {
namespace draw {

TransformMatrix::TransformMatrix() {
    m_matrix[0] = 1.0; m_matrix[1] = 0.0; m_matrix[2] = 0.0;
    m_matrix[3] = 0.0; m_matrix[4] = 1.0; m_matrix[5] = 0.0;
}

TransformMatrix::TransformMatrix(double m00, double m01, double m02,
                                 double m10, double m11, double m12) {
    m_matrix[0] = m00; m_matrix[1] = m01; m_matrix[2] = m02;
    m_matrix[3] = m10; m_matrix[4] = m11; m_matrix[5] = m12;
}

TransformMatrix TransformMatrix::Identity() {
    return TransformMatrix();
}

TransformMatrix TransformMatrix::CreateTranslation(double tx, double ty) {
    return TransformMatrix(1.0, 0.0, tx, 0.0, 1.0, ty);
}

TransformMatrix TransformMatrix::CreateScale(double sx, double sy) {
    return TransformMatrix(sx, 0.0, 0.0, 0.0, sy, 0.0);
}

TransformMatrix TransformMatrix::CreateRotation(double angle) {
    double c = std::cos(angle);
    double s = std::sin(angle);
    return TransformMatrix(c, -s, 0.0, s, c, 0.0);
}

TransformMatrix TransformMatrix::CreateRotation(double angle, double cx, double cy) {
    double c = std::cos(angle);
    double s = std::sin(angle);
    return TransformMatrix(c, -s, cx - c * cx + s * cy,
                           s, c, cy - s * cx - c * cy);
}

TransformMatrix TransformMatrix::CreateShear(double shx, double shy) {
    return TransformMatrix(1.0, shx, 0.0, shy, 1.0, 0.0);
}

double TransformMatrix::Get(int row, int col) const {
    if (row < 0 || row > 1 || col < 0 || col > 2) {
        return 0.0;
    }
    return m_matrix[row * 3 + col];
}

void TransformMatrix::Set(int row, int col, double value) {
    if (row >= 0 && row <= 1 && col >= 0 && col <= 2) {
        m_matrix[row * 3 + col] = value;
    }
}

TransformMatrix TransformMatrix::Multiply(const TransformMatrix& other) const {
    return TransformMatrix(
        m_matrix[0] * other.m_matrix[0] + m_matrix[1] * other.m_matrix[3],
        m_matrix[0] * other.m_matrix[1] + m_matrix[1] * other.m_matrix[4],
        m_matrix[0] * other.m_matrix[2] + m_matrix[1] * other.m_matrix[5] + m_matrix[2],
        m_matrix[3] * other.m_matrix[0] + m_matrix[4] * other.m_matrix[3],
        m_matrix[3] * other.m_matrix[1] + m_matrix[4] * other.m_matrix[4],
        m_matrix[3] * other.m_matrix[2] + m_matrix[4] * other.m_matrix[5] + m_matrix[5]
    );
}

TransformMatrix TransformMatrix::operator*(const TransformMatrix& other) const {
    return Multiply(other);
}

void TransformMatrix::Transform(double x, double y, double& outX, double& outY) const {
    outX = m_matrix[0] * x + m_matrix[1] * y + m_matrix[2];
    outY = m_matrix[3] * x + m_matrix[4] * y + m_matrix[5];
}

void TransformMatrix::Translate(double tx, double ty) {
    m_matrix[2] += m_matrix[0] * tx + m_matrix[1] * ty;
    m_matrix[5] += m_matrix[3] * tx + m_matrix[4] * ty;
}

void TransformMatrix::Scale(double sx, double sy) {
    m_matrix[0] *= sx;
    m_matrix[1] *= sx;
    m_matrix[2] *= sx;
    m_matrix[3] *= sy;
    m_matrix[4] *= sy;
    m_matrix[5] *= sy;
}

void TransformMatrix::Rotate(double angle) {
    double c = std::cos(angle);
    double s = std::sin(angle);
    double m0 = m_matrix[0], m1 = m_matrix[1];
    double m3 = m_matrix[3], m4 = m_matrix[4];
    
    m_matrix[0] = m0 * c + m3 * s;
    m_matrix[1] = m1 * c + m4 * s;
    m_matrix[3] = m3 * c - m0 * s;
    m_matrix[4] = m4 * c - m1 * s;
}

void TransformMatrix::Rotate(double angle, double cx, double cy) {
    Translate(cx, cy);
    Rotate(angle);
    Translate(-cx, -cy);
}

void TransformMatrix::Shear(double shx, double shy) {
    double m0 = m_matrix[0], m1 = m_matrix[1], m2 = m_matrix[2];
    double m3 = m_matrix[3], m4 = m_matrix[4], m5 = m_matrix[5];
    
    m_matrix[0] = m0 + m3 * shx;
    m_matrix[1] = m1 + m4 * shx;
    m_matrix[2] = m2 + m5 * shx;
    m_matrix[3] = m3 + m0 * shy;
    m_matrix[4] = m4 + m1 * shy;
    m_matrix[5] = m5 + m2 * shy;
}

double TransformMatrix::Determinant() const {
    return m_matrix[0] * m_matrix[4] - m_matrix[1] * m_matrix[3];
}

bool TransformMatrix::IsInvertible() const {
    return std::abs(Determinant()) > 1e-15;
}

TransformMatrix TransformMatrix::Inverse() const {
    double det = Determinant();
    if (std::abs(det) < 1e-15) {
        return Identity();
    }
    
    double invDet = 1.0 / det;
    return TransformMatrix(
        m_matrix[4] * invDet,
        -m_matrix[1] * invDet,
        (m_matrix[1] * m_matrix[5] - m_matrix[2] * m_matrix[4]) * invDet,
        -m_matrix[3] * invDet,
        m_matrix[0] * invDet,
        (m_matrix[2] * m_matrix[3] - m_matrix[0] * m_matrix[5]) * invDet
    );
}

bool TransformMatrix::IsIdentity() const {
    return std::abs(m_matrix[0] - 1.0) < 1e-10 &&
           std::abs(m_matrix[1]) < 1e-10 &&
           std::abs(m_matrix[2]) < 1e-10 &&
           std::abs(m_matrix[3]) < 1e-10 &&
           std::abs(m_matrix[4] - 1.0) < 1e-10 &&
           std::abs(m_matrix[5]) < 1e-10;
}

bool TransformMatrix::IsTranslation() const {
    return std::abs(m_matrix[0] - 1.0) < 1e-10 &&
           std::abs(m_matrix[1]) < 1e-10 &&
           std::abs(m_matrix[3]) < 1e-10 &&
           std::abs(m_matrix[4] - 1.0) < 1e-10;
}

bool TransformMatrix::IsUniformScale() const {
    return std::abs(m_matrix[0] - m_matrix[4]) < 1e-10 &&
           std::abs(m_matrix[1]) < 1e-10 &&
           std::abs(m_matrix[3]) < 1e-10;
}

double TransformMatrix::GetScaleX() const {
    return std::sqrt(m_matrix[0] * m_matrix[0] + m_matrix[1] * m_matrix[1]);
}

double TransformMatrix::GetScaleY() const {
    return std::sqrt(m_matrix[3] * m_matrix[3] + m_matrix[4] * m_matrix[4]);
}

double TransformMatrix::GetTranslationX() const {
    return m_matrix[2];
}

double TransformMatrix::GetTranslationY() const {
    return m_matrix[5];
}

double TransformMatrix::GetRotation() const {
    return std::atan2(m_matrix[3], m_matrix[0]);
}

void TransformMatrix::Reset() {
    m_matrix[0] = 1.0; m_matrix[1] = 0.0; m_matrix[2] = 0.0;
    m_matrix[3] = 0.0; m_matrix[4] = 1.0; m_matrix[5] = 0.0;
}

bool TransformMatrix::Equals(const TransformMatrix& other, double epsilon) const {
    for (int i = 0; i < 6; ++i) {
        if (std::abs(m_matrix[i] - other.m_matrix[i]) > epsilon) {
            return false;
        }
    }
    return true;
}

bool TransformMatrix::operator==(const TransformMatrix& other) const {
    return Equals(other);
}

bool TransformMatrix::operator!=(const TransformMatrix& other) const {
    return !Equals(other);
}

}  
}  
