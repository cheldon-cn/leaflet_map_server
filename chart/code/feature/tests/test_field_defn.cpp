#include <gtest/gtest.h>
#include "ogc/feature/field_defn.h"
#include "ogc/feature/field_type.h"

using namespace ogc;

class FieldDefnTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FieldDefnTest, DefaultConstructor) {
    CNFieldDefn* defn = CreateCNFieldDefn();
    EXPECT_NE(defn, nullptr);
    delete defn;
}

TEST_F(FieldDefnTest, NamedConstructor) {
    CNFieldDefn* defn = CreateCNFieldDefn("test_field");
    EXPECT_NE(defn, nullptr);
    EXPECT_STREQ(defn->GetName(), "test_field");
    delete defn;
}

TEST_F(FieldDefnTest, SetName) {
    CNFieldDefn* defn = CreateCNFieldDefn();
    defn->SetName("field_name");
    EXPECT_STREQ(defn->GetName(), "field_name");
    delete defn;
}

TEST_F(FieldDefnTest, SetType) {
    CNFieldDefn* defn = CreateCNFieldDefn();
    
    defn->SetType(CNFieldType::kInteger);
    EXPECT_EQ(defn->GetType(), CNFieldType::kInteger);
    
    defn->SetType(CNFieldType::kReal);
    EXPECT_EQ(defn->GetType(), CNFieldType::kReal);
    
    defn->SetType(CNFieldType::kString);
    EXPECT_EQ(defn->GetType(), CNFieldType::kString);
    
    delete defn;
}

TEST_F(FieldDefnTest, SetWidth) {
    CNFieldDefn* defn = CreateCNFieldDefn();
    
    defn->SetWidth(10);
    EXPECT_EQ(defn->GetWidth(), 10);
    
    defn->SetWidth(255);
    EXPECT_EQ(defn->GetWidth(), 255);
    
    delete defn;
}

TEST_F(FieldDefnTest, SetPrecision) {
    CNFieldDefn* defn = CreateCNFieldDefn();
    
    defn->SetPrecision(2);
    EXPECT_EQ(defn->GetPrecision(), 2);
    
    defn->SetPrecision(6);
    EXPECT_EQ(defn->GetPrecision(), 6);
    
    delete defn;
}

TEST_F(FieldDefnTest, SetSubType) {
    CNFieldDefn* defn = CreateCNFieldDefn();
    
    defn->SetSubType(CNFieldSubType::kBoolean);
    EXPECT_EQ(defn->GetSubType(), CNFieldSubType::kBoolean);
    
    defn->SetSubType(CNFieldSubType::kInt16);
    EXPECT_EQ(defn->GetSubType(), CNFieldSubType::kInt16);
    
    delete defn;
}

TEST_F(FieldDefnTest, SetNullable) {
    CNFieldDefn* defn = CreateCNFieldDefn();
    
    defn->SetNullable(true);
    EXPECT_TRUE(defn->IsNullable());
    
    defn->SetNullable(false);
    EXPECT_FALSE(defn->IsNullable());
    
    delete defn;
}

TEST_F(FieldDefnTest, SetDefaultValue) {
    CNFieldDefn* defn = CreateCNFieldDefn();
    defn->SetType(CNFieldType::kInteger);
    
    defn->SetDefaultValue("42");
    EXPECT_STREQ(defn->GetDefaultValue(), "42");
    
    delete defn;
}

TEST_F(FieldDefnTest, Clone) {
    CNFieldDefn* original = CreateCNFieldDefn("original");
    original->SetType(CNFieldType::kReal);
    original->SetWidth(10);
    original->SetPrecision(4);
    
    CNFieldDefn* cloned = original->Clone();
    EXPECT_NE(cloned, nullptr);
    EXPECT_STREQ(cloned->GetName(), "original");
    EXPECT_EQ(cloned->GetType(), CNFieldType::kReal);
    EXPECT_EQ(cloned->GetWidth(), 10);
    EXPECT_EQ(cloned->GetPrecision(), 4);
    
    delete cloned;
    delete original;
}

TEST_F(FieldDefnTest, AllFieldTypes) {
    CNFieldDefn* defn = CreateCNFieldDefn();
    
    std::vector<CNFieldType> types = {
        CNFieldType::kInteger,
        CNFieldType::kInteger64,
        CNFieldType::kReal,
        CNFieldType::kString,
        CNFieldType::kDate,
        CNFieldType::kTime,
        CNFieldType::kDateTime,
        CNFieldType::kBinary,
        CNFieldType::kNull
    };
    
    for (auto type : types) {
        defn->SetType(type);
        EXPECT_EQ(defn->GetType(), type);
    }
    
    delete defn;
}

TEST_F(FieldDefnTest, AllSubTypes) {
    CNFieldDefn* defn = CreateCNFieldDefn();
    
    std::vector<CNFieldSubType> subTypes = {
        CNFieldSubType::kNone,
        CNFieldSubType::kBoolean,
        CNFieldSubType::kInt16,
        CNFieldSubType::kFloat32
    };
    
    for (auto subType : subTypes) {
        defn->SetSubType(subType);
        EXPECT_EQ(defn->GetSubType(), subType);
    }
    
    delete defn;
}

TEST_F(FieldDefnTest, EmptyName) {
    CNFieldDefn* defn = CreateCNFieldDefn();
    defn->SetName("");
    EXPECT_STREQ(defn->GetName(), "");
    delete defn;
}

TEST_F(FieldDefnTest, LongName) {
    CNFieldDefn* defn = CreateCNFieldDefn();
    std::string longName(1000, 'a');
    defn->SetName(longName.c_str());
    EXPECT_STREQ(defn->GetName(), longName.c_str());
    delete defn;
}

TEST_F(FieldDefnTest, ZeroWidthAndPrecision) {
    CNFieldDefn* defn = CreateCNFieldDefn();
    defn->SetWidth(0);
    defn->SetPrecision(0);
    
    EXPECT_EQ(defn->GetWidth(), 0);
    EXPECT_EQ(defn->GetPrecision(), 0);
    
    delete defn;
}

TEST_F(FieldDefnTest, SetUnique) {
    CNFieldDefn* defn = CreateCNFieldDefn();
    
    defn->SetUnique(true);
    EXPECT_TRUE(defn->IsUnique());
    
    defn->SetUnique(false);
    EXPECT_FALSE(defn->IsUnique());
    
    delete defn;
}

TEST_F(FieldDefnTest, SetAlternativeName) {
    CNFieldDefn* defn = CreateCNFieldDefn();
    defn->SetAlternativeName("alt_name");
    EXPECT_STREQ(defn->GetAlternativeName(), "alt_name");
    delete defn;
}

TEST_F(FieldDefnTest, SetComment) {
    CNFieldDefn* defn = CreateCNFieldDefn();
    defn->SetComment("This is a comment");
    EXPECT_STREQ(defn->GetComment(), "This is a comment");
    delete defn;
}

TEST_F(FieldDefnTest, SetDomainName) {
    CNFieldDefn* defn = CreateCNFieldDefn();
    defn->SetDomainName("domain_name");
    EXPECT_STREQ(defn->GetDomainName(), "domain_name");
    delete defn;
}
