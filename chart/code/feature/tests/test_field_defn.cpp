#include "gtest/gtest.h"
#include "ogc/feature/field_defn.h"
#include "ogc/feature/field_type.h"

using namespace ogc;

class FieldDefnTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FieldDefnTest, DefaultConstructor) {
    CNFieldDefn defn;
    EXPECT_EQ(defn.GetName(), "");
    EXPECT_EQ(defn.GetType(), CNFieldType::kUnset);
    EXPECT_EQ(defn.GetWidth(), 0);
    EXPECT_EQ(defn.GetPrecision(), 0);
}

TEST_F(FieldDefnTest, NamedConstructor) {
    CNFieldDefn* defn = CNFieldDefn::Create("test_field");
    EXPECT_NE(defn, nullptr);
    EXPECT_EQ(defn->GetName(), "test_field");
    defn->ReleaseReference();
}

TEST_F(FieldDefnTest, NamedAndTypedConstructor) {
    CNFieldDefn* defn = CNFieldDefn::Create("id", CNFieldType::kInteger);
    EXPECT_NE(defn, nullptr);
    EXPECT_EQ(defn->GetName(), "id");
    EXPECT_EQ(defn->GetType(), CNFieldType::kInteger);
    defn->ReleaseReference();
}

TEST_F(FieldDefnTest, SetName) {
    CNFieldDefn defn;
    defn.SetName("field_name");
    EXPECT_EQ(defn.GetName(), "field_name");
}

TEST_F(FieldDefnTest, SetType) {
    CNFieldDefn defn;
    
    defn.SetType(CNFieldType::kInteger);
    EXPECT_EQ(defn.GetType(), CNFieldType::kInteger);
    
    defn.SetType(CNFieldType::kReal);
    EXPECT_EQ(defn.GetType(), CNFieldType::kReal);
    
    defn.SetType(CNFieldType::kString);
    EXPECT_EQ(defn.GetType(), CNFieldType::kString);
}

TEST_F(FieldDefnTest, SetWidth) {
    CNFieldDefn defn;
    
    defn.SetWidth(10);
    EXPECT_EQ(defn.GetWidth(), 10);
    
    defn.SetWidth(255);
    EXPECT_EQ(defn.GetWidth(), 255);
}

TEST_F(FieldDefnTest, SetPrecision) {
    CNFieldDefn defn;
    
    defn.SetPrecision(2);
    EXPECT_EQ(defn.GetPrecision(), 2);
    
    defn.SetPrecision(6);
    EXPECT_EQ(defn.GetPrecision(), 6);
}

TEST_F(FieldDefnTest, SetSubType) {
    CNFieldDefn defn;
    
    defn.SetSubType(CNFieldSubType::kBoolean);
    EXPECT_EQ(defn.GetSubType(), CNFieldSubType::kBoolean);
    
    defn.SetSubType(CNFieldSubType::kInt16);
    EXPECT_EQ(defn.GetSubType(), CNFieldSubType::kInt16);
}

TEST_F(FieldDefnTest, SetNullable) {
    CNFieldDefn defn;
    
    defn.SetNullable(true);
    EXPECT_TRUE(defn.IsNullable());
    
    defn.SetNullable(false);
    EXPECT_FALSE(defn.IsNullable());
}

TEST_F(FieldDefnTest, SetDefault) {
    CNFieldDefn defn;
    defn.SetType(CNFieldType::kInteger);
    
    defn.SetDefault("42");
    EXPECT_EQ(defn.GetDefault(), "42");
}

TEST_F(FieldDefnTest, Clone) {
    CNFieldDefn* original = CNFieldDefn::Create("original");
    original->SetType(CNFieldType::kReal);
    original->SetWidth(10);
    original->SetPrecision(4);
    
    CNFieldDefn* cloned = original->Clone();
    EXPECT_NE(cloned, nullptr);
    EXPECT_EQ(cloned->GetName(), "original");
    EXPECT_EQ(cloned->GetType(), CNFieldType::kReal);
    EXPECT_EQ(cloned->GetWidth(), 10);
    EXPECT_EQ(cloned->GetPrecision(), 4);
    
    delete cloned;
    original->ReleaseReference();
}

TEST_F(FieldDefnTest, CopyConstructor) {
    CNFieldDefn original;
    original.SetName("test");
    original.SetType(CNFieldType::kString);
    
    CNFieldDefn copy(original);
    EXPECT_EQ(copy.GetName(), "test");
    EXPECT_EQ(copy.GetType(), CNFieldType::kString);
}

TEST_F(FieldDefnTest, AssignmentOperator) {
    CNFieldDefn original;
    original.SetName("original");
    original.SetType(CNFieldType::kInteger64);
    
    CNFieldDefn assigned;
    assigned = original;
    
    EXPECT_EQ(assigned.GetName(), "original");
    EXPECT_EQ(assigned.GetType(), CNFieldType::kInteger64);
}

TEST_F(FieldDefnTest, AllFieldTypes) {
    CNFieldDefn defn;
    
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
        defn.SetType(type);
        EXPECT_EQ(defn.GetType(), type);
    }
}

TEST_F(FieldDefnTest, AllSubTypes) {
    CNFieldDefn defn;
    
    std::vector<CNFieldSubType> subTypes = {
        CNFieldSubType::kNone,
        CNFieldSubType::kBoolean,
        CNFieldSubType::kInt16,
        CNFieldSubType::kFloat32
    };
    
    for (auto subType : subTypes) {
        defn.SetSubType(subType);
        EXPECT_EQ(defn.GetSubType(), subType);
    }
}

TEST_F(FieldDefnTest, EmptyName) {
    CNFieldDefn defn;
    defn.SetName("");
    EXPECT_EQ(defn.GetName(), "");
}

TEST_F(FieldDefnTest, LongName) {
    CNFieldDefn defn;
    std::string longName(1000, 'a');
    defn.SetName(longName);
    EXPECT_EQ(defn.GetName(), longName);
}

TEST_F(FieldDefnTest, ZeroWidthAndPrecision) {
    CNFieldDefn defn;
    defn.SetWidth(0);
    defn.SetPrecision(0);
    
    EXPECT_EQ(defn.GetWidth(), 0);
    EXPECT_EQ(defn.GetPrecision(), 0);
}

TEST_F(FieldDefnTest, NegativeWidth) {
    CNFieldDefn defn;
    defn.SetWidth(-1);
    EXPECT_EQ(defn.GetWidth(), -1);
}

TEST_F(FieldDefnTest, LargeWidth) {
    CNFieldDefn defn;
    defn.SetWidth(1000000);
    EXPECT_EQ(defn.GetWidth(), 1000000);
}

TEST_F(FieldDefnTest, IsSame) {
    CNFieldDefn* defn1 = CNFieldDefn::Create("field1");
    defn1->SetType(CNFieldType::kInteger);
    
    CNFieldDefn* defn2 = CNFieldDefn::Create("field1");
    defn2->SetType(CNFieldType::kInteger);
    
    CNFieldDefn* defn3 = CNFieldDefn::Create("field2");
    defn3->SetType(CNFieldType::kReal);
    
    EXPECT_TRUE(defn1->IsSame(defn2));
    EXPECT_FALSE(defn1->IsSame(defn3));
    
    defn1->ReleaseReference();
    defn2->ReleaseReference();
    defn3->ReleaseReference();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
