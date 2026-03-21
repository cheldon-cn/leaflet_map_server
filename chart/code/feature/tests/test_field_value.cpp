#include "gtest/gtest.h"
#include "ogc/feature/field_value.h"
#include "ogc/feature/datetime.h"
#include <string>
#include <vector>

using namespace ogc;

TEST(CNFieldValue, DefaultConstructor) {
    CNFieldValue value;
    EXPECT_EQ(value.GetType(), CNFieldType::kUnset);
    EXPECT_TRUE(value.IsUnset());
}

TEST(CNFieldValue, IntegerConstructor) {
    CNFieldValue value(42);
    EXPECT_EQ(value.GetType(), CNFieldType::kInteger);
    EXPECT_EQ(value.GetInteger(), 42);
    EXPECT_FALSE(value.IsNull());
}

TEST(CNFieldValue, Integer64Constructor) {
    CNFieldValue value(static_cast<int64_t>(1234567890123));
    EXPECT_EQ(value.GetType(), CNFieldType::kInteger64);
    EXPECT_EQ(value.GetInteger64(), 1234567890123);
}

TEST(CNFieldValue, RealConstructor) {
    CNFieldValue value(3.14159);
    EXPECT_EQ(value.GetType(), CNFieldType::kReal);
    EXPECT_DOUBLE_EQ(value.GetReal(), 3.14159);
}

TEST(CNFieldValue, StringConstructor) {
    CNFieldValue value(std::string("test string"));
    EXPECT_EQ(value.GetType(), CNFieldType::kString);
    EXPECT_EQ(value.GetString(), "test string");
}

TEST(CNFieldValue, BinaryConstructor) {
    std::vector<uint8_t> data = {0x01, 0x02, 0x03, 0x04};
    CNFieldValue value(data);
    EXPECT_EQ(value.GetType(), CNFieldType::kBinary);
    const std::vector<uint8_t>& result = value.GetBinary();
    EXPECT_EQ(result.size(), 4);
    EXPECT_EQ(result[0], 0x01);
}

TEST(CNFieldValue, DateTimeConstructor) {
    CNDateTime dt(2024, 3, 15, 10, 30, 0);
    CNFieldValue value(dt);
    EXPECT_EQ(value.GetType(), CNFieldType::kDateTime);
    CNDateTime result = value.GetDateTime();
    EXPECT_EQ(result.year, 2024);
    EXPECT_EQ(result.month, 3);
}

TEST(CNFieldValue, SetInteger) {
    CNFieldValue value;
    value.SetInteger(100);
    EXPECT_EQ(value.GetType(), CNFieldType::kInteger);
    EXPECT_EQ(value.GetInteger(), 100);
}

TEST(CNFieldValue, SetInteger64) {
    CNFieldValue value;
    value.SetInteger64(9876543210);
    EXPECT_EQ(value.GetType(), CNFieldType::kInteger64);
    EXPECT_EQ(value.GetInteger64(), 9876543210);
}

TEST(CNFieldValue, SetReal) {
    CNFieldValue value;
    value.SetReal(2.71828);
    EXPECT_EQ(value.GetType(), CNFieldType::kReal);
    EXPECT_DOUBLE_EQ(value.GetReal(), 2.71828);
}

TEST(CNFieldValue, SetString) {
    CNFieldValue value;
    value.SetString("hello");
    EXPECT_EQ(value.GetType(), CNFieldType::kString);
    EXPECT_EQ(value.GetString(), "hello");
}

TEST(CNFieldValue, SetBinary) {
    CNFieldValue value;
    std::vector<uint8_t> data = {0xAA, 0xBB, 0xCC};
    value.SetBinary(data);
    EXPECT_EQ(value.GetType(), CNFieldType::kBinary);
    const std::vector<uint8_t>& result = value.GetBinary();
    EXPECT_EQ(result.size(), 3);
}

TEST(CNFieldValue, SetDateTime) {
    CNFieldValue value;
    CNDateTime dt(2024, 6, 1, 12, 0, 0);
    value.SetDateTime(dt);
    EXPECT_EQ(value.GetType(), CNFieldType::kDateTime);
    CNDateTime result = value.GetDateTime();
    EXPECT_EQ(result.year, 2024);
}

TEST(CNFieldValue, SetNull) {
    CNFieldValue value(42);
    EXPECT_FALSE(value.IsNull());
    value.SetNull();
    EXPECT_TRUE(value.IsNull());
    EXPECT_EQ(value.GetType(), CNFieldType::kNull);
}

TEST(CNFieldValue, CopyConstructor) {
    CNFieldValue value1(123);
    CNFieldValue value2(value1);
    EXPECT_EQ(value2.GetInteger(), 123);
    EXPECT_EQ(value2.GetType(), CNFieldType::kInteger);
}

TEST(CNFieldValue, MoveConstructor) {
    CNFieldValue value1(std::string("move test"));
    CNFieldValue value2(std::move(value1));
    EXPECT_EQ(value2.GetString(), "move test");
}

TEST(CNFieldValue, CopyAssignment) {
    CNFieldValue value1(456);
    CNFieldValue value2;
    value2 = value1;
    EXPECT_EQ(value2.GetInteger(), 456);
}

TEST(CNFieldValue, MoveAssignment) {
    CNFieldValue value1(std::string("move assign"));
    CNFieldValue value2;
    value2 = std::move(value1);
    EXPECT_EQ(value2.GetString(), "move assign");
}

TEST(CNFieldValue, Swap) {
    CNFieldValue value1(100);
    CNFieldValue value2(200);
    value1.Swap(value2);
    EXPECT_EQ(value1.GetInteger(), 200);
    EXPECT_EQ(value2.GetInteger(), 100);
}
