#include <gtest/gtest.h>
#include "ogc/geom/precision.h"
#include "ogc/geom/coordinate.h"
#include "ogc/geom/common.h"

namespace ogc {
namespace test {

class PrecisionModelTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(PrecisionModelTest, DefaultConstructor_CreatesFloatingPrecision) {
    PrecisionModel model;
    
    EXPECT_EQ(model.GetType(), PrecisionType::Floating);
    EXPECT_TRUE(model.IsFloating());
    EXPECT_FALSE(model.IsFixed());
}

TEST_F(PrecisionModelTest, Constructor_Type_CreatesCorrectPrecision) {
    PrecisionModel floating(PrecisionType::Floating);
    EXPECT_EQ(floating.GetType(), PrecisionType::Floating);
    EXPECT_TRUE(floating.IsFloating());
    
    PrecisionModel single(PrecisionType::FloatingSingle);
    EXPECT_EQ(single.GetType(), PrecisionType::FloatingSingle);
    
    PrecisionModel fixed(PrecisionType::Fixed);
    EXPECT_EQ(fixed.GetType(), PrecisionType::Fixed);
}

TEST_F(PrecisionModelTest, Constructor_Scale_CreatesFixedPrecision) {
    PrecisionModel model(100.0);
    
    EXPECT_EQ(model.GetType(), PrecisionType::Fixed);
    EXPECT_TRUE(model.IsFixed());
    EXPECT_DOUBLE_EQ(model.GetScale(), 100.0);
}

TEST_F(PrecisionModelTest, Constructor_NegativeScale_UsesAbsoluteValue) {
    PrecisionModel model(-100.0);
    
    EXPECT_EQ(model.GetType(), PrecisionType::Fixed);
    EXPECT_DOUBLE_EQ(model.GetScale(), 100.0);
}

TEST_F(PrecisionModelTest, MakePrecise_Floating_ReturnsSameValue) {
    PrecisionModel model(PrecisionType::Floating);
    
    double value = 1.23456789;
    double precise = model.MakePrecise(value);
    
    EXPECT_DOUBLE_EQ(precise, value);
}

TEST_F(PrecisionModelTest, MakePrecise_Fixed_RoundsToScale) {
    PrecisionModel model(100.0);
    
    EXPECT_DOUBLE_EQ(model.MakePrecise(1.234), 1.23);
    EXPECT_DOUBLE_EQ(model.MakePrecise(1.235), 1.24);
    EXPECT_DOUBLE_EQ(model.MakePrecise(1.236), 1.24);
}

TEST_F(PrecisionModelTest, MakePrecise_Fixed_Scale10) {
    PrecisionModel model(10.0);
    
    EXPECT_DOUBLE_EQ(model.MakePrecise(1.23), 1.2);
    EXPECT_DOUBLE_EQ(model.MakePrecise(1.25), 1.3);
    EXPECT_DOUBLE_EQ(model.MakePrecise(1.28), 1.3);
}

TEST_F(PrecisionModelTest, MakePrecise_Fixed_Scale1) {
    PrecisionModel model(1.0);
    
    EXPECT_DOUBLE_EQ(model.MakePrecise(1.2), 1.0);
    EXPECT_DOUBLE_EQ(model.MakePrecise(1.5), 2.0);
    EXPECT_DOUBLE_EQ(model.MakePrecise(1.8), 2.0);
}

TEST_F(PrecisionModelTest, MakePrecise_Coordinate_ReturnsPreciseCoordinate) {
    PrecisionModel model(100.0);
    
    Coordinate coord(1.234, 5.678);
    Coordinate precise = model.MakePrecise(coord);
    
    EXPECT_DOUBLE_EQ(precise.x, 1.23);
    EXPECT_DOUBLE_EQ(precise.y, 5.68);
}

TEST_F(PrecisionModelTest, MakePrecise_Coordinate3D_ReturnsPreciseCoordinate) {
    PrecisionModel model(100.0);
    
    Coordinate coord(1.234, 5.678, 9.012);
    Coordinate precise = model.MakePrecise(coord);
    
    EXPECT_DOUBLE_EQ(precise.x, 1.23);
    EXPECT_DOUBLE_EQ(precise.y, 5.68);
    EXPECT_DOUBLE_EQ(precise.z, 9.01);
}

TEST_F(PrecisionModelTest, MakePrecise_EmptyCoordinate_ReturnsEmpty) {
    PrecisionModel model(100.0);
    
    Coordinate empty = Coordinate::Empty();
    Coordinate precise = model.MakePrecise(empty);
    
    EXPECT_TRUE(precise.IsEmpty());
}

TEST_F(PrecisionModelTest, Equals_SameType_ReturnsTrue) {
    PrecisionModel model1(PrecisionType::Floating);
    PrecisionModel model2(PrecisionType::Floating);
    
    EXPECT_TRUE(model1.Equals(model2));
}

TEST_F(PrecisionModelTest, Equals_DifferentType_ReturnsFalse) {
    PrecisionModel model1(PrecisionType::Floating);
    PrecisionModel model2(PrecisionType::Fixed);
    
    EXPECT_FALSE(model1.Equals(model2));
}

TEST_F(PrecisionModelTest, Equals_SameScale_ReturnsTrue) {
    PrecisionModel model1(100.0);
    PrecisionModel model2(100.0);
    
    EXPECT_TRUE(model1.Equals(model2));
}

TEST_F(PrecisionModelTest, Equals_DifferentScale_ReturnsFalse) {
    PrecisionModel model1(100.0);
    PrecisionModel model2(200.0);
    
    EXPECT_FALSE(model1.Equals(model2));
}

TEST_F(PrecisionModelTest, Compare_LessThan_ReturnsNegative) {
    PrecisionModel model(100.0);
    
    EXPECT_LT(model.Compare(1.0, 2.0), 0);
}

TEST_F(PrecisionModelTest, Compare_GreaterThan_ReturnsPositive) {
    PrecisionModel model(100.0);
    
    EXPECT_GT(model.Compare(2.0, 1.0), 0);
}

TEST_F(PrecisionModelTest, Compare_Equal_ReturnsZero) {
    PrecisionModel model(100.0);
    
    EXPECT_EQ(model.Compare(1.0, 1.0), 0);
}

TEST_F(PrecisionModelTest, Compare_WithinTolerance_ReturnsZero) {
    PrecisionModel model(100.0);
    
    EXPECT_EQ(model.Compare(1.0, 1.0 + DEFAULT_TOLERANCE * 0.5), 0);
}

TEST_F(PrecisionModelTest, Static_Floating_ReturnsFloatingPrecision) {
    PrecisionModel model = PrecisionModel::Floating();
    
    EXPECT_EQ(model.GetType(), PrecisionType::Floating);
    EXPECT_TRUE(model.IsFloating());
}

TEST_F(PrecisionModelTest, Static_FloatingSingle_ReturnsSinglePrecision) {
    PrecisionModel model = PrecisionModel::FloatingSingle();
    
    EXPECT_EQ(model.GetType(), PrecisionType::FloatingSingle);
}

TEST_F(PrecisionModelTest, Static_Fixed_ReturnsFixedPrecision) {
    PrecisionModel model = PrecisionModel::Fixed(100.0);
    
    EXPECT_EQ(model.GetType(), PrecisionType::Fixed);
    EXPECT_DOUBLE_EQ(model.GetScale(), 100.0);
}

TEST_F(PrecisionModelTest, GetScale_Floating_ReturnsZero) {
    PrecisionModel model(PrecisionType::Floating);
    
    EXPECT_DOUBLE_EQ(model.GetScale(), 0.0);
}

TEST_F(PrecisionModelTest, GetScale_Fixed_ReturnsScale) {
    PrecisionModel model(1000.0);
    
    EXPECT_DOUBLE_EQ(model.GetScale(), 1000.0);
}

TEST_F(PrecisionModelTest, MakePrecise_ZeroScale_ReturnsSameValue) {
    PrecisionModel model(PrecisionType::Floating);
    
    double value = 1.23456789;
    double precise = model.MakePrecise(value);
    
    EXPECT_DOUBLE_EQ(precise, value);
}

TEST_F(PrecisionModelTest, MakePrecise_NegativeValue_HandlesCorrectly) {
    PrecisionModel model(100.0);
    
    EXPECT_DOUBLE_EQ(model.MakePrecise(-1.234), -1.23);
    EXPECT_DOUBLE_EQ(model.MakePrecise(-1.236), -1.24);
}

TEST_F(PrecisionModelTest, MakePrecise_VerySmallValue_HandlesCorrectly) {
    PrecisionModel model(100.0);
    
    EXPECT_NEAR(model.MakePrecise(0.001), 0.0, DEFAULT_TOLERANCE);
    EXPECT_NEAR(model.MakePrecise(0.005), 0.01, DEFAULT_TOLERANCE);
}

TEST_F(PrecisionModelTest, MakePrecise_VeryLargeValue_HandlesCorrectly) {
    PrecisionModel model(100.0);
    
    EXPECT_DOUBLE_EQ(model.MakePrecise(12345.678), 12345.68);
}

TEST_F(PrecisionModelTest, ChainOperations_WorkCorrectly) {
    PrecisionModel model(10.0);
    
    Coordinate coord(1.23, 4.56, 7.89);
    Coordinate precise = model.MakePrecise(coord);
    
    EXPECT_DOUBLE_EQ(precise.x, 1.2);
    EXPECT_DOUBLE_EQ(precise.y, 4.6);
    EXPECT_DOUBLE_EQ(precise.z, 7.9);
}

}
}
