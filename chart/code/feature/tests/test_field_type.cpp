#include "gtest/gtest.h"
#include "ogc/feature/field_type.h"

using namespace ogc;

TEST(CNFieldType, Values) {
    EXPECT_EQ(static_cast<int>(CNFieldType::kInteger), 0);
    EXPECT_EQ(static_cast<int>(CNFieldType::kInteger64), 1);
    EXPECT_EQ(static_cast<int>(CNFieldType::kReal), 2);
    EXPECT_EQ(static_cast<int>(CNFieldType::kString), 3);
    EXPECT_EQ(static_cast<int>(CNFieldType::kDate), 4);
    EXPECT_EQ(static_cast<int>(CNFieldType::kTime), 5);
    EXPECT_EQ(static_cast<int>(CNFieldType::kDateTime), 6);
    EXPECT_EQ(static_cast<int>(CNFieldType::kBinary), 7);
    EXPECT_EQ(static_cast<int>(CNFieldType::kNull), 100);
}

TEST(CNFieldType, SubTypeValues) {
    EXPECT_EQ(static_cast<int>(CNFieldSubType::kNone), 0);
    EXPECT_EQ(static_cast<int>(CNFieldSubType::kBoolean), 1);
    EXPECT_EQ(static_cast<int>(CNFieldSubType::kInt16), 2);
    EXPECT_EQ(static_cast<int>(CNFieldSubType::kFloat32), 3);
}
