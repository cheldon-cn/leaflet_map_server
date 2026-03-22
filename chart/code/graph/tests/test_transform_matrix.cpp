#include <gtest/gtest.h>
#include "ogc/draw/transform_matrix.h"
#include <cmath>

TEST(TransformMatrixTest, DefaultConstructor) {
    ogc::draw::TransformMatrix matrix;
    EXPECT_DOUBLE_EQ(matrix.Get(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(matrix.Get(0, 1), 0.0);
    EXPECT_DOUBLE_EQ(matrix.Get(0, 2), 0.0);
    EXPECT_DOUBLE_EQ(matrix.Get(1, 0), 0.0);
    EXPECT_DOUBLE_EQ(matrix.Get(1, 1), 1.0);
    EXPECT_DOUBLE_EQ(matrix.Get(1, 2), 0.0);
}

TEST(TransformMatrixTest, Identity) {
    ogc::draw::TransformMatrix matrix;
    matrix.Translate(10.0, 20.0);
    matrix.Reset();
    EXPECT_DOUBLE_EQ(matrix.Get(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(matrix.Get(1, 1), 1.0);
    EXPECT_DOUBLE_EQ(matrix.Get(0, 2), 0.0);
    EXPECT_DOUBLE_EQ(matrix.Get(1, 2), 0.0);
}

TEST(TransformMatrixTest, Translate) {
    ogc::draw::TransformMatrix matrix;
    matrix.Translate(10.0, 20.0);
    EXPECT_DOUBLE_EQ(matrix.Get(0, 2), 10.0);
    EXPECT_DOUBLE_EQ(matrix.Get(1, 2), 20.0);
}

TEST(TransformMatrixTest, Scale) {
    ogc::draw::TransformMatrix matrix;
    matrix.Scale(2.0, 3.0);
    EXPECT_DOUBLE_EQ(matrix.Get(0, 0), 2.0);
    EXPECT_DOUBLE_EQ(matrix.Get(1, 1), 3.0);
}

TEST(TransformMatrixTest, Rotate) {
    ogc::draw::TransformMatrix matrix;
    const double pi = 3.14159265358979323846;
    matrix.Rotate(pi / 2.0);
    EXPECT_NEAR(matrix.Get(0, 0), 0.0, 1e-10);
    EXPECT_NEAR(matrix.Get(0, 1), 1.0, 1e-10);
    EXPECT_NEAR(matrix.Get(1, 0), -1.0, 1e-10);
    EXPECT_NEAR(matrix.Get(1, 1), 0.0, 1e-10);
}

TEST(TransformMatrixTest, TransformPoint) {
    ogc::draw::TransformMatrix matrix;
    matrix.Translate(10.0, 20.0);
    double x = 5.0, y = 5.0;
    matrix.Transform(x, y, x, y);
    EXPECT_DOUBLE_EQ(x, 15.0);
    EXPECT_DOUBLE_EQ(y, 25.0);
}

TEST(TransformMatrixTest, Multiply) {
    ogc::draw::TransformMatrix m1;
    m1.Translate(10.0, 20.0);
    
    ogc::draw::TransformMatrix m2;
    m2.Scale(2.0, 2.0);
    
    ogc::draw::TransformMatrix result = m1.Multiply(m2);
    
    double x = 5.0, y = 5.0;
    result.Transform(x, y, x, y);
    EXPECT_DOUBLE_EQ(x, 20.0);
    EXPECT_DOUBLE_EQ(y, 30.0);
}
