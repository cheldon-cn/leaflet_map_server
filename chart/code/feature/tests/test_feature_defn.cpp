#include "gtest/gtest.h"
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
    CNFeatureDefn defn;
    EXPECT_EQ(defn.GetFieldCount(), 0);
    EXPECT_EQ(defn.GetGeomFieldCount(), 0);
    EXPECT_EQ(defn.GetName(), "");
}

TEST_F(FeatureDefnTest, NamedConstructor) {
    CNFeatureDefn* defn = CNFeatureDefn::Create("test_feature");
    EXPECT_NE(defn, nullptr);
    EXPECT_EQ(defn->GetName(), "test_feature");
    defn->ReleaseReference();
}

TEST_F(FeatureDefnTest, SetName) {
    CNFeatureDefn defn;
    defn.SetName("my_feature");
    EXPECT_EQ(defn.GetName(), "my_feature");
}

TEST_F(FeatureDefnTest, AddFieldDefn) {
    CNFeatureDefn defn;
    
    CNFieldDefn* fieldDefn = CNFieldDefn::Create("id", CNFieldType::kInteger);
    int index = defn.AddFieldDefn(fieldDefn);
    
    EXPECT_EQ(index, 0);
    EXPECT_EQ(defn.GetFieldCount(), 1);
    
    fieldDefn->ReleaseReference();
}

TEST_F(FeatureDefnTest, AddMultipleFields) {
    CNFeatureDefn defn;
    
    CNFieldDefn* f1 = CNFieldDefn::Create("id", CNFieldType::kInteger);
    CNFieldDefn* f2 = CNFieldDefn::Create("name", CNFieldType::kString);
    CNFieldDefn* f3 = CNFieldDefn::Create("value", CNFieldType::kReal);
    
    defn.AddFieldDefn(f1);
    defn.AddFieldDefn(f2);
    defn.AddFieldDefn(f3);
    
    EXPECT_EQ(defn.GetFieldCount(), 3);
    
    f1->ReleaseReference();
    f2->ReleaseReference();
    f3->ReleaseReference();
}

TEST_F(FeatureDefnTest, GetFieldDefn) {
    CNFeatureDefn defn;
    
    CNFieldDefn* field = CNFieldDefn::Create("test_field", CNFieldType::kString);
    defn.AddFieldDefn(field);
    
    CNFieldDefn* retrieved = defn.GetFieldDefn(0);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->GetName(), "test_field");
    
    field->ReleaseReference();
}

TEST_F(FeatureDefnTest, GetFieldDefnByName) {
    CNFeatureDefn defn;
    
    CNFieldDefn* field = CNFieldDefn::Create("my_field", CNFieldType::kReal);
    defn.AddFieldDefn(field);
    
    int index = defn.GetFieldIndex("my_field");
    EXPECT_EQ(index, 0);
    
    CNFieldDefn* retrieved = defn.GetFieldDefn("my_field");
    ASSERT_NE(retrieved, nullptr);
    
    field->ReleaseReference();
}

TEST_F(FeatureDefnTest, GetFieldDefn_InvalidIndex) {
    CNFeatureDefn defn;
    
    CNFieldDefn* retrieved = defn.GetFieldDefn(999);
    EXPECT_EQ(retrieved, nullptr);
}

TEST_F(FeatureDefnTest, GetFieldIndex_NotFound) {
    CNFeatureDefn defn;
    
    int index = defn.GetFieldIndex("nonexistent");
    EXPECT_EQ(index, -1);
}

TEST_F(FeatureDefnTest, AddGeomFieldDefn) {
    CNFeatureDefn defn;
    
    CNGeomFieldDefn* geomField = CNGeomFieldDefn::Create("geometry");
    int index = defn.AddGeomFieldDefn(geomField);
    
    EXPECT_EQ(index, 0);
    EXPECT_EQ(defn.GetGeomFieldCount(), 1);
    
    geomField->ReleaseReference();
}

TEST_F(FeatureDefnTest, GetGeomFieldDefn) {
    CNFeatureDefn defn;
    
    CNGeomFieldDefn* geomField = CNGeomFieldDefn::Create("shape");
    geomField->SetGeometryType(GeomType::kPoint);
    defn.AddGeomFieldDefn(geomField);
    
    CNGeomFieldDefn* retrieved = defn.GetGeomFieldDefn(0);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->GetName(), "shape");
    
    geomField->ReleaseReference();
}

TEST_F(FeatureDefnTest, Clone) {
    CNFeatureDefn* original = CNFeatureDefn::Create("original");
    
    CNFieldDefn* field = CNFieldDefn::Create("id", CNFieldType::kInteger);
    original->AddFieldDefn(field);
    field->ReleaseReference();
    
    CNFeatureDefn* cloned = original->Clone();
    ASSERT_NE(cloned, nullptr);
    EXPECT_EQ(cloned->GetName(), "original");
    EXPECT_EQ(cloned->GetFieldCount(), 1);
    
    delete cloned;
    original->ReleaseReference();
}

TEST_F(FeatureDefnTest, Clone_Empty) {
    CNFeatureDefn* original = CNFeatureDefn::Create("empty");
    CNFeatureDefn* cloned = original->Clone();
    
    ASSERT_NE(cloned, nullptr);
    EXPECT_EQ(cloned->GetFieldCount(), 0);
    EXPECT_EQ(cloned->GetGeomFieldCount(), 0);
    
    delete cloned;
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

TEST_F(FeatureDefnTest, IsSame) {
    CNFeatureDefn* defn1 = CNFeatureDefn::Create("feature1");
    CNFeatureDefn* defn2 = CNFeatureDefn::Create("feature1");
    CNFeatureDefn* defn3 = CNFeatureDefn::Create("feature2");
    
    EXPECT_TRUE(defn1->IsSame(defn2));
    EXPECT_FALSE(defn1->IsSame(defn3));
    
    defn1->ReleaseReference();
    defn2->ReleaseReference();
    defn3->ReleaseReference();
}

TEST_F(FeatureDefnTest, RemoveFieldDefn) {
    CNFeatureDefn defn;
    
    CNFieldDefn* f1 = CNFieldDefn::Create("field1", CNFieldType::kInteger);
    CNFieldDefn* f2 = CNFieldDefn::Create("field2", CNFieldType::kString);
    
    defn.AddFieldDefn(f1);
    defn.AddFieldDefn(f2);
    
    EXPECT_EQ(defn.GetFieldCount(), 2);
    
    defn.RemoveFieldDefn(0);
    EXPECT_EQ(defn.GetFieldCount(), 1);
    
    f1->ReleaseReference();
    f2->ReleaseReference();
}

TEST_F(FeatureDefnTest, ClearFields) {
    CNFeatureDefn defn;
    
    for (int i = 0; i < 5; ++i) {
        CNFieldDefn* field = CNFieldDefn::Create("field" + std::to_string(i), CNFieldType::kString);
        defn.AddFieldDefn(field);
        field->ReleaseReference();
    }
    
    EXPECT_EQ(defn.GetFieldCount(), 5);
    
    defn.ClearFields();
    EXPECT_EQ(defn.GetFieldCount(), 0);
}

TEST_F(FeatureDefnTest, EmptyName) {
    CNFeatureDefn defn;
    defn.SetName("");
    EXPECT_EQ(defn.GetName(), "");
}

TEST_F(FeatureDefnTest, LongName) {
    CNFeatureDefn defn;
    std::string longName(500, 'a');
    defn.SetName(longName);
    EXPECT_EQ(defn.GetName(), longName);
}

TEST_F(FeatureDefnTest, MultipleGeomFields) {
    CNFeatureDefn defn;
    
    CNGeomFieldDefn* g1 = CNGeomFieldDefn::Create("geom1");
    g1->SetGeometryType(GeomType::kPoint);
    
    CNGeomFieldDefn* g2 = CNGeomFieldDefn::Create("geom2");
    g2->SetGeometryType(GeomType::kPolygon);
    
    defn.AddGeomFieldDefn(g1);
    defn.AddGeomFieldDefn(g2);
    
    EXPECT_EQ(defn.GetGeomFieldCount(), 2);
    
    g1->ReleaseReference();
    g2->ReleaseReference();
}

TEST_F(FeatureDefnTest, CopyConstructor) {
    CNFeatureDefn original;
    original.SetName("original");
    
    CNFeatureDefn copy(original);
    EXPECT_EQ(copy.GetName(), "original");
}

TEST_F(FeatureDefnTest, AssignmentOperator) {
    CNFeatureDefn original;
    original.SetName("assigned");
    
    CNFeatureDefn assigned;
    assigned = original;
    
    EXPECT_EQ(assigned.GetName(), "assigned");
}

TEST_F(FeatureDefnTest, SetGeometryType) {
    CNFeatureDefn defn;
    defn.SetGeometryType(GeomType::kPoint);
    EXPECT_EQ(defn.GetGeometryType(), GeomType::kPoint);
    
    defn.SetGeometryType(GeomType::kPolygon);
    EXPECT_EQ(defn.GetGeometryType(), GeomType::kPolygon);
}

TEST_F(FeatureDefnTest, GetGeomFieldIndex) {
    CNFeatureDefn defn;
    
    CNGeomFieldDefn* geom = CNGeomFieldDefn::Create("my_geom");
    defn.AddGeomFieldDefn(geom);
    
    int index = defn.GetGeomFieldIndex("my_geom");
    EXPECT_EQ(index, 0);
    
    int notFound = defn.GetGeomFieldIndex("nonexistent");
    EXPECT_EQ(notFound, -1);
    
    geom->ReleaseReference();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
