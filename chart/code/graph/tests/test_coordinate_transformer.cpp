#include <gtest/gtest.h>
#include "ogc/draw/coordinate_transformer.h"
#include "ogc/coordinate.h"
#include "ogc/envelope.h"
#include <memory>

using namespace ogc::draw;
using ogc::Coordinate;
using ogc::Envelope;

class CoordinateTransformerTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_transformer = CoordinateTransformer::CreateWGS84ToWebMercator();
        ASSERT_NE(m_transformer, nullptr);
    }
    
    void TearDown() override {
        m_transformer.reset();
    }
    
    CoordinateTransformerPtr m_transformer;
};

TEST_F(CoordinateTransformerTest, Create) {
    EXPECT_NE(m_transformer, nullptr);
}

TEST_F(CoordinateTransformerTest, IsValid) {
    EXPECT_TRUE(m_transformer->IsValid());
}

TEST_F(CoordinateTransformerTest, TransformCoordinate) {
    Coordinate coord(116.4, 39.9);
    auto result = m_transformer->Transform(coord);
    
    EXPECT_NEAR(result.x, 12958000.0, 1000.0);
    EXPECT_NEAR(result.y, 4850000.0, 20000.0);
}

TEST_F(CoordinateTransformerTest, TransformInverseCoordinate) {
    Coordinate coord(12957588.0, 4851421.0);
    auto result = m_transformer->TransformInverse(coord);
    
    EXPECT_NEAR(result.x, 116.4, 0.1);
    EXPECT_NEAR(result.y, 39.9, 0.2);
}

TEST_F(CoordinateTransformerTest, TransformXY) {
    double x = 116.4;
    double y = 39.9;
    
    m_transformer->Transform(x, y);
    
    EXPECT_NEAR(x, 12958000.0, 1000.0);
    EXPECT_NEAR(y, 4850000.0, 20000.0);
}

TEST_F(CoordinateTransformerTest, TransformInverseXY) {
    double x = 12957588.0;
    double y = 4851421.0;
    
    m_transformer->TransformInverse(x, y);
    
    EXPECT_NEAR(x, 116.4, 0.1);
    EXPECT_NEAR(y, 39.9, 0.2);
}

TEST_F(CoordinateTransformerTest, TransformArray) {
    double x[] = {116.4, 117.0, 118.0};
    double y[] = {39.9, 40.0, 41.0};
    
    m_transformer->TransformArray(x, y, 3);
    
    EXPECT_NEAR(x[0], 12958000.0, 1000.0);
    EXPECT_NEAR(y[0], 4850000.0, 20000.0);
}

TEST_F(CoordinateTransformerTest, TransformArrayInverse) {
    double x[] = {12957588.0, 13000000.0, 13100000.0};
    double y[] = {4851421.0, 5000000.0, 5100000.0};
    
    m_transformer->TransformArrayInverse(x, y, 3);
    
    EXPECT_NEAR(x[0], 116.4, 0.1);
    EXPECT_NEAR(y[0], 39.9, 0.2);
}

TEST_F(CoordinateTransformerTest, TransformEnvelope) {
    Envelope env(116.0, 39.0, 117.0, 40.0);
    auto result = m_transformer->Transform(env);
    
    EXPECT_GT(result.GetWidth(), 0);
    EXPECT_GT(result.GetHeight(), 0);
}

TEST_F(CoordinateTransformerTest, TransformInverseEnvelope) {
    Envelope env(12900000.0, 4800000.0, 13000000.0, 4900000.0);
    auto result = m_transformer->TransformInverse(env);
    
    EXPECT_GT(result.GetWidth(), 0);
    EXPECT_GT(result.GetHeight(), 0);
}

TEST_F(CoordinateTransformerTest, GetSourceCRS) {
    EXPECT_FALSE(m_transformer->GetSourceCRS().empty());
}

TEST_F(CoordinateTransformerTest, GetTargetCRS) {
    EXPECT_FALSE(m_transformer->GetTargetCRS().empty());
}

TEST_F(CoordinateTransformerTest, GetName) {
    EXPECT_FALSE(m_transformer->GetName().empty());
}

TEST_F(CoordinateTransformerTest, Clone) {
    auto clone = m_transformer->Clone();
    EXPECT_NE(clone, nullptr);
    EXPECT_TRUE(clone->IsValid());
}

TEST_F(CoordinateTransformerTest, CreateWithCRS) {
    auto transformer = CoordinateTransformer::Create("EPSG:4326", "EPSG:3857");
    EXPECT_NE(transformer, nullptr);
    EXPECT_TRUE(transformer->IsValid());
}
