#include <gtest/gtest.h>
#include "parser/s57_attribute_parser.h"
#include <ogr_feature.h>

using namespace chart::parser;

class S57AttributeParserTest : public ::testing::Test {
protected:
    void SetUp() override {
        parser_ = new S57AttributeParser();
    }
    
    void TearDown() override {
        delete parser_;
    }
    
    S57AttributeParser* parser_;
};

TEST_F(S57AttributeParserTest, GetStringValueFromEmptyMap) {
    AttributeMap attrs;
    
    std::string result = parser_->GetStringValue(attrs, "nonexistent", "default");
    
    EXPECT_EQ(result, "default");
}

TEST_F(S57AttributeParserTest, GetIntValueFromEmptyMap) {
    AttributeMap attrs;
    
    int result = parser_->GetIntValue(attrs, "nonexistent", 42);
    
    EXPECT_EQ(result, 42);
}

TEST_F(S57AttributeParserTest, GetDoubleValueFromEmptyMap) {
    AttributeMap attrs;
    
    double result = parser_->GetDoubleValue(attrs, "nonexistent", 3.14);
    
    EXPECT_DOUBLE_EQ(result, 3.14);
}

TEST_F(S57AttributeParserTest, GetStringValue) {
    AttributeMap attrs;
    AttributeValue value;
    value.type = AttributeValue::Type::String;
    value.stringValue = "test_value";
    attrs["test_key"] = value;
    
    std::string result = parser_->GetStringValue(attrs, "test_key", "default");
    
    EXPECT_EQ(result, "test_value");
}

TEST_F(S57AttributeParserTest, GetIntValue) {
    AttributeMap attrs;
    AttributeValue value;
    value.type = AttributeValue::Type::Integer;
    value.intValue = 123;
    attrs["test_key"] = value;
    
    int result = parser_->GetIntValue(attrs, "test_key", 0);
    
    EXPECT_EQ(result, 123);
}

TEST_F(S57AttributeParserTest, GetDoubleValue) {
    AttributeMap attrs;
    AttributeValue value;
    value.type = AttributeValue::Type::Double;
    value.doubleValue = 3.14159;
    attrs["test_key"] = value;
    
    double result = parser_->GetDoubleValue(attrs, "test_key", 0.0);
    
    EXPECT_DOUBLE_EQ(result, 3.14159);
}

TEST_F(S57AttributeParserTest, GetIntValueFromDouble) {
    AttributeMap attrs;
    AttributeValue value;
    value.type = AttributeValue::Type::Double;
    value.doubleValue = 42.7;
    attrs["test_key"] = value;
    
    int result = parser_->GetIntValue(attrs, "test_key", 0);
    
    EXPECT_EQ(result, 42);
}

TEST_F(S57AttributeParserTest, GetDoubleValueFromInt) {
    AttributeMap attrs;
    AttributeValue value;
    value.type = AttributeValue::Type::Integer;
    value.intValue = 42;
    attrs["test_key"] = value;
    
    double result = parser_->GetDoubleValue(attrs, "test_key", 0.0);
    
    EXPECT_DOUBLE_EQ(result, 42.0);
}

TEST_F(S57AttributeParserTest, GetIntValueFromString) {
    AttributeMap attrs;
    AttributeValue value;
    value.type = AttributeValue::Type::String;
    value.stringValue = "123";
    attrs["test_key"] = value;
    
    int result = parser_->GetIntValue(attrs, "test_key", 0);
    
    EXPECT_EQ(result, 123);
}

TEST_F(S57AttributeParserTest, GetDoubleValueFromString) {
    AttributeMap attrs;
    AttributeValue value;
    value.type = AttributeValue::Type::String;
    value.stringValue = "3.14";
    attrs["test_key"] = value;
    
    double result = parser_->GetDoubleValue(attrs, "test_key", 0.0);
    
    EXPECT_DOUBLE_EQ(result, 3.14);
}

TEST_F(S57AttributeParserTest, GetIntValueFromInvalidString) {
    AttributeMap attrs;
    AttributeValue value;
    value.type = AttributeValue::Type::String;
    value.stringValue = "not_a_number";
    attrs["test_key"] = value;
    
    int result = parser_->GetIntValue(attrs, "test_key", 42);
    
    EXPECT_EQ(result, 42);
}
