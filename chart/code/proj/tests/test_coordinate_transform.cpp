#include <gtest/gtest.h>
#include <ogc/proj/coordinate_transform.h>
#include "ogc/coordinate.h"
#include "ogc/envelope.h"
#include "ogc/point.h"
#include <cmath>

using namespace ogc::proj;

namespace {
    constexpr double EPSILON = 1e-9;
}

class CoordinateTransformTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST(CoordinateTransformTest, DefaultConstructor) {
    CoordinateTransform transform;
    EXPECT_TRUE(transform.IsIdentity());
}

TEST(CoordinateTransformTest, ConstructorWithMatrix) {
    TransformMatrix matrix = TransformMatrix::CreateTranslation(10.0, 20.0);
    CoordinateTransform transform(matrix);
    EXPECT_FALSE(transform.IsIdentity());
    
    TransformMatrix result = transform.GetMatrix();
    EXPECT_NEAR(result.GetTranslationX(), 10.0, EPSILON);
    EXPECT_NEAR(result.GetTranslationY(), 20.0, EPSILON);
}

TEST(CoordinateTransformTest, ConstructorWithOffsetAndScale) {
    CoordinateTransform transform(100.0, 200.0, 0.5, 2.0);
    
    double offsetX, offsetY;
    transform.GetOffset(offsetX, offsetY);
    EXPECT_NEAR(offsetX, 100.0, EPSILON);
    EXPECT_NEAR(offsetY, 200.0, EPSILON);
    
    double scaleX, scaleY;
    transform.GetScale(scaleX, scaleY);
    EXPECT_NEAR(scaleX, 0.5, EPSILON);
    EXPECT_NEAR(scaleY, 2.0, EPSILON);
}

TEST(CoordinateTransformTest, ConstructorWithUniformScale) {
    CoordinateTransform transform(50.0, 100.0, 2.0);
    
    double scaleX, scaleY;
    transform.GetScale(scaleX, scaleY);
    EXPECT_NEAR(scaleX, 2.0, EPSILON);
    EXPECT_NEAR(scaleY, 2.0, EPSILON);
}

TEST(CoordinateTransformTest, SetMatrix) {
    CoordinateTransform transform;
    TransformMatrix matrix = TransformMatrix::CreateScale(2.0, 3.0);
    transform.SetMatrix(matrix);
    
    EXPECT_FALSE(transform.IsIdentity());
    TransformMatrix result = transform.GetMatrix();
    EXPECT_NEAR(result.GetScaleX(), 2.0, EPSILON);
    EXPECT_NEAR(result.GetScaleY(), 3.0, EPSILON);
}

TEST(CoordinateTransformTest, SetOffset) {
    CoordinateTransform transform;
    transform.SetOffset(10.0, 20.0);
    
    double offsetX, offsetY;
    transform.GetOffset(offsetX, offsetY);
    EXPECT_NEAR(offsetX, 10.0, EPSILON);
    EXPECT_NEAR(offsetY, 20.0, EPSILON);
}

TEST(CoordinateTransformTest, SetScale) {
    CoordinateTransform transform;
    transform.SetScale(2.0, 3.0);
    
    double scaleX, scaleY;
    transform.GetScale(scaleX, scaleY);
    EXPECT_NEAR(scaleX, 2.0, EPSILON);
    EXPECT_NEAR(scaleY, 3.0, EPSILON);
}

TEST(CoordinateTransformTest, SetUniformScale) {
    CoordinateTransform transform;
    transform.SetScale(5.0);
    
    double scaleX, scaleY;
    transform.GetScale(scaleX, scaleY);
    EXPECT_NEAR(scaleX, 5.0, EPSILON);
    EXPECT_NEAR(scaleY, 5.0, EPSILON);
}

TEST(CoordinateTransformTest, SetWorldToScreen) {
    CoordinateTransform transform;
    transform.SetWorldToScreen(0.0, 0.0, 1000.0, 1000.0,
                                0.0, 0.0, 500.0, 500.0);
    
    double x = 500.0, y = 500.0;
    transform.Transform(x, y);
    EXPECT_NEAR(x, 250.0, EPSILON);
    EXPECT_NEAR(y, 250.0, EPSILON);
}

TEST(CoordinateTransformTest, TransformPoint) {
    CoordinateTransform transform(10.0, 20.0, 1.0, 1.0);
    
    double x = 5.0, y = 10.0;
    transform.Transform(x, y);
    EXPECT_NEAR(x, 15.0, EPSILON);
    EXPECT_NEAR(y, 30.0, EPSILON);
}

TEST(CoordinateTransformTest, TransformWithScale) {
    CoordinateTransform transform(0.0, 0.0, 2.0, 3.0);
    
    double x = 5.0, y = 10.0;
    transform.Transform(x, y);
    EXPECT_NEAR(x, 10.0, EPSILON);
    EXPECT_NEAR(y, 30.0, EPSILON);
}

TEST(CoordinateTransformTest, TransformInverse) {
    CoordinateTransform transform(10.0, 20.0, 2.0, 2.0);
    
    double x = 5.0, y = 10.0;
    double originalX = x, originalY = y;
    
    transform.Transform(x, y);
    transform.TransformInverse(x, y);
    
    EXPECT_NEAR(x, originalX, EPSILON);
    EXPECT_NEAR(y, originalY, EPSILON);
}

TEST(CoordinateTransformTest, TransformCoordinate) {
    CoordinateTransform transform(100.0, 200.0, 1.0, 1.0);
    
    ogc::Coordinate coord(50.0, 75.0);
    ogc::Coordinate result = transform.Transform(coord);
    
    EXPECT_NEAR(result.x, 150.0, EPSILON);
    EXPECT_NEAR(result.y, 275.0, EPSILON);
}

TEST(CoordinateTransformTest, TransformCoordinateInverse) {
    CoordinateTransform transform(100.0, 200.0, 1.0, 1.0);
    
    ogc::Coordinate coord(150.0, 275.0);
    ogc::Coordinate result = transform.TransformInverse(coord);
    
    EXPECT_NEAR(result.x, 50.0, EPSILON);
    EXPECT_NEAR(result.y, 75.0, EPSILON);
}

TEST(CoordinateTransformTest, TransformArray) {
    CoordinateTransform transform(0.0, 0.0, 2.0, 2.0);
    
    double x[] = {1.0, 2.0, 3.0};
    double y[] = {4.0, 5.0, 6.0};
    
    transform.TransformArray(x, y, 3);
    
    EXPECT_NEAR(x[0], 2.0, EPSILON);
    EXPECT_NEAR(y[0], 8.0, EPSILON);
    EXPECT_NEAR(x[1], 4.0, EPSILON);
    EXPECT_NEAR(y[1], 10.0, EPSILON);
    EXPECT_NEAR(x[2], 6.0, EPSILON);
    EXPECT_NEAR(y[2], 12.0, EPSILON);
}

TEST(CoordinateTransformTest, TransformArrayInverse) {
    CoordinateTransform transform(0.0, 0.0, 2.0, 2.0);
    
    double x[] = {2.0, 4.0, 6.0};
    double y[] = {8.0, 10.0, 12.0};
    
    transform.TransformArrayInverse(x, y, 3);
    
    EXPECT_NEAR(x[0], 1.0, EPSILON);
    EXPECT_NEAR(y[0], 4.0, EPSILON);
    EXPECT_NEAR(x[1], 2.0, EPSILON);
    EXPECT_NEAR(y[1], 5.0, EPSILON);
    EXPECT_NEAR(x[2], 3.0, EPSILON);
    EXPECT_NEAR(y[2], 6.0, EPSILON);
}

TEST(CoordinateTransformTest, TransformEnvelope) {
    CoordinateTransform transform(0.0, 0.0, 2.0, 2.0);
    
    ogc::Envelope env(0.0, 0.0, 10.0, 10.0);
    ogc::Envelope result = transform.Transform(env);
    
    EXPECT_NEAR(result.GetMinX(), 0.0, EPSILON);
    EXPECT_NEAR(result.GetMinY(), 0.0, EPSILON);
    EXPECT_NEAR(result.GetMaxX(), 20.0, EPSILON);
    EXPECT_NEAR(result.GetMaxY(), 20.0, EPSILON);
}

TEST(CoordinateTransformTest, TransformEnvelopeInverse) {
    CoordinateTransform transform(0.0, 0.0, 2.0, 2.0);
    
    ogc::Envelope env(0.0, 0.0, 20.0, 20.0);
    ogc::Envelope result = transform.TransformInverse(env);
    
    EXPECT_NEAR(result.GetMinX(), 0.0, EPSILON);
    EXPECT_NEAR(result.GetMinY(), 0.0, EPSILON);
    EXPECT_NEAR(result.GetMaxX(), 10.0, EPSILON);
    EXPECT_NEAR(result.GetMaxY(), 10.0, EPSILON);
}

TEST(CoordinateTransformTest, Reset) {
    CoordinateTransform transform(10.0, 20.0, 2.0, 3.0);
    EXPECT_FALSE(transform.IsIdentity());
    
    transform.Reset();
    EXPECT_TRUE(transform.IsIdentity());
}

TEST(CoordinateTransformTest, Clone) {
    CoordinateTransform transform(10.0, 20.0, 2.0, 3.0);
    auto clone = transform.Clone();
    
    ASSERT_NE(clone, nullptr);
    EXPECT_FALSE(clone->IsIdentity());
    
    double scaleX, scaleY;
    clone->GetScale(scaleX, scaleY);
    EXPECT_NEAR(scaleX, 2.0, EPSILON);
    EXPECT_NEAR(scaleY, 3.0, EPSILON);
}

TEST(CoordinateTransformTest, Multiply) {
    CoordinateTransform transform1(10.0, 0.0, 1.0, 1.0);
    CoordinateTransform transform2(0.0, 20.0, 1.0, 1.0);
    
    transform1.Multiply(transform2);
    
    double offsetX, offsetY;
    transform1.GetOffset(offsetX, offsetY);
    EXPECT_NEAR(offsetX, 10.0, EPSILON);
    EXPECT_NEAR(offsetY, 20.0, EPSILON);
}

TEST(CoordinateTransformTest, Create) {
    auto transform = CoordinateTransform::Create();
    ASSERT_NE(transform, nullptr);
    EXPECT_TRUE(transform->IsIdentity());
}

TEST(CoordinateTransformTest, CreateWithMatrix) {
    TransformMatrix matrix = TransformMatrix::CreateTranslation(50.0, 100.0);
    auto transform = CoordinateTransform::Create(matrix);
    ASSERT_NE(transform, nullptr);
    EXPECT_FALSE(transform->IsIdentity());
}

TEST(CoordinateTransformTest, CreateWithOffsetAndScale) {
    auto transform = CoordinateTransform::Create(10.0, 20.0, 2.0, 3.0);
    ASSERT_NE(transform, nullptr);
    
    double scaleX, scaleY;
    transform->GetScale(scaleX, scaleY);
    EXPECT_NEAR(scaleX, 2.0, EPSILON);
    EXPECT_NEAR(scaleY, 3.0, EPSILON);
}

TEST(CoordinateTransformTest, CreateWorldToScreen) {
    auto transform = CoordinateTransform::CreateWorldToScreen(
        0.0, 0.0, 1000.0, 1000.0,
        0.0, 0.0, 500.0, 500.0);
    
    ASSERT_NE(transform, nullptr);
    
    double x = 1000.0, y = 1000.0;
    transform->Transform(x, y);
    EXPECT_NEAR(x, 500.0, EPSILON);
    EXPECT_NEAR(y, 500.0, EPSILON);
}
