#include <gtest/gtest.h>
#include "ogc/feature/feature_defn.h"
#include "ogc/feature/field_defn.h"
#include "ogc/feature/geom_field_defn.h"

using namespace ogc;

class FeatureDefnTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FeatureDefnTest, DefaultConstructor) {
    CNFeatureDefn* defn = CNFeatureDefn::Create();
    EXPECT_NE(defn, nullptr);
    EXPECT_EQ(defn->GetFieldCount(), 0);
    EXPECT_EQ(defn->GetGeomFieldCount(), 0);
    defn->ReleaseReference();
}

TEST_F(FeatureDefnTest, NamedConstructor) {
    CNFeatureDefn* defn = CNFeatureDefn::Create("test_feature");
    EXPECT_NE(defn, nullptr);
    EXPECT_STREQ(defn->GetName(), "test_feature");
    defn->ReleaseReference();
}

TEST_F(FeatureDefnTest, SetName) {
    CNFeatureDefn* defn = CNFeatureDefn::Create();
    defn->SetName("my_feature");
    EXPECT_STREQ(defn->GetName(), "my_feature");
    defn->ReleaseReference();
}

TEST_F(FeatureDefnTest, AddFieldDefn) {
    CNFeatureDefn* defn = CNFeatureDefn::Create();
    
    CNFieldDefn* fieldDefn = CreateCNFieldDefn("id");
    fieldDefn->SetType(CNFieldType::kInteger);
    defn->AddFieldDefn(fieldDefn);
    
    EXPECT_EQ(defn->GetFieldCount(), 1);
    
    defn->ReleaseReference();
}

TEST_F(FeatureDefnTest, AddMultipleFields) {
    CNFeatureDefn* defn = CNFeatureDefn::Create();
    
    CNFieldDefn* f1 = CreateCNFieldDefn("id");
    f1->SetType(CNFieldType::kInteger);
    
    CNFieldDefn* f2 = CreateCNFieldDefn("name");
    f2->SetType(CNFieldType::kString);
    
    CNFieldDefn* f3 = CreateCNFieldDefn("value");
    f3->SetType(CNFieldType::kReal);
    
    defn->AddFieldDefn(f1);
    defn->AddFieldDefn(f2);
    defn->AddFieldDefn(f3);
    
    EXPECT_EQ(defn->GetFieldCount(), 3);
    
    defn->ReleaseReference();
}

TEST_F(FeatureDefnTest, GetFieldDefn) {
    CNFeatureDefn* defn = CNFeatureDefn::Create();
    
    CNFieldDefn* field = CreateCNFieldDefn("test_field");
    field->SetType(CNFieldType::kString);
    defn->AddFieldDefn(field);
    
    CNFieldDefn* retrieved = defn->GetFieldDefn(0);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_STREQ(retrieved->GetName(), "test_field");
    
    defn->ReleaseReference();
}

TEST_F(FeatureDefnTest, GetFieldDefnByName) {
    CNFeatureDefn* defn = CNFeatureDefn::Create();
    
    CNFieldDefn* field = CreateCNFieldDefn("my_field");
    field->SetType(CNFieldType::kReal);
    defn->AddFieldDefn(field);
    
    int index = defn->GetFieldIndex("my_field");
    EXPECT_EQ(index, 0);
    
    defn->ReleaseReference();
}

TEST_F(FeatureDefnTest, GetFieldDefn_InvalidIndex) {
    CNFeatureDefn* defn = CNFeatureDefn::Create();
    
    CNFieldDefn* retrieved = defn->GetFieldDefn(999);
    EXPECT_EQ(retrieved, nullptr);
    
    defn->ReleaseReference();
}

TEST_F(FeatureDefnTest, GetFieldIndex_NotFound) {
    CNFeatureDefn* defn = CNFeatureDefn::Create();
    
    int index = defn->GetFieldIndex("nonexistent");
    EXPECT_EQ(index, -1);
    
    defn->ReleaseReference();
}

TEST_F(FeatureDefnTest, AddGeomFieldDefn) {
    CNFeatureDefn* defn = CNFeatureDefn::Create();
    
    CNGeomFieldDefn* geomField = CreateCNGeomFieldDefn("geometry");
    defn->AddGeomFieldDefn(geomField);
    
    EXPECT_EQ(defn->GetGeomFieldCount(), 1);
    
    defn->ReleaseReference();
}

TEST_F(FeatureDefnTest, GetGeomFieldDefn) {
    CNFeatureDefn* defn = CNFeatureDefn::Create();
    
    CNGeomFieldDefn* geomField = CreateCNGeomFieldDefn("shape");
    geomField->SetGeomType(GeomType::kPoint);
    defn->AddGeomFieldDefn(geomField);
    
    CNGeomFieldDefn* retrieved = defn->GetGeomFieldDefn(0);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_STREQ(retrieved->GetName(), "shape");
    
    defn->ReleaseReference();
}

TEST_F(FeatureDefnTest, Clone) {
    CNFeatureDefn* original = CNFeatureDefn::Create("original");
    
    CNFieldDefn* field = CreateCNFieldDefn("id");
    field->SetType(CNFieldType::kInteger);
    original->AddFieldDefn(field);
    
    CNFeatureDefn* cloned = original->Clone();
    ASSERT_NE(cloned, nullptr);
    EXPECT_STREQ(cloned->GetName(), "original");
    EXPECT_EQ(cloned->GetFieldCount(), 1);
    
    cloned->ReleaseReference();
    original->ReleaseReference();
}

TEST_F(FeatureDefnTest, Clone_Empty) {
    CNFeatureDefn* original = CNFeatureDefn::Create("empty");
    CNFeatureDefn* cloned = original->Clone();
    
    ASSERT_NE(cloned, nullptr);
    EXPECT_EQ(cloned->GetFieldCount(), 0);
    EXPECT_EQ(cloned->GetGeomFieldCount(), 0);
    
    cloned->ReleaseReference();
    original->ReleaseReference();
}

TEST_F(FeatureDefnTest, ReferenceCounting) {
    CNFeatureDefn* defn = CNFeatureDefn::Create("test");
    EXPECT_EQ(defn->GetReferenceCount(), 1);
    
    defn->AddReference();
    EXPECT_EQ(defn->GetReferenceCount(), 2);
    
    defn->ReleaseReference();
    EXPECT_EQ(defn->GetReferenceCount(), 1);
    
    defn->ReleaseReference();
}

TEST_F(FeatureDefnTest, DeleteFieldDefn) {
    CNFeatureDefn* defn = CNFeatureDefn::Create();
    
    CNFieldDefn* f1 = CreateCNFieldDefn("field1");
    f1->SetType(CNFieldType::kInteger);
    
    CNFieldDefn* f2 = CreateCNFieldDefn("field2");
    f2->SetType(CNFieldType::kString);
    
    defn->AddFieldDefn(f1);
    defn->AddFieldDefn(f2);
    
    EXPECT_EQ(defn->GetFieldCount(), 2);
    
    defn->DeleteFieldDefn(0);
    EXPECT_EQ(defn->GetFieldCount(), 1);
    
    defn->ReleaseReference();
}

TEST_F(FeatureDefnTest, ClearFieldDefns) {
    CNFeatureDefn* defn = CNFeatureDefn::Create();
    
    for (int i = 0; i < 5; ++i) {
        CNFieldDefn* field = CreateCNFieldDefn(("field" + std::to_string(i)).c_str());
        field->SetType(CNFieldType::kString);
        defn->AddFieldDefn(field);
    }
    
    EXPECT_EQ(defn->GetFieldCount(), 5);
    
    defn->ClearFieldDefns();
    EXPECT_EQ(defn->GetFieldCount(), 0);
    
    defn->ReleaseReference();
}

TEST_F(FeatureDefnTest, EmptyName) {
    CNFeatureDefn* defn = CNFeatureDefn::Create();
    defn->SetName("");
    EXPECT_STREQ(defn->GetName(), "");
    defn->ReleaseReference();
}

TEST_F(FeatureDefnTest, LongName) {
    CNFeatureDefn* defn = CNFeatureDefn::Create();
    std::string longName(500, 'a');
    defn->SetName(longName.c_str());
    EXPECT_STREQ(defn->GetName(), longName.c_str());
    defn->ReleaseReference();
}

TEST_F(FeatureDefnTest, MultipleGeomFields) {
    CNFeatureDefn* defn = CNFeatureDefn::Create();
    
    CNGeomFieldDefn* g1 = CreateCNGeomFieldDefn("geom1");
    g1->SetGeomType(GeomType::kPoint);
    
    CNGeomFieldDefn* g2 = CreateCNGeomFieldDefn("geom2");
    g2->SetGeomType(GeomType::kPolygon);
    
    defn->AddGeomFieldDefn(g1);
    defn->AddGeomFieldDefn(g2);
    
    EXPECT_EQ(defn->GetGeomFieldCount(), 2);
    
    defn->ReleaseReference();
}

TEST_F(FeatureDefnTest, GetGeomFieldIndex) {
    CNFeatureDefn* defn = CNFeatureDefn::Create();
    
    CNGeomFieldDefn* geom = CreateCNGeomFieldDefn("my_geom");
    defn->AddGeomFieldDefn(geom);
    
    int index = defn->GetGeomFieldIndex("my_geom");
    EXPECT_EQ(index, 0);
    
    int notFound = defn->GetGeomFieldIndex("nonexistent");
    EXPECT_EQ(notFound, -1);
    
    defn->ReleaseReference();
}

TEST_F(FeatureDefnTest, IsValid) {
    CNFeatureDefn* defn = CNFeatureDefn::Create("valid_feature");
    EXPECT_TRUE(defn->IsValid());
    defn->ReleaseReference();
}
