#include <gtest/gtest.h>
#include "ogc/feature/geom_field_defn.h"
#include "ogc/geom/geometry.h"

using namespace ogc;

class GeomFieldDefnTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(GeomFieldDefnTest, DefaultConstructor) {
    CNGeomFieldDefn* defn = CreateCNGeomFieldDefn();
    EXPECT_NE(defn, nullptr);
    delete defn;
}

TEST_F(GeomFieldDefnTest, NamedConstructor) {
    CNGeomFieldDefn* defn = CreateCNGeomFieldDefn("geometry");
    EXPECT_NE(defn, nullptr);
    EXPECT_STREQ(defn->GetName(), "geometry");
    delete defn;
}

TEST_F(GeomFieldDefnTest, SetName) {
    CNGeomFieldDefn* defn = CreateCNGeomFieldDefn();
    defn->SetName("shape");
    EXPECT_STREQ(defn->GetName(), "shape");
    delete defn;
}

TEST_F(GeomFieldDefnTest, SetGeometryType) {
    CNGeomFieldDefn* defn = CreateCNGeomFieldDefn();
    
    defn->SetGeomType(GeomType::kPoint);
    EXPECT_EQ(defn->GetGeomType(), GeomType::kPoint);
    
    defn->SetGeomType(GeomType::kLineString);
    EXPECT_EQ(defn->GetGeomType(), GeomType::kLineString);
    
    defn->SetGeomType(GeomType::kPolygon);
    EXPECT_EQ(defn->GetGeomType(), GeomType::kPolygon);
    
    delete defn;
}

TEST_F(GeomFieldDefnTest, SetNullable) {
    CNGeomFieldDefn* defn = CreateCNGeomFieldDefn();
    
    defn->SetNullable(true);
    EXPECT_TRUE(defn->IsNullable());
    
    defn->SetNullable(false);
    EXPECT_FALSE(defn->IsNullable());
    
    delete defn;
}

TEST_F(GeomFieldDefnTest, Clone) {
    CNGeomFieldDefn* original = CreateCNGeomFieldDefn("original");
    original->SetGeomType(GeomType::kPolygon);
    
    CNGeomFieldDefn* cloned = original->Clone();
    EXPECT_NE(cloned, nullptr);
    EXPECT_STREQ(cloned->GetName(), "original");
    EXPECT_EQ(cloned->GetGeomType(), GeomType::kPolygon);
    
    delete cloned;
    delete original;
}

TEST_F(GeomFieldDefnTest, AllGeometryTypes) {
    CNGeomFieldDefn* defn = CreateCNGeomFieldDefn();
    
    std::vector<GeomType> types = {
        GeomType::kUnknown,
        GeomType::kPoint,
        GeomType::kLineString,
        GeomType::kPolygon,
        GeomType::kMultiPoint,
        GeomType::kMultiLineString,
        GeomType::kMultiPolygon,
        GeomType::kGeometryCollection
    };
    
    for (auto type : types) {
        defn->SetGeomType(type);
        EXPECT_EQ(defn->GetGeomType(), type);
    }
    
    delete defn;
}

TEST_F(GeomFieldDefnTest, EmptyName) {
    CNGeomFieldDefn* defn = CreateCNGeomFieldDefn();
    defn->SetName("");
    EXPECT_STREQ(defn->GetName(), "");
    delete defn;
}

TEST_F(GeomFieldDefnTest, LongName) {
    CNGeomFieldDefn* defn = CreateCNGeomFieldDefn();
    std::string longName(500, 'g');
    defn->SetName(longName.c_str());
    EXPECT_STREQ(defn->GetName(), longName.c_str());
    delete defn;
}

TEST_F(GeomFieldDefnTest, SetExtent) {
    CNGeomFieldDefn* defn = CreateCNGeomFieldDefn();
    
    defn->SetExtent(0.0, 100.0, 0.0, 100.0);
    EXPECT_DOUBLE_EQ(defn->GetXMin(), 0.0);
    EXPECT_DOUBLE_EQ(defn->GetXMax(), 100.0);
    EXPECT_DOUBLE_EQ(defn->GetYMin(), 0.0);
    EXPECT_DOUBLE_EQ(defn->GetYMax(), 100.0);
    
    delete defn;
}

TEST_F(GeomFieldDefnTest, Set2D) {
    CNGeomFieldDefn* defn = CreateCNGeomFieldDefn();
    
    defn->Set2D(true);
    EXPECT_TRUE(defn->Is2D());
    
    defn->Set2D(false);
    EXPECT_FALSE(defn->Is2D());
    
    delete defn;
}

TEST_F(GeomFieldDefnTest, SetMeasured) {
    CNGeomFieldDefn* defn = CreateCNGeomFieldDefn();
    
    defn->SetMeasured(true);
    EXPECT_TRUE(defn->IsMeasured());
    
    defn->SetMeasured(false);
    EXPECT_FALSE(defn->IsMeasured());
    
    delete defn;
}

TEST_F(GeomFieldDefnTest, DefaultNullable) {
    CNGeomFieldDefn* defn = CreateCNGeomFieldDefn();
    EXPECT_TRUE(defn->IsNullable());
    delete defn;
}
