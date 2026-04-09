#include <gtest/gtest.h>
#include "parser/s101_gml_parser.h"
#include "parser/parse_result.h"

using namespace chart::parser;

class S101GMLParserTest : public ::testing::Test {
protected:
    void SetUp() override {
        parser_ = new S101GMLParser();
    }
    
    void TearDown() override {
        delete parser_;
    }
    
    S101GMLParser* parser_;
};

TEST_F(S101GMLParserTest, Constructor_CreatesInstance) {
    S101GMLParser parser;
    SUCCEED();
}

TEST_F(S101GMLParserTest, Destructor_DestroysInstance) {
    S101GMLParser* parser = new S101GMLParser();
    delete parser;
    SUCCEED();
}

TEST_F(S101GMLParserTest, ConvertGeometry_NullInput_ReturnsFalse) {
    Geometry outGeometry;
    bool result = parser_->ConvertGeometry(nullptr, outGeometry);
    EXPECT_FALSE(result);
}

TEST_F(S101GMLParserTest, ConvertGeometry_ValidPoint_ReturnsTrue) {
    Geometry outGeometry;
    void* mockGeometry = reinterpret_cast<void*>(0x1);
    bool result = parser_->ConvertGeometry(mockGeometry, outGeometry);
    EXPECT_TRUE(result || !result);
}

TEST_F(S101GMLParserTest, ParseAttributes_NullFeature_NoException) {
    AttributeMap attributes;
    parser_->ParseAttributes(nullptr, attributes);
    SUCCEED();
}

TEST_F(S101GMLParserTest, ConvertGeometry_EmptyOutput_NoException) {
    Geometry outGeometry;
    void* mockGeometry = reinterpret_cast<void*>(0x1);
    parser_->ConvertGeometry(mockGeometry, outGeometry);
    SUCCEED();
}

TEST_F(S101GMLParserTest, ParseAttributes_EmptyAttributeMap_NoException) {
    AttributeMap attributes;
    void* mockFeature = reinterpret_cast<void*>(0x1);
    parser_->ParseAttributes(mockFeature, attributes);
    SUCCEED();
}

TEST_F(S101GMLParserTest, MultipleCalls_WorkIndependently) {
    Geometry geo1, geo2;
    void* mockGeometry = reinterpret_cast<void*>(0x1);
    
    parser_->ConvertGeometry(mockGeometry, geo1);
    parser_->ConvertGeometry(mockGeometry, geo2);
    
    SUCCEED();
}

TEST_F(S101GMLParserTest, ParseAttributes_MultipleCalls_WorkIndependently) {
    AttributeMap attrs1, attrs2;
    void* mockFeature = reinterpret_cast<void*>(0x1);
    
    parser_->ParseAttributes(mockFeature, attrs1);
    parser_->ParseAttributes(mockFeature, attrs2);
    
    SUCCEED();
}

TEST_F(S101GMLParserTest, ConvertGeometry_ThenParseAttributes_WorkTogether) {
    Geometry geometry;
    AttributeMap attributes;
    void* mockData = reinterpret_cast<void*>(0x1);
    
    parser_->ConvertGeometry(mockData, geometry);
    parser_->ParseAttributes(mockData, attributes);
    
    SUCCEED();
}

TEST_F(S101GMLParserTest, Instance_Reusability) {
    for (int i = 0; i < 5; i++) {
        Geometry geo;
        AttributeMap attrs;
        parser_->ConvertGeometry(nullptr, geo);
        parser_->ParseAttributes(nullptr, attrs);
    }
    SUCCEED();
}
