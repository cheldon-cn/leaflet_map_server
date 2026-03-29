#include <gtest/gtest.h>
#include "ogc/draw/transform_matrix.h"
#include <cmath>

using namespace ogc::draw;

class TransformMatrixTest : public ::testing::Test {
protected:
    void SetUp() override {
        identity = TransformMatrix::Identity();
    }
    
    TransformMatrix identity;
};

TEST_F(TransformMatrixTest, IdentityConstructor) {
    EXPECT_TRUE(identity.IsIdentity());
    EXPECT_DOUBLE_EQ(identity.m[0][0], 1.0);
    EXPECT_DOUBLE_EQ(identity.m[1][1], 1.0);
    EXPECT_DOUBLE_EQ(identity.m[2][2], 1.0);
    EXPECT_DOUBLE_EQ(identity.m[0][1], 0.0);
    EXPECT_DOUBLE_EQ(identity.m[0][2], 0.0);
    EXPECT_DOUBLE_EQ(identity.m[1][0], 0.0);
    EXPECT_DOUBLE_EQ(identity.m[1][2], 0.0);
    EXPECT_DOUBLE_EQ(identity.m[2][0], 0.0);
    EXPECT_DOUBLE_EQ(identity.m[2][1], 0.0);
}

TEST_F(TransformMatrixTest, Translate) {
    TransformMatrix t = TransformMatrix::Translate(10.0, 20.0);
    
    EXPECT_DOUBLE_EQ(t.m[0][2], 10.0);
    EXPECT_DOUBLE_EQ(t.m[1][2], 20.0);
    EXPECT_TRUE(t.IsTranslationOnly());
}

TEST_F(TransformMatrixTest, Rotate) {
    TransformMatrix r = TransformMatrix::Rotate(3.14159265358979323846 / 2.0);
    
    EXPECT_NEAR(r.m[0][0], 0.0, 1e-10);
    EXPECT_NEAR(r.m[0][1], -1.0, 1e-10);
    EXPECT_NEAR(r.m[1][0], 1.0, 1e-10);
    EXPECT_NEAR(r.m[1][1], 0.0, 1e-10);
}

TEST_F(TransformMatrixTest, RotateDegrees) {
    TransformMatrix r = TransformMatrix::RotateDegrees(90.0);
    
    EXPECT_NEAR(r.m[0][0], 0.0, 1e-10);
    EXPECT_NEAR(r.m[0][1], -1.0, 1e-10);
    EXPECT_NEAR(r.m[1][0], 1.0, 1e-10);
    EXPECT_NEAR(r.m[1][1], 0.0, 1e-10);
}

TEST_F(TransformMatrixTest, Scale) {
    TransformMatrix s = TransformMatrix::Scale(2.0, 3.0);
    
    EXPECT_DOUBLE_EQ(s.m[0][0], 2.0);
    EXPECT_DOUBLE_EQ(s.m[1][1], 3.0);
}

TEST_F(TransformMatrixTest, ScaleUniform) {
    TransformMatrix s = TransformMatrix::ScaleUniform(2.0);
    
    EXPECT_DOUBLE_EQ(s.m[0][0], 2.0);
    EXPECT_DOUBLE_EQ(s.m[1][1], 2.0);
    EXPECT_TRUE(s.IsUniformScale());
}

TEST_F(TransformMatrixTest, Shear) {
    TransformMatrix sh = TransformMatrix::Shear(0.5, 0.25);
    
    EXPECT_DOUBLE_EQ(sh.m[0][1], 0.5);
    EXPECT_DOUBLE_EQ(sh.m[1][0], 0.25);
}

TEST_F(TransformMatrixTest, Multiply) {
    TransformMatrix t1 = TransformMatrix::Translate(10.0, 20.0);
    TransformMatrix t2 = TransformMatrix::Scale(2.0, 2.0);
    TransformMatrix result = t1 * t2;
    
    Point pt(0.0, 0.0);
    Point transformed = result.TransformPoint(pt);
    
    EXPECT_DOUBLE_EQ(transformed.x, 10.0);
    EXPECT_DOUBLE_EQ(transformed.y, 20.0);
}

TEST_F(TransformMatrixTest, MultiplyAssign) {
    TransformMatrix t = TransformMatrix::Identity();
    t *= TransformMatrix::Translate(10.0, 20.0);
    
    EXPECT_DOUBLE_EQ(t.m[0][2], 10.0);
    EXPECT_DOUBLE_EQ(t.m[1][2], 20.0);
}

TEST_F(TransformMatrixTest, Inverse) {
    TransformMatrix t = TransformMatrix::Translate(10.0, 20.0);
    TransformMatrix inv = t.Inverse();
    TransformMatrix result = t * inv;
    
    EXPECT_TRUE(result.IsIdentity());
}

TEST_F(TransformMatrixTest, InverseScale) {
    TransformMatrix s = TransformMatrix::Scale(2.0, 4.0);
    TransformMatrix inv = s.Inverse();
    TransformMatrix result = s * inv;
    
    EXPECT_TRUE(result.IsIdentity());
}

TEST_F(TransformMatrixTest, Determinant) {
    TransformMatrix t = TransformMatrix::Scale(2.0, 3.0);
    double det = t.Determinant();
    
    EXPECT_DOUBLE_EQ(det, 6.0);
}

TEST_F(TransformMatrixTest, TransformPoint) {
    TransformMatrix t = TransformMatrix::Translate(10.0, 20.0);
    
    double x, y;
    t.TransformPoint(0.0, 0.0, x, y);
    
    EXPECT_DOUBLE_EQ(x, 10.0);
    EXPECT_DOUBLE_EQ(y, 20.0);
}

TEST_F(TransformMatrixTest, TransformPointWithScale) {
    TransformMatrix s = TransformMatrix::Scale(2.0, 3.0);
    
    double x, y;
    s.TransformPoint(10.0, 20.0, x, y);
    
    EXPECT_DOUBLE_EQ(x, 20.0);
    EXPECT_DOUBLE_EQ(y, 60.0);
}

TEST_F(TransformMatrixTest, TransformPointWithRotation) {
    TransformMatrix r = TransformMatrix::Rotate(3.14159265358979323846);
    
    double x, y;
    r.TransformPoint(1.0, 0.0, x, y);
    
    EXPECT_NEAR(x, -1.0, 1e-10);
    EXPECT_NEAR(y, 0.0, 1e-10);
}

TEST_F(TransformMatrixTest, TransformRect) {
    TransformMatrix t = TransformMatrix::Translate(10.0, 20.0);
    Rect r(0.0, 0.0, 100.0, 200.0);
    
    Rect result = t.TransformRect(r);
    
    EXPECT_DOUBLE_EQ(result.x, 10.0);
    EXPECT_DOUBLE_EQ(result.y, 20.0);
    EXPECT_DOUBLE_EQ(result.w, 100.0);
    EXPECT_DOUBLE_EQ(result.h, 200.0);
}

TEST_F(TransformMatrixTest, TransformRectWithScale) {
    TransformMatrix s = TransformMatrix::Scale(2.0, 3.0);
    Rect r(0.0, 0.0, 100.0, 200.0);
    
    Rect result = s.TransformRect(r);
    
    EXPECT_DOUBLE_EQ(result.x, 0.0);
    EXPECT_DOUBLE_EQ(result.y, 0.0);
    EXPECT_DOUBLE_EQ(result.w, 200.0);
    EXPECT_DOUBLE_EQ(result.h, 600.0);
}

TEST_F(TransformMatrixTest, IsIdentity) {
    EXPECT_TRUE(identity.IsIdentity());
    
    TransformMatrix t = TransformMatrix::Translate(1.0, 0.0);
    EXPECT_FALSE(t.IsIdentity());
}

TEST_F(TransformMatrixTest, IsInvertible) {
    EXPECT_TRUE(identity.IsInvertible());
    
    TransformMatrix singular;
    singular.m[0][0] = 0;
    singular.m[1][1] = 0;
    EXPECT_FALSE(singular.IsInvertible());
}

TEST_F(TransformMatrixTest, GetScaleX) {
    TransformMatrix s = TransformMatrix::Scale(2.0, 3.0);
    EXPECT_DOUBLE_EQ(s.GetScaleX(), 2.0);
}

TEST_F(TransformMatrixTest, GetScaleY) {
    TransformMatrix s = TransformMatrix::Scale(2.0, 3.0);
    EXPECT_DOUBLE_EQ(s.GetScaleY(), 3.0);
}

TEST_F(TransformMatrixTest, GetRotation) {
    TransformMatrix r = TransformMatrix::Rotate(1.57079632679489661923);
    EXPECT_NEAR(r.GetRotation(), 1.57079632679489661923, 1e-10);
}

TEST_F(TransformMatrixTest, GetTranslationX) {
    TransformMatrix t = TransformMatrix::Translate(10.0, 20.0);
    EXPECT_DOUBLE_EQ(t.GetTranslationX(), 10.0);
}

TEST_F(TransformMatrixTest, GetTranslationY) {
    TransformMatrix t = TransformMatrix::Translate(10.0, 20.0);
    EXPECT_DOUBLE_EQ(t.GetTranslationY(), 20.0);
}

TEST_F(TransformMatrixTest, PreTranslate) {
    TransformMatrix s = TransformMatrix::Scale(2.0, 2.0);
    s.PreTranslate(10.0, 20.0);
    
    Point pt(0.0, 0.0);
    Point result = s.TransformPoint(pt);
    
    EXPECT_DOUBLE_EQ(result.x, 10.0);
    EXPECT_DOUBLE_EQ(result.y, 20.0);
}

TEST_F(TransformMatrixTest, PostTranslate) {
    TransformMatrix s = TransformMatrix::Scale(2.0, 2.0);
    s.PostTranslate(10.0, 20.0);
    
    Point pt(0.0, 0.0);
    Point result = s.TransformPoint(pt);
    
    EXPECT_DOUBLE_EQ(result.x, 10.0);
    EXPECT_DOUBLE_EQ(result.y, 20.0);
}

TEST_F(TransformMatrixTest, Equality) {
    TransformMatrix t1 = TransformMatrix::Translate(10.0, 20.0);
    TransformMatrix t2 = TransformMatrix::Translate(10.0, 20.0);
    TransformMatrix t3 = TransformMatrix::Translate(10.0, 30.0);
    
    EXPECT_TRUE(t1 == t2);
    EXPECT_FALSE(t1 == t3);
}
